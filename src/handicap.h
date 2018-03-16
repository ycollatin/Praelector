/*    handicap.h   */
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

# ifndef HANDICAP_H
# define HANDICAP_H

#include <QObject>

class Handicap: public QObject
{

    Q_OBJECT

    private:
        QString _forme;
        QString _lemme;
        QString _lemmeExclu;
        int     _hand;
    public:
        Handicap(QStringList sl);
        QString forme();
        QString lemme();
        QString lemmeExclu();
        int     hand();
};

# endif
