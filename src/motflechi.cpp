/*   MotFlechi.cpp  */
/* Copyright (C) 2015 Yves Ouvrard

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <QDebug>

#include <ch.h>
#include <flexfr.h>
#include <lemmatiseur.h>
#include <motflechi.h>
#include <regle.h>
#include <requete.h>

/**********************
      Classe MotFlechi
 **********************/

MotFlechi::MotFlechi(Lemme* l, QString m, Mot* parent)
{
    _mot = parent;
    _lemme = l;
    _morpho = m;
    _phrase = parent->phrase();
    // traduction fléchie
    QString tr = l->traduction("fr");
    tr.remove(QRegExp("[(\\[][^)^\\]]*[)\\]]"));
    _traductions = tr.split(QRegExp("[,;]"));
    for (int i=0;i<_traductions.count();++i)
    {
        QString c = _traductions.at(i).simplified();
        QString fl;
        switch (_lemme->pos().at(0).toLatin1())
        {
            case 'p':
            case 'a': fl = accorde(c, _morpho); break;
            case 'n':
                      {
                          if (_morpho.contains("plur")) fl = pluriel(c, _morpho);
                          else fl = c;
                          break;
                      }
            //case 'p': ret = _pronom->accorde(c, _morpho); break;
            case 'v':
            case 'w':
                      fl = conjnat(c, _morpho);
                      break;
            default: fl = c;
        }
        if (!fl.isEmpty()) _trfl.append(fl);
    }
    _itr = -1;
    if (!_trfl.empty())
    {
        _trNue = _trfl.at(0);
        _tr = _trNue;
    }
}

void MotFlechi::ajReq(Requete* req)
{
    if (req->subRequis()) _lreqSub.append(req);
    else _lreqSup.append(req);
}

bool MotFlechi::aUnSuper()
{
    for (int i=0;i<_lreqSub.count();++i)
        if (_lreqSub.at(i)->close()) return true;
    return false;
}

bool MotFlechi::auxiliaire()
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close() && (req->id() == "ppp" || req->id() == "parFut"))
            return true;
    }
    return false;
}

QList<Requete*> MotFlechi::closes()
{
    QList<Requete*> ret;
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close()) ret.append(req);
    }
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close()) ret.append(req);
    }
    return ret;
}

/*
QString MotFlechi::conj()
{
    QString tr = _traduction;
	QString ret = conjnat(_tr, _morpho);
	QString ajout = "";
	int x = _tr.indexOf (' ');
	if ((x > -1) && !_tr.startsWith ("se "))
	{
		ajout = tr.section (' ', 1);
		ajout.prepend (" ");
		//_tr.chop (tr.length()-x);
	}
	// adjectif verbal
	if (_morpho.contains("verbal"))
		ret.prepend ("devant être ");
	return ret;
}
*/

bool MotFlechi::contigu(MotFlechi* mf)
{
    return abs(rang() - mf->rang()) == 1;
}

void MotFlechi::delSubs()
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close()) req->annuleRequis("sub ANNULÉ");
    }
}

void MotFlechi::delSups()
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close()) req->annuleRequis("sub ANNULÉ");
    }
}

// _lreqSub est la liste des requêtes dont le fléchi est sub
// _lreqSup est la liste des requêtes dont le fléchi est super

bool MotFlechi::estSub()
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close()) return true;
    }
    return false;
}

bool MotFlechi::estSubParAff(QString aff)
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close() && req->aff() == aff)
            return true;
    }
    return false;
}

bool MotFlechi::estSubParId(QString id)
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close() && req->id() == id)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperDe(MotFlechi* m)
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close() && req->sub() == m)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperParAff(QString aff)
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close() && req->aff() == aff)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperParId(QString id)
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close() && req->id() == id)
            return true;
    }
    return false;
}

bool MotFlechi::estVerbe()
{
    return (_lemme->pos().contains("v")
            || _lemme->pos().contains("w"));
}

int MotFlechi::freq()
{
    return _lemme->freq();
}

QString MotFlechi::gr()
{
    return _mot->gr();
}

