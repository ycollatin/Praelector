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
#include <libflexfr.h>
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
}

void MotFlechi::ajReqSub(Requete* req)
{
    _lreqSub.append(req);
}

void MotFlechi::ajReqSup(Requete* req)
{
    _lreqSup.append(req);
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

QString MotFlechi::conj()
{
    QString gr = _lemme->gr();
	QString ret = conjnat(_traduction, _morpho);
	QString ajout = "";
	int x = gr.indexOf (' ');
	if ((x > -1) && !gr.startsWith ("se "))
	{
		ajout = gr.section (' ', 1);
		ajout.prepend (" ");
		gr.chop (gr.length()-x);
	}
	// adjectif verbal
	if (_morpho.contains("verbal"))
		ret.prepend ("devant être ");
	return ret;
}

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
    bool debog = false;
    //debog = gr()=="quem" && _lemme->gr()=="qui";
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        if (debog && req->close()) qDebug()<<gr()<<"estSubPar"<<req->id();
        if (req->close() && req->aff() == aff)
            return true;
    }
    return false;
}

bool MotFlechi::estSubParId(QString id)
{
    bool debog = false;
    //debog = gr()=="et" && id=="conjcoordN";
    if (debog) qDebug()<<gr()<<"estSubParId"<<id;
    for (int i=0;i<_lreqSub.count();++i)
    {
        Requete* req = _lreqSub.at(i);
        //if (debog) qDebug()<<"    req"<<req->debog();
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
    bool debog = false;
    //debog = gr()=="uitia" && _lemme->cle()=="uitium";
    if (debog) qDebug()<<"        motflechi::handicap"<<_morpho;
    int ret = 0;
    ret += _phrase->handicap(this);
    if (debog) qDebug()<<"        handicap _lemme->gr()"<<_lemme->gr()<<"ret"<<ret;
    // handicaps morpho
    if (_morpho.contains("vocatif")) ret += 50;
    else if (gr().endsWith("is"))
    {
        if (_morpho.contains("accusatif pluriel")) ret += 22;
        else if (_lemme->gr().endsWith("ia")) ret += 20;
    }
    int mfl = _mot->mfLies().count();
    ret -= mfl;
    if (debog) qDebug()<<"        mfl"<<mfl<<"ret"<<ret;
    return ret;
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
    bool debog = false;
    //debog = gr()=="Leo";
    if (debog) qDebug()<<gr()<<"lance. nbRegles"<<_phrase->nbRegles();
    for (int i=0;i<_phrase->nbRegles();++i)
    {
        Regle* r = _phrase->regle(i);
        //debog = gr()=="Vitelliorum" && r->id()=="genitif";
        if (debog) qDebug()<<"  lance"<<_morpho<<"  règle"<<r->id()<<"estSub"<<r->estSub(this);
        if (r->estSuper(this) && r->sens() != '<') lanceReqSup(r);
        if (r->estSub(this) && r->sens() != '>') lanceReqSub(r);
    }
}

// _lreqSub est la liste des requêtes dont le fléchi est sub
// _lreqSup est la liste des requêtes dont le fléchi est super
Requete* MotFlechi::lanceReqSub(Regle* r)
{
    bool debog = false;
    //debog = gr()=="leo";
    Requete* nr = new Requete(0, this, r);
    if (debog) qDebug()<<"lanceReqSub"<<nr->debog();
    _phrase->ajRequete(nr);
    _lreqSub.append(nr);
    nr->ajHist("\n------------------\nRequête numéro "+nr->numc());
    nr->ajHist(nr->humain());
    nr->ajHist(nr->debog());
    return nr;
}

Requete* MotFlechi::lanceReqSup(Regle* r)
{
    //debog = gr()=="et"; // && (mf->gr()=="maiores" || mf->gr()=="natu");
    Requete* nr = new Requete(this, 0, r);
    _phrase->ajRequete(nr);
    _lreqSup.append(nr);
    nr->ajHist("\n------------------\nRequête numéro "+nr->numc());
    nr->ajHist(nr->humain());
    nr->ajHist(nr->debog());
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
        //qDebug()<<i<<_lreqSup.at(i)->debog();
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
                req->annuleRequis("accord rompu par "+req->debog());
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
    bool debog = false;
    if (debog) qDebug()<<"MotFlechi::resout. mot"<<gr()<<this->morpho()<<", req"<<req->debog();
    // signetResout
    // un mot ne peut être lié à lui-même.
    if (req->requerant() == _mot) return false;
    if (debog) qDebug()<<"   oka";

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
    if (debog) qDebug()<<"   okb";

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
    if (debog) qDebug()<<"   oke";

    // liens associés
    if (!req->regle()->subEstSup().isEmpty()
        && ((req->subRequis() && !estSuperParId(req->regle()->subEstSup()))
            || (req->superRequis() && !req->sub()->estSuperParId(req->regle()->subEstSup()))))
            return false;
    if (debog) qDebug()<<"   okf";

    if (debog) qDebug()<<"   resout ok subestsup. supEstSub:"<<req->regle()->supEstSub()<<"listeR"<<_phrase->nbListeR();
    if (!req->regle()->supEstSub().isEmpty() && req->super() != 0)
    {
        if (!req->super()->estSubParId(req->regle()->supEstSub()))
        {
            return false;
        }
    }
    if (debog) qDebug()<<"   resout ok supestsub"<<req->regle()->supEstSup()<<"listeR"<<_phrase->nbListeR();
    if (!req->regle()->supEstSup().empty() && req->super() != 0)
    {
        bool passe = false;
        for (int i=0;i<req->regle()->supEstSup().count();++i)
            passe = passe || (req->super()->estSuperParId(req->regle()->supEstSup().at(i)));
        if (!passe) return false;
    }

    if (debog) qDebug()<<"   resout okb"<<"listeR"<<_phrase->nbListeR();
    // Le super est requis
    QString accord = req->regle()->accord();
    if (req->superRequis() && req->sens() != '>')
    {
        if (debog) qDebug()<<"   resout okc superRequis"<<"listeR"<<_phrase->nbListeR();
        // un super en -que ne peut avoir de sub qui le précède
        if (_mot->que() && rang() > req->sub()->rang())
            return false;
        if (debog) qDebug()<<"   resout okd accord"<<req->gv()<<"accord"<<accord<<_morpho<<"avec"<<req->sub()->morpho()<<_phrase->accord(this, req->sub(), accord);
        if (!accord.isEmpty() && !_phrase->accord(this, req->sub(), accord))
        {
            if (debog) qDebug()<<"   false!"<<"listeR"<<_phrase->nbListeR();
            return false;
        }
        if (debog) qDebug()<<"   resout okd, règle"<<req->regle()->id()<<"estSuper("<<morpho()<<req->regle()->estSuper(this)<<"listeR"<<_phrase->nbListeR();
        if (req->regle()->estSuper(this))
        {
            if (debog) qDebug()<<"   resout oke"<<"listeR"<<_phrase->nbListeR();
            return true;
        }
    }
    // le sub est requis
    else if (req->subRequis() && req->sens() != '<')
    {
        if (debog) qDebug()<<"   resout okf subRequis"<<"listeR"<<_phrase->nbListeR();
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
                if (debog) qDebug()<<"  resout coord2, requêtes reqCC"<<lrs.count()<<"listeR"<<_phrase->nbListeR();
                bool ac = false;
                for (int irs=0;irs<lrs.count();++irs)
                {
                    Requete* rs = lrs.at(irs);
                    if (debog) qDebug()<<"   lrs"<<irs<<".rs:"<<rs->debog();
                    if (debog) qDebug()<<"   intersect"<<rs->super()->lemme()->pos()<<" et "<<_lemme->pos()
                        <<(intersect(rs->super()->lemme()->pos(), _lemme->pos()));
                    if (debog) qDebug()<<"   accord"<< _phrase->accord(this, rs->super(), accord);
                    if (intersect(rs->super()->lemme()->pos(), _lemme->pos())
                        && _phrase->accord(this, rs->super(), accord))
                    {
                        if (debog) qDebug()<<"   coord2 accepté";
                        req->setCoord1(rs);
                        ac = true;
                    }
                }
                if (!ac) return false;
            }
            else if (!accord.isEmpty() && !(_phrase->accord(this, req->super(), accord)))
                return false;
        }
        if (debog) qDebug()<<"    resout okg, règle"<<req->id()<<"estSub "<<morpho()<<":"<<req->regle()->estSub(this);
        // règle
        if (req->regle()->estSub(this))
        {
            if (debog) qDebug()<<"    resout okh, regle->estSub";
            // filtres
            if (req->regle()->filtre().contains("que") && !_mot->que())
                return false;
            if (debog) qDebug()<<"    resout, true pour la requête"<<req->num();
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
	QString retour;
    QChar pos = _lemme->pos().at(0);
	/* passage de la morpho latine à la morpho française */
	switch (pos.unicode())
	{
		case 'v': // verbe
			retour = conj(); break;
		case 'n': // nom
		case 'p': // pronom
			{
				if (_lemme->gr() == "qui")
					switch (_morpho.at (7).unicode())
					{
						case 'b':
						case 'd':
						case 'n': return "qui"; break;
						case 'a': return "que"; break;
						case 'g': return "dont"; break;
					}
			}
		case 'a': // adjectif
			{
				int n;
				switch (_morpho.at (2).unicode ())
				{
					case 'p': n=1; break;
					default : n=0;
				}
				if (pos == 'a')
				{
					int g;
					switch (_morpho.at (6).unicode ())
					{
						case 'f': g=1;break;
						default:  g=0;
					}
					retour = Accorde (gr(), g, n);
					if (_morpho.at (8) == 'c') // comparatif
						retour.prepend ("plus ");
				}
				else if (pos == 'n')
				{
					if (n == 1)
						retour =  Pluriel (gr());
					else retour = gr();
				}
				else retour = gr();
				break;
			}
		default:
			retour = gr();
			break;
	}
	return retour;
}

void MotFlechi::setTraduction(QString t)
{
    _traduction = t;
}
