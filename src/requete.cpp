/*                       requete.cpp    */
/*
*  This file is part of PRAELECTOR.
*
*  PRAELECTOR is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  COLLATINVS is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PRAELECTOR; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* © Yves Ouvrard, 2009 - 2017
*/

#include <requete.h>

//////////////////////////////
//   Classe Requete         //
//////////////////////////////

Requete::Requete(MotFlechi* sup, MotFlechi* sub, Regle* r)
{
    // signetRequete
    _regle = r;
    _super = 0;
    _sub = 0;
    _activee = false;
    if (sup != 0)
    {
        _super = sup;
        _requerant = _super->mot();
        _lemme = sup->lemme();
        _morpho = sup->morpho();
        _phrase = sup->mot()->phrase();
        _subRequis = true;
    }
    if (sub != 0)
    {
        _sub = sub;
        _requerant = _sub->mot();
        _lemme = sub->lemme();
        _morpho = sub->morpho();
        _phrase = sub->mot()->phrase();
        _subRequis = false;
    }
    _coord1 = 0;
    _morte = false;
}

bool Requete::activee()
{
    return _activee;
}

QString Requete::aff()
{
    return _regle->aff();
}

/*
void Requete::ajEx(MotFlechi* mf)
{
    _exx.append(mf);
}
*/

void Requete::ajHist(QString h)
{
    _hist.append(h);
}

void Requete::annuleRequis(QString cause)
{
    if (!close())
    {
        ajHist("Erreur - Requête déjà annulée");
        return;
    }
    MotFlechi *mf = requisFl();
    //if (!_exx.empty())
    bool inex = true;
    // essai de récupération des mots non liés
    for (int i=_exx.count()-1;i>=0;--i)
    {
        MotFlechi* mex = _exx.takeLast();
        if (mex->nbCloses() == 0)
        {
            setRequis(mex, cause);
            inex = false;
            break;
        }
    }
    if (inex)
    {
        setRequis(0, cause);
    }
    _exx.append(mf);
    ajHist("ANNULÉE "+cause);
}

Requete* Requete::clone()
{
    Requete* nreq;
    if (!close())
    {
        if (_subRequis) nreq = new Requete(_super, 0, _regle);
        else nreq = new Requete(0, _sub, _regle);
    }
    else 
    {
        nreq = new Requete(_super, _sub, _regle);
    }
    if (_subRequis) nreq->setSubRequis();
    else nreq->setSuperRequis();
    return nreq;
}

bool Requete::close()
{
    return (_super != 0 && _sub != 0);
}

bool Requete::contigue()
{
    if (!close()) return false;
    return abs(_super->rang() - _sub->rang()) == 1;
}

Requete* Requete::coord1()
{
    return _coord1;
}

bool Requete::croise(Requete* req)
{
    if (!close() || !req->close()) return false;
    int prim = prima()->rang();
    int ult = ultima()->rang();
    int reqprim = req->prima()->rang();
    int reqult = req->ultima()->rang();
    //if (req->aff()=="coord1") ++reqult;
    if (prim < reqprim && ult > reqprim && ult < reqult)
        return true;
    if (prim > reqprim && prim < reqult && ult > reqult)
        return true;
    return false;
}

bool Requete::homonyme(Requete* req)
{
    return id() == req->id() 
        && _super->mot() == req->super()->mot()
        && _sub->mot() == req->sub()->mot();
}

/**
 * rang du requis - rang du requérant
 * la virgule accroît le résultat de 5
 * à modifier : distance en groupes, non en mots.
 */
int Requete::distance()
{
    if (!close()) return 100;
    int ret = requis()->rang() - requerant()->rang();
    // une seule virgule est un handicap
    int nbp = nbPonct(',');
    if (id() == "app" && nbp == 1) ret = 0;
    else ret += 20;
    return ret;
}

int Requete::distance(Mot* m)
{
    if (m != 0) return m->rang() - _requerant->rang();
    return _phrase->nbMots();
}

QString Requete::doc()
{
    QString ret;
    QTextStream fl(&ret);
    fl<<_num<<". ";
    if (_subRequis) fl<<"subRequis ";
    else fl<<"superRequis ";
    if (_super == 0) fl << "?";
    else fl << _super->mot()->gr()<<" "<<_super->lemme()->gr()<<" "<<_super->morpho();
    fl << "-"<<id().toUpper()<<"->";
    if (_sub == 0) fl << "?";
    else fl << _sub->mot()->gr()<<" "<<_sub->lemme()->gr()<<" "<<_sub->morpho();
    return ret;
}

