/*                 regle.h */
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
 * © Yves Ouvrard, 2009 - 2018
 */

# ifndef REGLE_H
# define REGLE_H

# include <QStringList>
# include <modele.h>
# include <motflechi.h>

class Modele;
class MotFlechi;

class Regle : public QObject
{
    Q_OBJECT

    private:
        QString     _accord;
        int         _antepos;
        QString     _aff;
        int         _avContig;
        QStringList _conflits;
        int         _contig;
        QString     _doc;
        QString     _id;
        QStringList _exclus;
        QStringList _filtre;
        QStringList _lemmeSub;
        QStringList _lemmeSup;
        QString     _subEstSup;
        QString     _supEstSub;
        QStringList _supEstSup;
        QStringList _lsSub;
        QStringList _lsSup;
        QStringList _lSubExclus;
        QStringList _lSupExclus;
        QStringList _morphoSub;
        QStringList _morphoSup;
        int         _poids;
        QString     _posSub;
        QString     _posSup;
        QChar       _sens;
        QString     _traduction;
       
    public:
        Regle(QStringList ll);
        QString      accord();
        int          antepos();
        QString      aff();
        bool         conflit(QString id);
        bool         compatibleSubSub(Regle* r);
        bool         compatibleSubSup(Regle* r);
        bool         compatibleSupSub(Regle* r);
        bool         compatibleSupSup(Regle* r);
        int          contig();
        QString      doc();
        bool         estSub(MotFlechi* mf);
        bool         estSuper(MotFlechi* mf);
        bool         exclut(QString id);
        QString      id();
        QStringList  filtre();
        bool static  inclus(QString a, QString b); // tous les mots de a sont dans b
        bool static  intersect(QString a, QString b); // a et b ont au moins un char commun
        QStringList  lemmeSub();
        QStringList  lemmeSup();
        QString      lsSub(int i);
        QString      lsSup(int i);
        QStringList  morphoSub();
        QStringList  morphoSup();
        int          nbLsSub();
        int          nbLsSup();
        int          poids();
        QString      posSub();
        QString      posSup();
        QChar        sens();
        QString      subEstSup();
        QString      supEstSub();
        QStringList  supEstSup();
        bool         subExclu(QString s);
        bool         supExclu(QString s);
        QString      traduction();
};

# endif
