/*   mot.cpp   */
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

#include <mot.h>
#include <mainwindow.h>

#include <iostream>
#include <QDebug>
#include <QTextCodec>
#include <QTextStream>

/**
 * \fn Mot::Mot(QString g)
 * \brief Créateur de la classe Mot.
 *  d : position de la première lettre ;
 *  f : position de la dernière lettre ;
 *  r : rang dans la phrase (le premier mot a le rang 0) ;
 *  parent : la phrase.
 */
// signetMot
Mot::Mot(QString g, int d, int f, int r, QObject *parent) : QObject(parent)
{
    if (parent != 0)
        _phrase = qobject_cast<Phrase*>(parent);
    _gr = g;
    _dm = d;
    _fm = f;
    _que = -1;
    _rang = r;
    _morphos.clear();
    _reqLancees = false;
}

Mot::~Mot()
{
    _morphos.clear();
    while (_flechis.count() > 0)
    {
        MotFlechi* mf = _flechis.takeFirst();
        delete mf;
    }
}

void Mot::ajFlechi(MotFlechi* mf)
{
    if (!_flechis.contains(mf))
        _flechis.append(mf);
}

void Mot::annuleFlechi(int f)
{
    MotFlechi* mf = _flechis.at(f);
    for (int ir=0;ir<mf->nbReqSup();++ir)
        mf->reqSup(ir)->annuleRequis(mf->morpho()+", fléchi rejeté");
    for (int ir=0;ir<mf->nbReqSub();++ir)
        mf->reqSub(ir)->annuleRequis(mf->morpho()+", fléchi rejeté");
    mf->setRejete(true);
}

void Mot::annuleFlechi(MotFlechi* mf)
{
    mf->setRejete(true);
}

void Mot::annuleLemme(Lemme* lem)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (mf->lemme() == lem)
        {
            mf->setRejete(true);
        }
    }
}

void Mot::choixFlechi(MotFlechi* mf)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* f = _flechis.at(i);
        if (f != mf)
        {
            f->setRejete(true);
        }
        else f->setValide(true);
    }
}

void Mot::choixLemme(Lemme* l)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (mf->lemme() != l)
        {
            mf->setRejete(true);
        }
    }
}

void Mot::choixSub(Requete* req)
{
    // annuler toutes les autres super, sauf antécédent
    MotFlechi* mf = req->sub();
    if (mf==0)
    {
        return;
    }
    for (int i=0;i<mf->nbReqSup();++i)
    {
        Requete* r = mf->reqSup(i);
        if (r->close() && r != req && r->id() != "antecedent")
            r->annuleRequis("Lien concurrent choisi");
    }
}

void Mot::choixSuper(Requete* req)
{
    // annuler toutes les requêtes homonymes, sauf si req est multi
    // TODO : tenir compte de l'antécédent ?
    if (!req->multi())
    {
        // annuler toutes les requêtes homonymes, sauf si req est multi
        MotFlechi* mf = req->super();
        for (int i=0;i<mf->nbReqSub();++i)
        {
            Requete* r = mf->reqSub(i);
            if (r->close() && r->aff() == req->aff())
                r->annuleRequis("Lien concurrent choisi");
        }
    }
}

int Mot::debut()
{
    return _dm;
}

bool Mot::estAu(QString t)
{
    for (int i=0;i<_morphos.values().count();++i)
    {
        QList<SLem> lsl = _morphos.values().at(i);
        for (int iv=0;iv<lsl.count();++iv)
            if (lsl.at(iv).morpho.contains(t))
                return true;
    }
    return false;

}

Lemme* Mot::estLemDe(QStringList lr)
{
    if (lr.empty()) return 0;
    for (int i=0;i<_morphos.keys().count();++i)
    {
        Lemme*l = _morphos.keys().at(i);
        if (lr.contains(l->gr()))
            return l;
    }
    return 0;
}

bool Mot::estLie()
{
    for (int i=0;i<_phrase->nbRequetes();++i)
    {
        Requete* req = _phrase->requete(i);
        if (req != 0 && req->close() && ((req->super()->mot() == this) || (req->sub()->mot() == this)))
            return true;
    }
    return false;
}

Lemme* Mot::estPosDe(QString pr)
{
    if (pr.isEmpty()) return 0;
    for (int i=0;i<_morphos.keys().count();++i)
    {
        Lemme* l = _morphos.keys().at(i);
        QString pm = l->pos();
        for (int ip=0;ip<pm.size();++ip)
            if (pr.contains(pm.at(ip)))
                return l;
    }
    return 0;
}

bool Mot::estPrep()
{
    for (int i=0;i<_flechis.count();++i)
    {
        Lemme* l = _flechis.at(i)->lemme();
        if (l->synt("prepabl") || l->synt("prepacc"))
            return true;
    }
    return false;
}

bool Mot::estRelatif()
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (mf->lemme()->cle() != "qui2") continue;
        if (mf->nbCloses() > 0) return true;
    }
    return false;
}

