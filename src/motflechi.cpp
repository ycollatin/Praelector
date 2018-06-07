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

MotFlechi::MotFlechi(Lemme* l, char p, QString m, Mot* parent)
{
    _mot = parent;
    _lemme = l;
    _morpho = m;
    _phrase = parent->phrase();
    _pos = p;
    QString tr;
    // traduction fléchie
    tr = l->traduction("fr", _pos);
    tr.remove(QRegExp("[(\\[][^)^\\]]*[)\\]]"));
    _traductions = tr.split(QRegExp("[,;]"));
    //_traductions.removeDuplicates();
    _rejete = false;
    _valide = false;
    _neg    = false;
    // _trfl  : liste de traductions fléchies
    // _trNue : traduction courante sans déterminant ni sujet
    // _tr    : traduction courante
    for (int i=0;i<_traductions.count();++i)
    {
        QString c = _traductions.at(i).simplified();
        QString fl = c;
        switch(_pos)
        {
            case 'p':
                if (l->gr()=="je")
                {
                    if (m.contains("acc"))
                    {
                        fl = "me";
                    }
                    else if (!m.contains("nomin"))
                        fl = "moi";
                }
                else if (l->gr()=="tu")
                {
                    if (m.contains("acc"))
                    {
                        fl = "te";
                    }
                    else if (!m.contains("nomin"))
                    {
                        fl = "toi";
                    }
                }
                else
                {
                    Pronom* pr = new Pronom();
                    fl = pr->accorde(c, _morpho);
                    delete pr;
                }
                break;
            case 'a': fl = accorde(c, _morpho); break;
            case 'n':
                      {
                          if (_morpho.contains("plur")) fl = pluriel(c, _morpho);
                          else fl = c;
                          break;
                      }
            case 'v':
            case 'w':
                      {
                          fl = conjnat(c, _morpho);
                          QString mcond = _morpho;
                          mcond.replace("subjonctif imparfait", "conditionnel présent");
                          mcond.replace("impératif futur", "impératif présent");
                          if (mcond != _morpho) _trfl.append(conjnat(c, mcond));
                          break;
                      }
            default: fl = c;
        }
        if (!fl.isEmpty()) _trfl.append(fl);
    }
    _trfl.removeDuplicates();

    if (!_trfl.isEmpty())
    {
        _trNue = _trfl.at(0);
        _tr = _trNue;
    }
}

MotFlechi::~MotFlechi()
{
    _traductions.clear();
    _trfl.clear();
}

bool MotFlechi::contigu(MotFlechi* mf)
{
    return abs(rang() - mf->rang()) == 1;
}

QString MotFlechi::elideFr(QString s)
{
    QString vv = "[aeéioôu]";
    // ta affaire -> ton affaire, sa->son
    s.replace(QRegularExpression("(^|\\s)[st]a ("+vv+")"), "\\1ton \\2");
    // je->j'  te->t'  me->m' ne->n' se->s'
    s.replace(QRegularExpression("(^|\\s)([jtmns])e ("+vv+")"), "\\1\\2'\\3");
    // le->l'  la->l' 
    s.replace(QRegularExpression("(^|\\s)(l[ea]) ("+vv+")"), "\\1\\2'\\3");
    // de les->des XXX le regret *de les* quitter
    s.replace(QRegularExpression("(^|\\s)de les "), "\\1des ");
    // de le->du
    s.replace(QRegularExpression("(^|\\s)de le "), "\\1du ");
    // de un->d'un
    s.replace(QRegularExpression("(^|\\s)de ("+vv+")"), "\\1d'\\2");
    // que il|elle->qu'il|elle
    s.replace(QRegularExpression("(^|\\s)que ("+vv+")"), "\\1qu'\\2");
    // pronoms objet 
    s.replace(QRegularExpression("(^|\\s)(me|te|la|le|se|les) (je|tu|il|elle|on|nous|vous|ils|elles) "), "\\1\\3 \\2 ");
    // pronoms datif
    s.replace(QRegularExpression("(lui|leurs?|me|te|nous|vous) (ils?|elles?|on)"), "\\2 \\1");
    // à le à les
    s.replace(QRegularExpression("(^|\\s)à le "), "\\1au ");
    s.replace(QRegularExpression("(^|\\s)à les "), "\\1aux ");

    // je te n'aime pas, je le n'aime pas etc.
    s.replace(QRegularExpression("(je|tu|il|elle) ([tl])[ea] n'"), "\\1 ne \\2'");

    return s;
}

