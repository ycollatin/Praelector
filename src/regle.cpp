/*          regle.cpp    */
/*
 * This file is part of PRAELECTOR.
 * PRAELECTOR is free software; you can redistribute it and/or modify *  it
 * under the terms of the GNU General Public License as published by *  the Free
 * Software Foundation; either version 2 of the License, or *  (at your option)
 * any later version.
 *
 *  COLLATINVS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *
 *  This file is part of PRAELECTOR.
 *
 *  PRAELECTOR is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  PRAELECTOR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PRAELECTOR; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  © Yves Ouvrard, 2009 - 2017
 */

// TODO
// - Ajouter dans Regle le moyen de fixer la morpho française de
// <sub> et <sup>, au moyen de deux nouvelles clés
// frsup: et frsub:. Il faudra overloader MotFlechi::tr() et
// MotFlechi::trGroupe().

#include <iostream>
#include <lemme.h>
#include <regle.h>

QStringList const clesR = QStringList()
    <<"accord"      // 0
    <<"aff"
    <<"conflit"
    <<"contig"
    <<"doc"
    <<"id"          // 5
    <<"filtre"
    <<"lemmeSub"
    <<"lemmeSup"
    <<"lsSub"
    <<"lsSup"      // 10
    <<"morphoSub"
    <<"morphoSup"
    <<"poids"
    <<"posSub"
    <<"posSup"     // 15
    <<"sens"
    <<"subEstSup"
    <<"supEstSub"
    <<"supEstSup"
    <<"tr"         // 20
    <<"x";

//////////////////////////////
//   Classe Regle           //
//////////////////////////////

Regle::Regle(QStringList ll)
{
    _contig = 0;
    for (int i=0;i<ll.count();++i)
    {
        QString lin = ll.at(i).simplified();
        QStringList ecl=lin.split(':', QString::KeepEmptyParts);
        if (ecl.count() < 2)
        {
            std::cerr << "regles.la, ligne mal formée :"<<qPrintable(lin);
            continue;
        }
        int p = clesR.indexOf(ecl.at(0));
        QString v = ecl.at(1);
        switch(p)
        {
            case  0: _accord = v; break;
            case  1: _aff = v; break;
            case  2: _conflits = v.split(','); break;
            case  3: _contig = v.toInt(); break;
            case  4: _doc = v; break;
            case  5: _id = v; break;
            case  6: _filtre = v.split(','); break;
            case  7:
                     {
                         QStringList eclats = v.split(',');
                         for (int i=0;i<eclats.count();++i)
                         {
                             QString ecl = eclats.at(i);
                             if (ecl.startsWith('-'))
                             {
                                 ecl.remove(0,1);
                                 _lSubExclus.append(ecl);
                             }
                             else _lemmeSub.append(ecl);
                         }
                         break;
                     }
            case  8:
                     {
                         QStringList eclats = v.split(',');
                         for (int i=0;i<eclats.count();++i)
                         {
                             QString ecl = eclats.at(i);
                             if (ecl.startsWith('-'))
                             {
                                 ecl.remove(0,1);
                                 _lSupExclus.append(ecl);
                             }
                             else _lemmeSup.append(ecl);
                         }
                         break;
                     }
            case  9: _lsSub = v.split(','); break;
            case 10: _lsSup = v.split(','); break;
            case 11: _morphoSub = v.split(','); break;
            case 12: _morphoSup = v.split(','); break;
            case 13: _poids = v.toInt(); break;
            case 14: _posSub = v; break;
            case 15: _posSup = v; break;
            case 16: _sens = v[0]; break;
            case 17: _subEstSup = v; break;
            case 18: _supEstSub = v; break;
            case 19: _supEstSup = v.split(','); break;
            case 20: _tr.append(v); break;
            case 21: _exclus = v.split(','); break;

            default:
                     {
                         std::cerr << qPrintable("regles.la:"+ecl.at(0)+"erreur de clé");
                         break;
                     }
        }
    }
    // _tr manquant ?
    if (_tr.isEmpty()) _tr.append("<sup> <sub>");
    // analyse des filtres
    _antepos = 0;
    for (int i=0;i<_filtre.count();++i)
    {
       QString f = _filtre.at(i);
       if (f.startsWith("antep"))
           _antepos = f.mid(5).toInt();
    }
}

QString Regle::accord()
{
    return _accord;
}

QString Regle::aff()
{
    if (_aff.isEmpty()) return _id;
    return _aff;
}

int Regle::antepos()
{
    return _antepos;
}

bool Regle::conflit(QString id)
{
    return _conflits.contains(id);
}

/**
  Regle::compatibleSu?Su?
  Quatre fonctions qui disent si deux requêtes qui ont un
  noeud en commun ont au moin un pos compatible pour ce
  noeud.
 */
bool Regle::compatibleSubSub(Regle* r)
{
    if (_posSub.isEmpty() || r->posSub().isEmpty()) return true;
    QSet<QString> st = QSet<QString>::fromList(_posSub.split(','));
    QSet<QString> sr = QSet<QString>::fromList(r->posSub().split(','));
    return !(st.intersect(sr).empty());
}

