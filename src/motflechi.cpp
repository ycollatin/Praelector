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

// signetMotFlechi
MotFlechi::MotFlechi(Lemme* l, QString m, Mot* parent)
{
    _mot = parent;
    _lemme = l;
    _morpho = m;
    _phrase = parent->phrase();
    _traduction = l->traduction("fr");
    // traduction fléchie
    QString tr = _traduction;
    tr.remove(QRegExp("[(\\[][^)^\\]]*[)\\]]"));
    QStringList ll = tr.split(QRegExp("[,;]"));
    QStringList ltr;
    for (int i=0;i<ll.count();++i)
    {
        QString c = ll.at(i).simplified();
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
        if (!fl.isEmpty()) ltr.append(fl);
    }
    _trfl = ltr.join(", ");
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
    // 3. un triangle hyperlien vers doc
    // 3. deux hyperliens : valider rejeter
    QStringList ll;
    // trier _lreqSup et _lreqSub en fonction de leur poids
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
    return _lemme->gr()+" "+_morpho;
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
    if (f) qDebug()<<"à écrire";
    /*
    bool lelales = _traduction.startsWith ("l'")
		|| _traduction.startsWith ("le ")
		|| _traduction.startsWith ("la ")
		|| _traduction.startsWith ("les ");
	bool initVoc = QString ("aeiouâéêAEIOUÂÉÊ").contains (_trNue.at (0));
	// déterminant
	if (_postag.at (0) == 'n')
	{
		if (lelales)
		{
			_traduction = _trNue;
		}
		else if (_postag.at (2) == 'p')
		{
			if (_traduction.startsWith ("des "))
				_traduction = "les " + _trNue;
			else _traduction = "des " + _trNue;
		}
		else if (f) // féminin singulier
		{
			if (_traduction.startsWith ("une "))
			{
				if (initVoc) _traduction = "l'"+_trNue;
				else _traduction = "la "+ _trNue;
			}
			else
				_traduction = "une "+_trNue;
		}
		else  // masculin singulier
		{
			if (_traduction.startsWith ("un "))
			{
				if (initVoc) _traduction = "l'"+_trNue;
				else _traduction = "le " + _trNue;
			}
			else
			{
				_traduction = "un "+ _trNue;
			}
		}
	}
    */
}

QString MotFlechi::trfl()
{
    return _trfl;
}

void MotFlechi::setTraduction(QString t)
{
    _traduction = t;
}