bool MotFlechi::estSub()
{
    QList<Requete*> lreqSub = _phrase->lReqSub(this);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req != 0 && req->valide()) return true;
    }
    return false;
}

bool MotFlechi::estSubParAff(QString aff)
{
    QList<Requete*> lreqSub = _phrase->lReqSub(this);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req->close() && req->aff() == aff)
            return true;
    }
    return false;
}

bool MotFlechi::estSubParId(QString id)
{
    QList<Requete*> lreqSub = _phrase->lReqSub(this);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req->close() && req->id() == id)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperDe(MotFlechi* m)
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
        if (req->close() && req->sub() == m)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperParAff(QString aff)
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
        if (req->close() && req->aff() == aff)
            return true;
    }
    return false;
}

bool MotFlechi::estSuperParId(QString id)
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
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
    QList<Requete*> lreqSub = _phrase->lReqSub(this);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req->close()) ll.append(req->fonction());
    }
    // 2. liens dont le mot est super :
    QList<Requete*> lreqSup = _phrase->lReqSup(this);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
        if (req->close()) ll.append(req->fonction());
    }
    return ll.join("<br/>");
}

void MotFlechi::incItr()
{
    // XXX _trfl moins riche que _traductions
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
        if (r->estSuper(this) && r->sens() != '<')
        {
            Requete* nr = new Requete(this, 0, r);
            _phrase->ajRequete(nr);
        }
        if (r->estSub(this) && r->sens() != '>')
        {
            Requete* nr = new Requete(0, this, r);
            _phrase->ajRequete(nr);
        }
    }
}

Lemme* MotFlechi::lemme()
{
    return _lemme;
}

QString MotFlechi::morpho()
{
    return _lemme->gr()+" <small>"+_pos+" "+ _morpho+"</small>";
}

Mot* MotFlechi::mot()
{
    return _mot;
}

int MotFlechi::nbCloses()
{
    return _phrase->lReqSub(this, true).count()
        + _phrase->lReqSup(this, true).count();
}

int MotFlechi::nbReqSub()
{
    return _phrase->lReqSub(this, true).count();
}

int MotFlechi::nbReqSup()
{
    return _phrase->lReqSup(this).count();
}

int MotFlechi::nbSuper()
{
    return _phrase->nbSuper(this);
}

int MotFlechi::nbReqSupCloses()
{
    return _phrase->lReqSup(this).count();
}

int MotFlechi::nbReqSupValides(Requete* rtest)
{
    int ret = 0;
    for (int i=0;i<_phrase->nbRequetes();++i)
    {
        Requete* req = _phrase->requete(i);
        if (req != 0 && (req->valide() || req == rtest) && req->super() == this)
            ++ret;
    }
    return ret;
}

bool MotFlechi::neg()
{
    return _neg;
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

char MotFlechi::pos()
{
    return _pos;
}

int MotFlechi::rang()
{
    return _mot->rang();
}

bool MotFlechi::rejete()
{
    return _rejete;
}

Requete* MotFlechi::reqSub(int i)
{
    return _phrase->lReqSub(this).at(i);
}

Requete* MotFlechi::reqSub(QString id)
{
    QList<Requete*> lreqSub = _phrase->lReqSub(this, true);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req->id() == id)
            return req;
    }
    return 0;
}

Requete* MotFlechi::reqSup(int i)
{
    return _phrase->lReqSup(this).at(i);
}

