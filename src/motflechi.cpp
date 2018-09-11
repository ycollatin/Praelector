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

MotFlechi::MotFlechi(Lemme* l, char p, QString m, Mot* parent, char po)
{
    _mot = parent;
    _lemme = l;
    _det.clear();
    QTextStream(&_morpho) << p << " " << m;
    _phrase = parent->phrase();
    _pos = p;
    if (po == 0x00)  
        _posO = p;
    else _posO = po;

    // fixer _eqivPos
    // adjectifs substantivables
    if (p == 'a' && _lemme->synt("posn"))
        _eqivPos.append('n');
    // participes présents
    if ((p == 'v' || p == 'w')
        && (_lemme->synt("ppr") || _lemme->synt("ppp")))
    {
        _eqivPos.append('n');
    }
    // participes
    if ((p == 'v' || p == 'w') && m.contains("participe"))
            _eqivPos.append('a');

    // traduction fléchie
    QString tr;
    tr = l->traduction("fr", _pos);
    tr.remove(QRegExp("[(\\[][^)^\\]]*[)\\]]"));
    _traductions = tr.split(QRegExp("[,;]"));
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
        // le pos d'un fléchi peut différer du pos d'origine (ppr, posn, etc.)
        // mais la flexion reste celle du pos d'origine.
        switch(_posO) 
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
            case 'a': fl = _phrase->flechisseur()->accorde(c, _morpho); break;
            case 'n':
                      {
                          if (_morpho.contains("plur")) fl = _phrase->flechisseur()->pluriel(c, _morpho);
                          else fl = c;
                          break;
                      }
            case 'v':
            case 'w':
                      {
                          // Un verbe est souvent traduit par plusieurs mots.
                          // praesto:l'emporter sur, être garant, fournir (praestat : imp. : il vaut mieux)
                          // Il faut pouvoir trouver quel est le verbe, ne
                          // conjuguer que lui, et le remettre à sa place.
                          // infinitif, adjectif verbal :
                          if (_morpho.contains("adjectif verbal"))
                          {
                              fl = "à "+c;
                              break;
                          }
                          else if (_morpho.contains("supin"))
                          {
                              fl = "pour "+c;
                              break;
                          }
                          if (_lemme->gr() == "venio" && Ch::deramise(gr().toLower()) == "uenitur")
                          {
                              fl = "on vient";
                              break;
                          }
                          fl = _phrase->flechisseur()->conjnat(c, morphoLatFr(_morpho));
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

void MotFlechi::ajTrfl(QString t)
{
    _trfl.append(t);
    _itr = _trfl.count() - 2;
    incItr();
}

bool MotFlechi::contigu(MotFlechi* mf)
{
    return abs(rang() - mf->rang()) == 1;
}

QString MotFlechi::det()
{
    if (!_det.isEmpty())
        return _det+" ";
    return "";
}

QString MotFlechi::elideFr(QString s)
{
    QString vv = "[aeéêioôu]";
    // ta affaire -> ton affaire, sa->son
    s.replace(QRegularExpression("(\\b)[st]a ("+vv+")"), "\\1ton \\2");
    // je->j'  te->t'  me->m' ne->n' se->s'
    s.replace(QRegularExpression("(\\b)([jtmns])e ("+vv+")"), "\\1\\2'\\3");
    // le->l'  la->l' 
    s.replace(QRegularExpression("(\\b)([ld])[ea] ("+vv+")"), "\\1\\2'\\3");
    // de les->des XXX le regret *de les* quitter
    s.replace(QRegularExpression("(\\b)de les "), "\\1des ");
    // de le->du
    s.replace(QRegularExpression("(\\b)de le "), "\\1du ");
    // de un->d'un
    s.replace(QRegularExpression("(\\b)de ("+vv+")"), "\\1d'\\2");
    // que il|elle->qu'il|elle
    s.replace(QRegularExpression("(\\b)que ("+vv+")"), "\\1qu'\\2");
    // pronoms objet 
    s.replace(QRegularExpression("(\\b)(me|te|la|le|se|les) (je|tu|il|elle|on|nous|vous|ils|elles) "), "\\1\\3 \\2 ");
    // pronoms datif
    s.replace(QRegularExpression("\\b(lui|leurs?|me|te|nous|vous) (ils?|elles?|on)"), "\\2 \\1");
    // à le à les
    s.replace(QRegularExpression("à le "), "au ");
    s.replace(QRegularExpression("à les "), "aux ");
    // ce est
    s.replace(QRegularExpression("(\\b)ce est(\\b)"), "\\1c'est\\2");
    // je te n'aime pas, je le n'aime pas etc.
    s.replace(QRegularExpression("(je|tu|il|elle) ([tl])[ea] n'"), "\\1 ne \\2'");

    return s;
}

QString MotFlechi::eqiv()
{
    return _eqivPos;
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

bool MotFlechi::estSuper()
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this);
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
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
        if (req->valide() && req->sub() == m)
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
        if (req->valide() && req->aff() == aff)
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

int MotFlechi::itr()
{
    return _itr;
}

void MotFlechi::lance()
{
    // les prépositons et les conjsub ne peuvent avoir qu'un sub
    if ((_pos == 'r' || _pos == 'c') && estSuper())
        return;
    for (int i=0;i<_phrase->nbRegles();++i)
    {
        Regle* r = _phrase->regle(i);
        if (r->estSuper(this) && r->sens() != '<'
            && (!r->multi() || !estSuperParAff(r->aff())))
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
    return _morpho;
}

QString MotFlechi::morphoHtml()
{
    return _lemme->gr()+" <small>"+_morpho+"</small>";
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

int MotFlechi::nbTr()
{
    return _trfl.count();
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

char MotFlechi::posO()
{
    return _posO;
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
            else if (!_phrase->accord(this, req->super(), accord))
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

/*
    \fn void MotFlechi::setDet(bool f)
    \brief rotation du déterminant. f = féminin
 */
void MotFlechi::setDet(bool f)
{
    bool zero = _det.isEmpty();
    bool indef = _det == "un" || _det == "une" || _det == "des";
    bool plur = _morpho.contains("plur");

	// déterminant
    if (_pos == 'n')
	{
        if (zero) // passer à indef
        {
            if (plur) _det = "des";
            else if (f) _det = "une";
            else _det = "un";
        }
		else if (indef) // passer à def
		{
		    if (plur) _det = "les";
            else if (f) _det = "la";
            else _det = "le";
		} // pas de déterminant
        else _det.clear();
	}
}

void MotFlechi::setItr(int i)
{
    _itr = i;
    if (_itr < 0 || _itr >= _trfl.count()) return; 
    _trNue = _trfl.at(_itr);
    _tr = _trNue;
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
    // TODO
    // vérifier que t ne fait pas déjà partie des traductions
    // l'ajouter en fin de liste des traductions
    // régler itr
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

QList<Requete*> MotFlechi::sub(QString id, Requete* rtest)
{
    QList<Requete*> lreqSup = _phrase->lReqSup(this, true);
    QList<Requete*> ret;
    for (int i=0;i<lreqSup.count();++i)
    {
        Requete* req = lreqSup.at(i);
        if (req != 0 && (req->valide() || req == rtest) && req->id() == id)
        {
            ret.append(req);
        }
    }
    return ret;
}

QString MotFlechi::tr()
{
    if (_tr.isEmpty()) return trfl();
    return _tr;
}

QString MotFlechi::trfl()
{
    if (_trfl.isEmpty())
    {
        return "traduction manquante pour "+_lemme->gr();
    }
    if (_trfl.count() == 1)
        return _trfl.at(0);
    QStringList l = _trfl;
    QString ret = l.takeFirst()+" (";
    ret += l.join(", ");
    ret += ")";
    return ret;
}

QString MotFlechi::trfl(int i)
{
    if (_trfl.isEmpty())
    {
        return "traduction manquante pour "+_lemme->gr();
    }
    if (_trfl.count() == 1)
        return _trfl.at(0);
    QStringList l = _trfl;
    return _trfl.at(i);
}

/**
 * QString MotFlechi::trGroupe(Requete* rtest, QString morph)
 * Permet de réordonner les groupes en suivant les
 * règle syntagmatiques du français, pour aboutir à une
 * traduction la plus correcte possible.
 */
QString MotFlechi::trGroupe(Requete* rtest)
{
    QStringList lante;
    QStringList lpost;
    bool ante = true;
    QStringList lgr = _phrase->lgr(_pos);
    QString rel;
    //int inoyau = -1;
    for (int i = 0;i<lgr.count();++i)
    {
        QString el = lgr.at(i);
        if (el == "det" && !_det.isEmpty())
        {
            lante.append(_det);
        }
        // relatif en tête
        else if (el == "-")
        {
            QString trf = _tr;
            if (_neg)
            {
                // introduire le premier élément négatif en 
                // seconde position, après le pronom sujet 
                // ou après l'auxiliaire.
                if (trf.contains("'"))
                    trf.insert(trf.indexOf("'"), "e n");
                else if (trf.contains(" ")) 
                    trf.insert(trf.indexOf(" "), " ne");
                else trf.prepend("ne ");
            }
            lante.append (trf);
            ante = false;
        }
        else
        {
            QList<Requete*> lr = sub(el, rtest);
            for (int j=0;j<lr.count();++j)
            { 
                Requete* r = lr.at(j);
                // lien antécédent 
                if (el=="antecedent" || el=="isqui")
                {
                    int nbsup = _phrase->nbSuper(r->sub());
                    if (nbsup == 2)
                    {
                        MotFlechi* mfv = _phrase->vbRelative(r->sub());
                        if (mfv != 0)
                        {
                            if (ante) lante.append(mfv->trGroupe());
                            else lpost.append(mfv->trGroupe());
                        }
                    }
                    else
                    {
                        if (ante)
                        {
                            lante.append(r->trSub());
                        }
                        else lpost.append(r->trSub());
                    }
                }
                else
                {
                    // le relatif doit être en tête de groupe.
                    // On le met en réserve
                    if (r->sub()->lemme()->cle() == "qui2")
                    {
                        rel = r->sub()->tr();
                    }
                    else if (ante && r->subSup())
                    {
                        lante.append(r->trSub());
                    }
                    else if (!ante && !r->subSup())
                    {
                        lpost.append(r->trSub());
                    }
                }
            }
        }
    }
    // restitution du relatif
    lante.prepend(rel);
    QStringList liste = lante + lpost;
    return elideFr(liste.join(" ").simplified());
}

QString MotFlechi::trNue()
{
    return _trNue;
}

QString MotFlechi::trs()
{
    QStringList ret;
    for (int i=0;i<_trfl.count();++i)
    {
        QString t = _trfl.at(i);
        if (i == _itr)
        {
            t.prepend("<span style=\"color:darkred;\">");
            t.append("</span>");
        }
        ret.append(t);
    }
    return ret.join("<br/>");
}

bool MotFlechi::valide()
{
    return _valide;
}