bool Requete::enConflit(QString id)
{
    return _regle->conflit(id);
}

bool Requete::estEx(MotFlechi* mf)
{
    return _exx.contains(mf);
}

MotFlechi* Requete::ex(int i)
{
    return _exx.at(i);
}

QString Requete::fonction()
{
    QString ret;
    QTextStream fl(&ret);
    fl<< _sub->gr()<<" "<<_regle->aff()<<" "<<_super->gr();
    return ret;
}

int Requete::freq()
{
    return _super->freq() + _sub->freq();
}

QString Requete::gv(QString format)
{
    QString lin;
    QTextStream flin(&lin);
    if (format == "dot")
    {
        if (!close())
        {
            //if (_subRequis) flin << "/* "<<_requerant->gr()<<"-"<<id()<<"->? */";
            if (_subRequis) flin << "/* "<<_requerant->gr()<<"-"<<aff()<<"->? */";
            else            flin << "/* ? -"<<aff()<<"->"<<_requerant->gr()<<" */";
        }
        else flin << "N"<<_super->rang()<<"->N"<<_sub->rang()
            << " [label=\""<<aff()<<"\"];";
    }
    else if (format=="easy")
    {
        if (!close())
        {
            if (_subRequis) flin << "# "<<_requerant->gr()<<"-"<<aff()<<"->? ";
            else            flin << "# ? -"<<aff()<<"->"<<_requerant->gr();
        }
        else flin << "["<<_super->rang()<<" "<<_super->gr()<<"] -- "<<aff()<<" --> ["<<_sub->rang()<<" "<<_sub->gr()<<"]";
    }
    return lin;
}

int Requete::handicap()
{
    if (!close()) return 500;
    // possum n'admet pas les noms et pronoms objet
    if (_super->lemme()->gr() == "possum" && id() == "objet"
        && (_sub->lemme()->pos() == "n" || _sub->lemme()->pos() == "p"))
        return 50;
    // somme des handicaps des noeuds
    int ret = _super->handicap() + _sub->handicap();
    // une virgule est un gros handicap
    if (id() != "app" && id() != "conjSub" && nbPonct(',') > 0) ret += 40;
    // un génitif est rarement super d'un autre génitif
    if (id() == "genitif" && _super->morpho().contains("génitif"))
        ret += 7;
    if (_regle->id() == "epithete" && _sub->mot()->nomAdj() && !contigue())
        ret += 10;
    return ret;
}

QString Requete::hist()
{
    return _hist.join("\n");
}

bool Requete::homoSub(MotFlechi* mf)
{
    return _sub->mot() == mf->mot();
}

QString Requete::html()
{
    QString ret;
    QTextStream fl(&ret);
    // En noir, fonction
    fl << _sub->gr() << " "
        << _regle->aff() << " "
        << _super->gr() << " "
        // En bleu italique, traduction
        << "<span style=\"color:blue;font-style:italic\">"<<tr()<<"</span> "
        // doc de la règle
        << "<a href=\"l.d."<<_num<<"\">doc</a> "
        // lien valider
        << "<a href=\"l.v."<<_num<<"\">valider</a> "
        // lien rejeter
        << "<a href=\"l.r.\""<<_num<<">rejeter</a>";
    return ret;
}

QString Requete::humain()
{
    QString ret;
    QTextStream fl(&ret);
    fl << _num <<". ";
    if (_super == 0)
        fl << "?";
    else fl << _super->gr();
    fl << " "<<id()<<" ";
    if (_sub == 0)
        fl << "?";
    else fl << _sub->gr();
    return ret;
}

bool Requete::homoSuper(MotFlechi* mf)
{
    return _super->mot() == mf->mot();
}

QString Requete::id()
{
    return _regle->id();
}

void Requete::initExx()
{
    _exx.clear();
}

int Requete::largeur()
{
    if (!close()) return 1000;
    return abs(_super->rang() - _sub->rang());
}

void Requete::meurt()
{
    _morte = true;
}

bool Requete::morte()
{
    return _morte;
}

