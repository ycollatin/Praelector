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
}

void Mot::ajFlechi(MotFlechi* mf)
{
    if (!_flechis.contains(mf))
        _flechis.append(mf);
}

QList<Requete*> Mot::closes()
{
    QList<Requete*> ret;
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        ret.append(mf->closes());
    }
    return ret;
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
        if (req->close() && !req->morte() && ((req->super()->mot() == this) || (req->sub()->mot() == this)))
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

QString Mot::gr()
{
    return _gr;
}

QString Mot::htmlMorphos()
{
    QString ret;
    QTextStream fl(&ret);
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        fl << mf->morpho() << " : " << mf->trfl() << "<br/>\n";
    }
    return ret;
}

void Mot::lance()
{
    for (int i=0;i<_flechis.count();++i)
    {
        MotFlechi* mf = _flechis.at(i);
        mf->lance();
    }
}

// liste de requêtes closes dont le mot est super ou sub
QList<MotFlechi*> Mot::mfLies()
{
    QList<MotFlechi*> liste;
    QList<Requete*> lreq = closes();
    for (int i=0;i<lreq.count();++i)
    {
        Requete* req = lreq.at(i);
        if (!req->close()) continue;
        if (req->super()->mot() == this && !liste.contains(req->super()))
            liste.append(req->super());
        else if (!liste.contains(req->sub()))
            liste.append(req->sub());
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

void Mot::setMorphos(MapLem m)
{
    _morphos = m;
    // calcul de tous les fléchis de mc
    for (int il=0;il<m.keys().count();++il)
    {
        Lemme *l = m.keys().at(il);
        for (int im=0;im<m[l].count();++im)
        {
            QString morpho = m[l].at(im).morpho;
            MotFlechi* mf = new MotFlechi(l, morpho, this);
            ajFlechi(mf);
        }
    }
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
