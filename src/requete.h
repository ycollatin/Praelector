/*                       requete.h    */
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

#ifndef REQUETE_H
#define REQUETE_H

#include <phrase.h>
#include <mot.h>
#include <regle.h>

class Phrase;
class Mot;
class Regle;

class Requete : public QObject
{

    Q_OBJECT

    private:
        Requete*          _coord1;
        bool              _activee;
        QList<MotFlechi*> _exx;
        QStringList       _hist;
        Lemme*            _lemme;
        QString           _morpho;
        bool              _morte;
        int               _num;
        Phrase*           _phrase;
        Regle*            _regle;
        Mot*              _requerant;
        MotFlechi*        _sub;
        bool              _subRequis;
        MotFlechi*        _super;
    public:
        Requete(MotFlechi* sup, MotFlechi* sub, Regle* r);
        bool        activee();
        QString     aff();
        void        ajHist(QString h);
        void        annuleRequis(QString cause);
        Requete*    clone();
        bool        close();
        bool        contigue();
        Requete*    coord1();
        bool        croise(Requete* req);
        int         distance();
        int         distance(Mot* m);
        QString     doc();
        bool        enConflit(QString id);
        bool        estEx(MotFlechi* mf);
        MotFlechi*  ex(int i);
        QString     fonction();
        int         freq();
        QString     gv(QString format="dot");
        int         handicap();
        bool        idConflit(QString id);
        QString     hist();
        bool        homonyme(Requete* req);
        bool        homoSuper(MotFlechi* mf);
        QString     html();
        bool        homoSub(MotFlechi* mf);
        QString     humain();
        QString     id();
        void        initExx();
        int         largeur();
        void        meurt();
        bool        morte();
        bool        multi();
        int         nbEx();
        int         nbPonct(QChar c);
        int         num();
        QString     numc();
        int         poids();
        QList<Mot*> portee();
        MotFlechi*  prima();
        bool        reciproque(Requete* req);
        Regle*      regle();
        Mot*        requerant();
        MotFlechi*  requerantFl();
        Mot*        requis();
        MotFlechi*  requisFl();
        QChar       sens();
        bool        separeparVConj();
        void        setCoord1(Requete* req);
        void        setNum(int n);
        void        setRequis(MotFlechi* m, QString cause="cause inconnue");
        void        setSub(MotFlechi *m);
        void        setSubRequis();
        void        setSuper(MotFlechi *m);
        void        setSuperRequis();
        MotFlechi*  sub();
        bool        subRequis();
        MotFlechi*  super();
        bool        superRequis();
        QString     tr();
        QString     trSub(); // traduction 
        MotFlechi*  ultima();
};

# endif