bool Requete::multi()
{
    return _regle->filtre().contains("multi");
}

int Requete::nbEx()
{
    return _exx.count();
}

int Requete::nbPonct(QChar c)
{
    if (!close()) return 0;
    return _requerant->phrase()->nbPonct(c, _requerant, requis());
}

int Requete::num()
{
    return _num;
}

QString Requete::numc()
{
    return QString::number(_num);
}

int Requete::poids()
{
    int ret = _regle->poids();
    if (!close()) return ret;
    ret -= handicap();
    if (contigue()) ret += _regle->contig();
    if (_sub->rang() < _super->rang()) ret += _regle->antepos();
    // si le requis de la requête est le dernier mot, et que l'annulation
    // laisserait le requérant isolé
    if (requisFl()->rang() == _phrase->nbMots()-1 && requerantFl()->nbCloses() == 1)
        ret += 20;
    return ret;
}

QList<Mot*> Requete::portee()
{
    QList<Mot*> liste;
    for (int i=prima()->rang()+1;i<ultima()->rang();++i)
        liste.append( _phrase->motNo(i));
    return liste;
}

MotFlechi* Requete::prima()
{
    if (!close()) return requerantFl();
    if (_super->rang() < _sub->rang()) return _super;
    return _sub;
}

bool Requete::reciproque(Requete* req)
{
    return _super == req->sub() && _sub == req->super();
}

Regle* Requete::regle()
{
    return _regle;
}

Mot* Requete::requerant()
{
    return _requerant;
}

MotFlechi* Requete::requerantFl()
{
    if (_subRequis) return _super;
    return _sub;
}

Mot* Requete::requis()
{
    if (!close()) return 0;
    if (_requerant == _super->mot()) return _sub->mot();
    return _super->mot();
}

MotFlechi* Requete::requisFl()
{
    if (!close()) return 0;
    if (_subRequis) return _sub;
    return _super;
}

QChar Requete::sens()
{
    return _regle->sens();
}

bool Requete::separeparVConj()
{
    if (!close()) return false;
    //QList<Mot*> mots = _phrase->portee(_requerant->rang(), requis()->rang());
    QList<Mot*> mots = portee();
    if (mots.empty()) return false;
    bool v = true;
    for (int i=0;i<mots.count();++i)
    {
        Mot* m = mots.at(i);
        for (int im=0;im<m->nbFlechis();++im)
        {
            MotFlechi* mf = m->flechi(im);
            v = v || (mf->lemme()->pos().contains('w')
                && mf->morpho().contains("è"));
        }
    }
    return v;
}

void Requete::setCoord1(Requete* req)
{
    _coord1 = req;
}

void Requete::setNum(int n)
{
    _num = n;
}

void Requete::setRequis(MotFlechi *m, QString cause)
{
    bool nul = m == 0;
    if (_subRequis)
    {
        if (nul) _sub = 0;
        else setSub(m);
    }
    else 
    {
        if (nul) _super = 0;
        else setSuper(m);
    }
    if (nul) ajHist("ANNULATION du requis, "+cause);
    else 
    {
        //_traduction.replace("<sup>", _super->tr()).replace("<sub>", _sub->tr());
        ajHist("REQUIS adopté ("+cause+") "+m->gr()+", "+m->morpho());
    }
}

void Requete::setSub(MotFlechi *m)
{
    _sub = m;
    _activee = true;
    _sub->ajReq(this);
}

void Requete::setSubRequis()
{
    _subRequis = true;
}

void Requete::setSuper(MotFlechi *m)
{
    _super = m;
    _activee = true;
    _super->ajReq(this);
}

void Requete::setSuperRequis()
{
    _subRequis = false;
}

MotFlechi* Requete::sub()
{
    return _sub;
}

bool Requete::subRequis()
{
    return _subRequis;
}

MotFlechi* Requete::super()
{
    return _super;
}

bool Requete::superRequis()
{
    return !_subRequis;
}

QString Requete::tr()
{
    QString ret = _regle->traduction();
    ret.replace("<sup>", _super->tr());
    ret.replace("<sub>", _sub->tr());
    return ret;
}

MotFlechi* Requete::ultima()
{
    if (!close()) return requerantFl();
    if (_super->rang() > _sub->rang()) return _super;
    return _sub;
}