bool Mot::estSommet()
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        // si le fléchi a un sub relatif, le mot n'est pas sommet.
        if (mf->estSub() || _phrase->estVbRelative(mf)) return false;
    }
    return true;
}

bool Mot::estSubParAff(QString aff)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* ms = _flechis.at(i);
        if (ms->estSubParAff(aff)) return true;
    }
    return false;
}

bool Mot::estSubParId(QString id)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* ms = _flechis.at(i);
        if (ms->estSubParId(id)) return true;
    }
    return false;
}

bool Mot::estSuperParAff(QString aff)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* ms = _flechis.at(i);
        if (ms->estSuperParAff(aff)) return true;
    }
    return false;
}

bool Mot::estSuperParId(QString id)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* ms = _flechis.at(i);
        if (ms->estSuperParId(id)) return true;
    }
    return false;
}

int Mot::fin()
{
    return _fm;
}

MotFlechi* Mot::flechi(int i)
{
    return _flechis.at(i);
}

QList<MotFlechi*> Mot::flValides(Requete* rtest)
{
    QList<MotFlechi*> lmf;
    if (rtest != 0 && rtest->super()->mot() == this)
        lmf.append(rtest->super());
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (mf->nbReqSupValides(rtest) > 0)
        {
            lmf.append(mf);
        }
    }
    return lmf;
}

QString Mot::gr()
{
    return _gr;
}

QString Mot::html()
{
    if (_flechis.empty())
		return "mot non reconnu <a href=\"m.a."
			+ QString::number(_rang)
			+ "\">ajouter une entrée</a>";
    QStringList ret;
    for (int i=0;i<_flechis.count();++i)
    {
        QString lin;
        QTextStream fl(&lin);
        MotFlechi* mf = _flechis.at(i);
        if (mf->rejete()) continue;
		fl << "<span style=\"color:green;font-style:italic\"> "
           << mf->morphoHtml() << "</span>";
        // déterminants pour les noms, sujets pour les verbes
        switch(mf->pos())
        {
            case 'n': fl << " <a href=\"m.d."<<i<<"\">det.</a> "; break;
            case 'w':
            case 'v': if (mf->morpho().contains('3'))
						  fl << " <a href=\"m.s."<<i<<"\">suj.</a> ";
                      break;
            default:break;
        }
        fl	<< "tr. <span style=\"color:darkred;font-style:italic\">"<<mf->det()<<mf->tr()<<"</span> ";
        if (mf->nbTr() > 1)
        {
            fl << "<a href=\"m.t."<<i<<"\">traductions</a> ";
        }


        fl  << "<a href=\"m.e."<<i<<"\">&eacute;diter</a>&nbsp;"
            << "<a href=\"m.c."<<i<<"\">choisir</a>&nbsp;rejeter&nbsp;"
			<< "<a href=\"m.r.m."<<i<<"\">le lemme</a>&nbsp;"
			<< "<a href=\"m.r.f."<<i<<"\">la forme</a>&nbsp;";
            //<< "<a href=\"m.+."<<i<<"\">+sub</a>";
        ret.append(lin);
    }
    if (lemmeUnique())
    { 
        int numfl = 0;
        for (int i=0;i<_flechis.count();++i)
        {
            if (_flechis.at(i)->valide())
                numfl = i;
        }
        MotFlechi* mf = _flechis.at(numfl);
        if (mf->nbTr() > 1)
        {
            for (int j=0;j<mf->nbTr();++j)
                if (j != mf->itr())
                {
                    QString lin;
                    QTextStream fl(&lin);
                    fl << "<a href=\"m.i."<<numfl<<"."<<j<<"\">"+mf->trfl(j)+"</a>";
                    ret << lin;
                }
        }
        QString lin = "<a href=\"m.e."+QString::number(numfl)+"\">&eacute;diter</a>";
        ret << lin;
    }
    return ret.join("<br/>\n");
}

void Mot::lance()
{
    if (_reqLancees) return;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (!mf->rejete()) mf->lance();
    }
    _reqLancees = true;
}

bool Mot::lemmeUnique()
{
    // permet, en cas de retour true,
    // d'afficher les choix de traduction
    // du seul lemme [valide]
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (mf->valide()) return true;
    }
    return _morphos.keys().count() == 1;
}

QList<Requete*> Mot::lReqSupCloses()
{
    QList<Requete*> liste;
    for (int i=0;i<_phrase->nbRequetes();++i)
    {
        Requete* req = _phrase->requete(i);
        if (req->close() && req->super()->mot() == this)
            liste.append(req);
    }
    return liste;
}

bool Mot::monomorphe()
{
    return _morphos.values().count() == 1;
}

MapLem Mot::morphos()
{
    return _morphos;
}

int Mot::nbFlechis()
{
    return _flechis.count();
}

int Mot::nbSupers()
{
    int ret = 0;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        for (int ir=0;ir<mf->nbReqSup();++ir)
            if (mf->reqSup(ir)->close()) ++ret;
    }
    return ret;
}