int MotFlechi::handicap()
{
    int ret = 0;
    ret += _phrase->handicap(this);
    // handicaps morpho
    if (_morpho.contains("vocatif")) ret += 50;
    else if (gr().endsWith("is"))
    {
        if (_morpho.contains("accusatif pluriel")) ret += 22;
        else if (_lemme->gr().endsWith("ia")) ret += 20;
    }
    int mfl = _mot->mfLies().count();
    ret -= mfl;
    return ret;
}

QString MotFlechi::htmlLiens()
{
    // format : 
    // 1. en noir, fonction 
    // 2. en bleu, traduction
    // 3. un hyperlien vers doc
    // 3. deux hyperliens : valider rejeter
    QStringList ll;
    // TODO trier _lreqSup et _lreqSub en fonction de leur poids
    // 1. liens dont le mot est sub :
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close()) ll.append(req->fonction());
    }
    // 1. liens dont le mot est super :
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close()) ll.append(req->fonction());
    }
    return ll.join("<br/>");
}

void MotFlechi::incItr()
{
    if (_itr < 0) 
    {
        if (_trfl.count() == 1) _itr = 0;
        else _itr = 1;
    }
    else ++_itr;
    if (_itr >= _trfl.count()) _itr = 0;
    _trNue = _trfl.at(_itr);
    _tr = _trNue;
}

bool MotFlechi::intersect(QString a, QString b)
{
    // ajouté 7 février 2018 pour coord2.
    if (a.contains('n')) a.append("p");
    else if (a.contains('p')) a.append("n");
    for (int i=0;i<a.size();++i)
        if (b.contains(a.at(i))) return true;
    return false;
}

void MotFlechi::lance()
{
    for (int i=0;i<_phrase->nbRegles();++i)
    {
        Regle* r = _phrase->regle(i);
        if (r->estSuper(this) && r->sens() != '<') lanceReqSup(r);
        if (r->estSub(this) && r->sens() != '>') lanceReqSub(r);
    }
}

// _lreqSub est la liste des requêtes dont le fléchi est sub
// _lreqSup est la liste des requêtes dont le fléchi est super
Requete* MotFlechi::lanceReqSub(Regle* r)
{
    Requete* nr = new Requete(0, this, r);
    _phrase->ajRequete(nr);
    _lreqSub.append(nr);
    nr->ajHist("\n------------------\nRequête numéro "+nr->numc());
    nr->ajHist(nr->humain());
    nr->ajHist(nr->doc());
    return nr;
}

Requete* MotFlechi::lanceReqSup(Regle* r)
{
    Requete* nr = new Requete(this, 0, r);
    _phrase->ajRequete(nr);
    _lreqSup.append(nr);
    nr->ajHist("\n------------------\nRequête numéro "+nr->numc());
    nr->ajHist(nr->humain());
    return nr;
}

Lemme* MotFlechi::lemme()
{
    return _lemme;
}

QString MotFlechi::morpho()
{
    return _lemme->gr()+" "+_lemme->pos()+" "+_morpho;
}

Mot* MotFlechi::mot()
{
    return _mot;
}

int MotFlechi::nbCloses()
{
    int ret = 0;
    for (int i=0;i<_lreqSub.count();++i)
        if (_lreqSub.at(i)->close()) ++ret;
    for (int i=0;i<_lreqSup.count();++i)
    {
        if (_lreqSup.at(i)->close()) ++ret;
    }
    return ret;
}

int MotFlechi::nbReqSub()
{
    return _lreqSub.count();
}

int MotFlechi::nbReqSup()
{
    return _lreqSup.count();
}

int MotFlechi::nbReqSupCloses()
{
    int ret = 0;
    for (int i=0;i<_lreqSup.count();++i)
        if (_lreqSup.at(i)->close()) ++ret;
    return ret;
}

