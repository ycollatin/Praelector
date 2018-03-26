/*    handicap.cpp   */
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

#include <handicap.h>
#include <QDebug>

/**********************
      Classe Handicap
 **********************/

Handicap::Handicap(QStringList sl)
{
    // contôle
    if (sl.count() < 3)
    {
        qDebug()<<"ligne mal formée"<<sl;
    }
    _forme = sl.at(0);
    QString lem = sl.at(1);
    if (lem.startsWith('-'))
        _lemme = lem.mid(1);
    else _lemmeExclu = lem;
    _hand = sl.at(2).toInt();
}

QString Handicap::forme()
{
    return _forme;
}

QString Handicap::lemme()
{
    return _lemme;
}

QString Handicap::lemmeExclu()
{
    return _lemmeExclu;
}

int Handicap::hand()
{
    return _hand;
}