bool Mot::nomAdj()
{
    bool n = false;
    bool a = false;
    for (int i=0;i<_morphos.keys().count();++i)
    {
        Lemme* l = _morphos.keys().at(i);
        a = a || l->pos().contains('a');
        n = n || l->pos().contains('n');
        if (a && n) return true;
    }
    return false;
}

Phrase* Mot::phrase()
{
    return _phrase;
}

QString Mot::pos()
{
    QString ret;
    for (int i=0;i<_morphos.keys().count();++i)
        ret.append(_morphos.keys().at(i)->pos());
    return ret;
}

MotFlechi* Mot::princeps()
{
    int max = 0;
    MotFlechi* maxm = 0;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        int n = mf->nbCloses();
        if (n > max)
        {
            max = n;
            maxm = mf;
        }
    }
    if (max > 3) return maxm;
    return 0;
}

bool Mot::que()
{
    if (_que > 0) return true;
    if (_gr.endsWith("que"))
    {
        _que = 1;
        foreach (Lemme *l, _morphos.keys())
            if (l->gr().endsWith("que"))
                _que = 0;
    }
    else _que = 0;
    return _que > 0;
}

int Mot::rang()
{
    return _rang;
}

/**
 *  void Mot::reinit()
 *  repêche tous les fléchis rejetées
 */
void Mot::reinit()
{
    for (int i=0;i<_flechis.count();++i)
    {
        _flechis.at(i)->setRejete(false);
    }
}

bool Mot::reqLancees()
{
    return _reqLancees;
}

Requete* Mot::reqSub(QString id)
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        for (int j=0;j<mf->nbReqSub();++j)
        {
             Requete* req = mf->reqSub(j);
             if (req->close() && req->id() == id) return req;
        }
    }
    return 0;
}

QList<Requete*> Mot::reqSuper()
{
    QList<Requete*> ret;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        for (int j=0;j<mf->nbReqSup();++j)
        {
            Requete* req = mf->reqSup(j);
            if (req->close()) ret.append(req);
        }
    }
    return ret;
}

void Mot::rmFlechi(MotFlechi* mf)
{
    if (mf == 0 || !_flechis.contains(mf)) return;
    mf->setRejete(true);
}

void Mot::setFlechis(MapLem m)
{
    _morphos = m;
    // calcul de tous les fléchis de mc
    for (int il=0;il<m.keys().count();++il)
    {
        Lemme *l = m.keys().at(il);
        QString pl = l->pos();
        for (int im=0;im<m[l].count();++im)
        {
            // pour les lemmes à plusieurs pos
            for (int ip=0;ip<pl.count();++ip)
            {
                QString morpho = m[l].at(im).morpho;
                char p = pl.at(ip).toLatin1();
                MotFlechi* mf = new MotFlechi(l, p, morpho, this);
                ajFlechi(mf);
                // équivalents des fléchis
                QString eqiv = mf->eqiv();
                if (!eqiv.isEmpty()) for (int ie=0;ie<eqiv.length();++ie)
                {
                    MotFlechi* nmf = new MotFlechi(l, eqiv.at(ie).toLatin1(), morpho, this, mf->posO());
                    ajFlechi(nmf);
                }
            }
        }
    }
    // trier les fléchis
    qSort(_flechis.begin(), _flechis.end(), sortF);
}

//MotFlechi::MotFlechi(Lemme* l, char p, QString m, Mot* parent)

void Mot::setReqLancees(bool r)
{
    _reqLancees = r;
}

void Mot::setTr(QString t)
{
    _tr = t;
}

bool Mot::sortF(MotFlechi* mfa, MotFlechi* mfb)
{
    if (mfa->lemme() != mfb->lemme())
    {
        return mfa->lemme()->freq() > mfb->lemme()->freq();
    }
    QStringList k = QStringList()
        << "nominatif"<<"accusatif"<<"ablatif"
        <<"datif"<<"génitif"<<"vocatif";
    QString ma = mfa->morpho();
    QString mb = mfb->morpho();
    for (int i=0;i<k.count();++i)
    {
        if (ma.contains(k.at(i)))
            return true;
        else if (mb.contains(k.at(i)))
            return false;
    }
    return true;
}

MotFlechi* Mot::super()
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        for (int ir=0;ir<mf->nbReqSub();++ir)
        {
            Requete* r = mf->reqSub(ir);
            if (r->close()) // && r->id() != "antecedent")
                return r->super();
        }
    }
    return 0;
}

QString Mot::trGroupe(Requete* rtest)
{
    QList<MotFlechi*> lmf = flValides(rtest);
    if (!lmf.isEmpty())
    {
        QStringList lret;
        if (lmf.count() > 1) lret.append( _gr+" : Erreur, plusieurs fléchis valides");
        for (int i=0;i<lmf.count();++i)
        {
            lret.append(lmf.at(i)->trGroupe());
        }
        return lret.join("\n");
    }
    return trs();
}

QString Mot::trs()
{
    QStringList ret;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        if (!mf->rejete()) ret.append(mf->trGroupe());
    }
    ret.removeDuplicates();
    return ret.join(" / ");
}