void MotFlechi::nettoie()
{
    // nettoyer les subs :
    for (int i=0;i<_mot->nbFlechis();++i)
    {
        MotFlechi* mf = _mot->flechi(i);
        if (mf == this) continue;
        for (int ir=0;ir<mf->nbReqSup();++ir)
        {
            Requete* req = mf->reqSup(ir);
            if (req->close() && (req->aff() == "epithete"))
            {
                req->annuleRequis("accord rompu par "+req->doc());
            }
        }
    }
}

int MotFlechi::rang()
{
    return _mot->rang();
}

Requete* MotFlechi::reqSub(int i)
{
    return _lreqSub.at(i);
}

Requete* MotFlechi::reqSub(QString id)
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close() && req->id() == id)
            return req;
    }
    return 0;
}

Requete* MotFlechi::reqSup(int i)
{
    return _lreqSup.at(i);
}

bool MotFlechi::resout(Requete* req)
{
    // signetResout
    // un mot ne peut être lié à lui-même.
    if (req->requerant() == _mot) return false;

    // pas de sujet séparé de son verbe par le seul mot /quam/
    if (this->rang() - req->requerant()->rang() == 2)
    {
        Mot* m = _phrase->motNo(this->rang() - 1);
        if (req->aff() == "sujet" && m->gr()=="quam")
            return false;
        if (req->aff() == "epithete")
        {
            if ((QStringList()<<"est"<<"fuit"<<"erat"
                 <<"sunt"<<"fuerunt"<<"erant").contains(m->gr())) return false;
        }
    }

    // position initiale obligatoire
    if (req->regle()->filtre().contains("sup0") && req->subRequis() && req->super()->rang() != 0)
        return false;

    // locatif
    if (req->id()=="locatif"
        && ((req->subRequis() && !_lemme->synt("loc"))
            || (req->superRequis() && !req->sub()->lemme()->synt("loc"))))
        return false;

    // contiguïté
    if (req->regle()->filtre().contains("contigu") && (rang() - req->requerant()->rang() != 1))
        return false;
    if (req->regle()->filtre().contains("tete") && req->subRequis() && rang() > 0)
        return false;

    // liens associés
    if (!req->regle()->subEstSup().isEmpty()
        && ((req->subRequis() && !estSuperParId(req->regle()->subEstSup()))
            || (req->superRequis() && !req->sub()->estSuperParId(req->regle()->subEstSup()))))
            return false;

    if (!req->regle()->supEstSub().isEmpty() && req->super() != 0)
    {
        if (!req->super()->estSubParId(req->regle()->supEstSub()))
        {
            return false;
        }
    }
    if (!req->regle()->supEstSup().empty() && req->super() != 0)
    {
        bool passe = false;
        for (int i=0;i<req->regle()->supEstSup().count();++i)
            passe = passe || (req->super()->estSuperParId(req->regle()->supEstSup().at(i)));
        if (!passe) return false;
    }

    // Le super est requis
    QString accord = req->regle()->accord();
    if (req->superRequis() && req->sens() != '>')
    {
        // un super en -que ne peut avoir de sub qui le précède
        if (_mot->que() && rang() > req->sub()->rang())
            return false;
        if (!accord.isEmpty() && !_phrase->accord(this, req->sub(), accord))
        {
            return false;
        }
        if (req->regle()->estSuper(this))
        {
            return true;
        }
    }
    // le sub est requis
    else if (req->subRequis() && req->sens() != '<')
    {
        // un mot en -que (== et + mot) ne peut être que coordonné
        if  (_mot->que() && req->aff() != "coord") return false;
        // un super en -que ne peut avoir de sub qui le précède
        if (req->super()->mot()->que() && rang() < req->super()->rang())
            return false;
        if (!accord.isEmpty())
        {
            if (req->aff() == "coord2")
            {
                QList<Requete*> lrs = _phrase->reqCC(req->super()->mot());
                bool ac = false;
                for (int irs=0;irs<lrs.count();++irs)
                {
                    Requete* rs = lrs.at(irs);
                    if (intersect(rs->super()->lemme()->pos(), _lemme->pos())
                        && _phrase->accord(this, rs->super(), accord))
                    {
                        req->setCoord1(rs);
                        ac = true;
                    }
                }
                if (!ac) return false;
            }
            else if (!accord.isEmpty() && !(_phrase->accord(this, req->super(), accord)))
                return false;
        }
        // règle
        if (req->regle()->estSub(this))
        {
            // filtres
            if (req->regle()->filtre().contains("que") && !_mot->que())
                return false;
            return true;
        }
    }
    return false;
} // resout