bool MotFlechi::resout(Requete* req)
{
    // un mot ne peut être lié à lui-même.
    if (req->requerant() == _mot) return false;
    if (req->rejetee()) return false;

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
    // TODO : redondance avec ::elidefr ?
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
		} // pas de déterminant
        else _tr = _trNue;
	}
}

void MotFlechi::setNeg(bool n)
{
    _neg = n;
}

void MotFlechi::setRejete(bool r)
{
    _rejete = r;
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

void MotFlechi::setValide(bool v)
{
    _valide = v;
}

bool MotFlechi::sommet()
{
    QList<Requete*> lreqSub = _phrase->lReqSub(this, true);
    for (int i=0;i<lreqSub.count();++i)
    {
        Requete* req = lreqSub.at(i);
        if (req->close()) return false;
    }
    return true;
}

Requete* MotFlechi::sub(QString id, Requete* rtest)
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this, true);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
        if (req != 0 && (req->valide() || req == rtest) && req->id() == id)
        {
            return req;
        }
    }
    return 0;
}

QString MotFlechi::tr()
{
    if (_tr.isEmpty()) return trfl();
    return _tr;
}

QString MotFlechi::trfl()
{
    if (_trfl.count() == 1)
        return _trfl.at(0);
    QStringList l = _trfl;
    if (_trfl.isEmpty())
    {
        return "traduction manquante pour "+_lemme->gr();
    }
    QString ret = l.takeFirst()+" (";
    ret += l.join(", ");
    ret += ")";
    return ret;
}

/**
 * QString MotFlechi::trGroupe(Requete* rtest, QString morph)
 * Permet de réordonner les groupes en suivant les
 * règle syntagmatiques du français, pour aboutir à une
 * traduction la plus correcte possible.
 */
QString MotFlechi::trGroupe(Requete* rtest, QString morph)
{
    QStringList lret;
    QStringList lgr = _phrase->lgr(_pos);
    int inoyau = -1;

    for (int i = 0;i<lgr.count();++i)
    {
        QString el = lgr.at(i);
        if (el == "-")
        {
            QString trf;
            if (morph.isEmpty()) trf = _tr;
            else
            {
                trf = conjnat(_trNue, morph);
            }
            if (_neg)
            {
                // introduire le premier élément négatif en 
                // seconde position, après le pronom sujet 
                // ou après l'auxiliaire.
                if (trf.contains("'"))
                    trf.insert(trf.indexOf("'"), "e n");
                else if (trf.contains(" ")) 
                    trf.insert(trf.indexOf(" "), " ne");
                    //trf.insert(trf.indexOf(QRegularExpression("('| )")), " ne");
                // ne j'aime
                else trf.prepend("ne ");
            }
            lret.append (trf);
            inoyau = lret.count()-1;
        }
        else
        {
            Requete* r = sub(el, rtest);
            if (r != 0)
            {
                // relatifs
                if (el=="antecedent" || el=="isqui")
                {
                    int nbsup = _phrase->nbSuper(r->sub());
                    if (nbsup == 2)
                    {
                        MotFlechi* mfv = _phrase->vbRelative(r->sub());
                        if (mfv != 0)
                        {
                            lret.append(mfv->trGroupe());
                        }
                    }
                    else
                    {
                        lret.append(r->trSub());
                    }
                }
                else
                {
                    // placer les pronoms antéposés avant leur verbe
                    if (Ch::anteposes.contains(r->sub()->tr())
                        && (inoyau > -1)
                        && (el == "objet" || el == "datif"))
                    {
                        lret.insert(inoyau, r->trSub());
                        // TODO : si un pronom sujet a été ajouté, insérer
                        // l'objet après ce pronom.
                    }
                    else
                    {
                        lret.append(r->trSub());
                    }
                }
            }
        }
    }
    return elideFr(lret.join(" ").simplified());
}

QString MotFlechi::trNue()
{
    return _trNue;
}

bool MotFlechi::valide()
{
    return _valide;
}