bool Regle::compatibleSubSup(Regle* r)
{
    if (_posSub.isEmpty() || r->posSup().isEmpty()) return true;
    QSet<QString> st = QSet<QString>::fromList(_posSub.split(','));
    QSet<QString> sr = QSet<QString>::fromList(r->posSup().split(','));
    return !(st.intersect(sr).empty());
}

bool Regle::compatibleSupSub(Regle* r)
{
    if (_posSup.isEmpty() || r->posSub().isEmpty()) return true;
    QSet<QString> st = QSet<QString>::fromList(_posSup.split(','));
    QSet<QString> sr = QSet<QString>::fromList(r->posSub().split(','));
    return !(st.intersect(sr).empty());
}

bool Regle::compatibleSupSup(Regle* r)
{
    if (_posSup.isEmpty() || r->posSup().isEmpty()) return true;
    QSet<QString> st = QSet<QString>::fromList(_posSup.split(','));
    QSet<QString> sr = QSet<QString>::fromList(r->posSup().split(','));
    return !(st.intersect(sr).empty());
}

int Regle::contig()
{
    return _contig;
}

QString Regle::doc()
{
    return _doc;
}

bool Regle::estSub(MotFlechi* mf)
{
    // lemme
    Lemme* l = mf->lemme();
    QString cle = l->cle();
    if (subExclu(cle)) return false;
    if (!_lemmeSub.empty() && !_lemmeSub.contains(cle)) return false;
    // pos
    if (!_posSub.isEmpty() && !intersect(_posSub, l->pos()))
        return false;
    // morpho
    QString m = mf->morpho();
    bool okm = _morphoSub.empty();
    if (!okm) for (int i=0;i<_morphoSub.count();++i)
        okm = okm || inclus(_morphoSub.at(i), m);
    if (!okm) return false;
    // lexicosyntaxe
    if (!_lsSub.empty())
    {
        for (int i=0;i<_lsSub.count();++i)
            if (l->synt(_lsSub.at(i))) return true;
        return false;
    }
    return true;
}

bool Regle::estSuper(MotFlechi* mf)
{
    // lemme
    Lemme* l = mf->lemme();
    //QStringList ll = l->gr().split(',');
    QString cle = l->cle();
    if (!_lemmeSup.empty() && !_lemmeSup.contains(cle)) return false;
    // pos
    if (!_posSup.isEmpty() && !intersect(_posSup, l->pos()))
    {
        return false;
    }
    // morpho
    QString m = mf->morpho();
    bool okm = _morphoSup.empty();
    if (!okm) for (int i=0;i<_morphoSup.count();++i)
        okm = okm || inclus(_morphoSup.at(i), m);
    if (!okm) return false;
    // lexicosyntaxe
    if (!_lsSup.empty())
    {
        for (int i=0;i<_lsSup.count();++i)
        {
            if (l->synt(_lsSup.at(i))) return true;
        }
        return false;
    }
    return true;
}

bool Regle::exclut(QString id)
{
    return _exclus.contains(id);
}

QString Regle::id()
{
    return _id;
}

QStringList Regle::filtre()
{
    return _filtre;
}

// tous les traits demandés par a doivent être dans b
bool Regle::inclus(QString a, QString b)
{
    QStringList la = a.split(',');
    for (int i=0;i<la.count();++i)
        if (!b.contains(la.at(i))) return false;
    return true;
}

bool Regle::intersect(QString a, QString b)
{
    for (int i=0;i<a.length();++i)
        if (b.contains(a.at(i))) return true;
    return false;
}

QStringList Regle::lemmeSub()
{
    return _lemmeSub;
}

QString Regle::subEstSup()
{
    return _subEstSup;
}

QString Regle::supEstSub()
{
    return _supEstSub;
}

QStringList Regle::supEstSup()
{
    return _supEstSup;
}

QString Regle::lsSub(int i)
{
    return _lsSup.at(i);
}

QString Regle::lsSup(int i)
{
    return _lsSup.at(i);
}

QStringList Regle::morphoSub()
{
    return _morphoSub;
}

QStringList Regle::morphoSup()
{
    return _morphoSup;
}

int Regle::nbLsSub()
{
    return _lsSub.count();
}

int Regle::nbLsSup()
{
    return _lsSup.count();
}

int Regle::nbTr()
{
    return _tr.count();
}

int Regle::poids()
{
    return _poids;
}

QString Regle::posSub()
{
    return _posSub;
}

QString Regle::posSup()
{
    return _posSup;
}

QChar Regle::sens()
{
    return _sens;
}

bool Regle::subExclu(QString s)
{
    return _lSubExclus.contains(s);
}

bool Regle::supExclu(QString s)
{
    return _lSupExclus.contains(s);
}

QString Regle::tr(int i)
{
    return _tr.at(i); 
}