void MotFlechi::setDet(bool f)
{
    bool zero = _tr == _trNue;
    bool indef = _tr.startsWith ("un ") || _tr.startsWith ("une ") || _tr.startsWith ("des ");
	bool initVoc = QString ("aehiouâéêAEHIOUÂÉÊ").contains (_trNue.at (0));
    bool plur = _morpho.contains("plur");
	// déterminant
	if (_lemme->pos().contains('n'))
	{
        if (zero) // passer à indef
        {
            if (plur) _tr.prepend("des ");
            else if (f) _tr.prepend("une ");
            else _tr.prepend("un ");
        }
		else if (indef) // passer à def
		{
            _tr = _trNue;
		    if (plur) _tr.prepend("les ");
            else if (initVoc) _tr.prepend("l'");
            else if (f) _tr.prepend("la ");
            else _tr.prepend("le ");
		}
        else _tr = _trNue;
	}
}

/* rotation du sujet des formes v. à la 3ème pers */
void MotFlechi::setSujet()
{
    bool zero = _tr == _trNue;
    bool plur = _morpho.contains("plur");
    if (plur)
    {
        if (zero) _tr.prepend("elles ");
        else if (_tr.startsWith("elles "))
            _tr = "ils " + _trNue;
        else _tr = _trNue;
    }
    else
    {
        if (zero) _tr.prepend("elle ");
        else if (_tr.startsWith("elle "))
            _tr = "il " + _trNue;
        else _tr = _trNue;
    }
}

void MotFlechi::setTr(QString t)
{
    _tr = t;
}

bool MotFlechi::sommet()
{
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (req->close()) return false;
    }
    return true;
}

Requete* MotFlechi::sub(QString id)
{
    for (int i=0;i<_lreqSup.count();++i)
    {
        Requete* req = _lreqSup.at(i);
        if (req->close() && req->id() == id)
            return req;
    }
    return 0;
}

QString MotFlechi::tr()
{
    if (!_tr.isEmpty()) return _tr;
    return trfl();
}

QString MotFlechi::trfl()
{
    if (_trfl.count() == 1)
        return _trfl.at(0);
    QStringList l = _trfl;
    QString ret = l.takeFirst()+" (";
    ret += l.join(", ");
    ret += ")";
    return ret;
}

QString MotFlechi::trGroupe()
{
    QString lp = _lemme->pos();
    QString ret;
    QTextStream fl(&ret);
    QStringList lgr;
    if (lp.contains("n")) lgr
            << "det"
            << "-" 
            << "epithete"
            << "genitif"
            << "datif"
            << "antecedent"
            << "nomQue";
    else if (lp.contains("v")) lgr
            << "sujet"
            << "-" 
            << "objet"
            << "abl"
            << "datif"
            << "prep";
    else if (lp.contains("a")) lgr 
            << "-"
            << "genitif"
            << "datif";
    else if (lp.contains("r")) lgr
            << "-"
            << "regimeAbl"
            << "regimeAcc";

    for (int i = 0;i<lgr.count();++i)
    {
        QString el = lgr.at(i);
        if (el == "-") 
        {
            fl << _tr << " ";
        }
        else 
        {
            Requete* r = sub(el);
            if (r != 0)
            {
                fl << r->trSub() << " ";
            }
        }
    }
    return ret.simplified();
}

QString MotFlechi::trNue()
{
    return _trNue;
}

void MotFlechi::videReq()
{
    for (int i=0;i<_lreqSub.count();++i)
        _lreqSub.at(i)->annuleRequis("fléchi rejeté");
    for (int i=0;i<_lreqSup.count();++i)
        _lreqSup.at(i)->annuleRequis("fléchi rejeté");
}
