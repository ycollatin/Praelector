/* mot.h */
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

#ifndef MOT_H
#define MOT_H

#include <QObject>
#include <QList>
#include <QString>

#include <lemmatiseur.h>
//#include <libflexfr.h>
#include <motflechi.h>
#include <phrase.h>

class MotFlechi;
class Phrase;
class Requete;

class Mot : public QObject
{

    Q_OBJECT

    private:
        QString             _gr;
        MapLem              _morphos;
        int                 _dm;
        QList<MotFlechi*>   _flechis; 
        int                 _fm;
        Phrase*             _phrase;
        int                 _rang;
        int                 _que;
        QString             _tr;

    public:
        Mot(QString g, int d=-1, int f=-1, int r=-1, QObject *parent=0);
        void              ajFlechi(MotFlechi* mf);
        void              annuleFlechi(int f);
        void              annuleLemme(int l);
        void              choixFlechi(MotFlechi* mf);
        void              choixSuper(Requete* req);
        void              choixSub(Requete* req);
        QList<Requete*>   closes();
        int               debut();
        bool              estAu(QString t);
        Lemme*            estLemDe(QStringList lr);
        bool              estLie();
        Lemme*            estPosDe(QString pr);
        bool              estPrep();
        bool              estRelatif();
        bool              estSubParAff(QString aff);
        bool              estSubParId(QString id);
        //bool              estSuperDe(Mot* m);
        bool              estSuperParAff(QString aff);
        bool              estSuperParId(QString id);
        int               fin();
        MotFlechi*        flechi(int i);
        QList<MotFlechi*> flValide();
        QString           gr();
        QString           html();
        void              lance();
        QList<MotFlechi*> mfLies();
        bool              monomorphe();
        MapLem            morphos();
        int               nbFlechis();
        int               nbSupers();
        bool              nomAdj();
        Phrase*           phrase();
        bool              polyLemme();
        QString           pos();
        MotFlechi*        princeps();
        bool              que();
        int               rang();
        Requete*          reqSub(QString id);
        QList<Requete*>   reqSuper();
        void              setMorphos(MapLem m);
        void              setPSup(int p);
        void              setTr(QString t);
        MotFlechi*        super();
        QString           trGroupe();
        QString           trs();
};

#endif
