/*   morphomot.h */
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

#ifndef MOTFLECHI_H
#define MOTFLECHI_H

#include <QObject>
#include <QString>
#include <QStringList>

#include <lemme.h>
#include <phrase.h>
#include <regle.h>
#include <requete.h>

class Mot;
class Phrase;
class Regle;
class Requete;

class MotFlechi: public QObject
{
    Q_OBJECT

    private:
        Mot*            _mot;
        int             _itr;
        Lemme*          _lemme;
        QString         _morpho;
        Phrase*         _phrase;
        QStringList     _traductions;
        QStringList     _trfl;
        QString         _trNue;
        QString         _tr;
        bool            _valide;
    public:
        MotFlechi(Lemme* l, QString m, Mot* parent);
        bool            aUnSuper();
        bool            auxiliaire();
        QList<Requete*> closes();
        bool            contigu(MotFlechi* mf);
        QString         elideFr(QString s);
        bool            estSub();
        bool            estSubParAff(QString aff);
        bool            estSubParId(QString id);
        bool            estSuperDe(MotFlechi* m);
        bool            estSuperParAff(QString aff);
        bool            estSuperParId(QString id);
        bool            estVerbe();
        int             freq();
        QString         gr();
        int             handicap();
        QString         htmlLiens();
        void            incItr(); 
        bool static     intersect(QString a, QString b);
        void            lance();
        Lemme*          lemme();
        QString         morpho();
        Mot*            mot();
        int             nbCloses();
        int             nbReqSub();
        int             nbReqSup();
        int             nbReqSupCloses();
        int             nbReqSupValides(Requete* rtest=0);
        void            nettoie();
        int             rang();
        Requete*        reqSub(int i);
        Requete*        reqSub(QString id);
        Requete*        reqSup(int i);
        bool            resout(Requete* req);
        void            setDet(bool f=false);
        void            setSujet();
        void            setTr(QString t);
        void            setValide(bool v);
        bool            sommet();
        Requete*        sub(QString id, Requete* rtest=0);
        QString         trfl();
        QString         tr();
        //QString         trGroupe(Requete* rtest=0);
        QString         trGroupe(Requete* rtest=0, QString morph=0);
        QString         trNue();
        bool            valide();
        void            videReq();
};

#endif
