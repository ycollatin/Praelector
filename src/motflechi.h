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
    private:
        Mot*            _mot;
        QString         _det;
        QString         _eqivPos;
        int             _itr;
        Lemme*          _lemme;
        QString         _morpho;
        bool            _neg;
        char            _pos;
        char            _posO;
        Phrase*         _phrase;
        bool            _rejete;
        QStringList     _traductions;
        QStringList     _trfl;
        QString         _trNue;
        QString         _tr;
        bool            _valide;
    public:
        MotFlechi(Lemme* l, char p, QString m, Mot* parent, char=0x00);
        ~MotFlechi();
        void            ajTrfl(QString t);
        bool            contigu(MotFlechi* mf);
        QString         det();
        QString static  elideFr(QString s);
        QString         eqiv();
        bool            estSub();
        bool            estSubParAff(QString aff);
        bool            estSubParId(QString id);
        bool            estSuper();
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
        int             itr();
        void            lance();
        Lemme*          lemme();
        QString         morpho();
        QString         morphoHtml();
        Mot*            mot();
        int             nbCloses();
        int             nbReqSub();
        int             nbReqSup();
        int             nbReqSupCloses();
        int             nbReqSupValides(Requete* rtest=0);
        int             nbSuper();
        int             nbTr();
        bool            neg();
        void            nettoie();
        char            pos();
        char            posO();
        int             rang();
        bool            rejete();
        Requete*        reqSub(int i);
        Requete*        reqSub(QString id);
        Requete*        reqSup(int i);
        bool            resout(Requete* req);
        void            setDet(bool f=false);
        void            setItr(int i);
        void            setNeg(bool n);
        void            setRejete(bool r);
        void            setSujet();
        void            setTr(QString t);
        void            setValide(bool v);
        bool            sommet();
        QList<Requete*> sub(QString id, Requete* rtest=0);
        QString         trfl();
        QString         trfl(int i);
        QString         tr();
        QString         trs();
        QString         trGroupe(Requete* rtest=0);
        QString         trNue();
        bool            valide();
        void            videReq();
};

#endif
