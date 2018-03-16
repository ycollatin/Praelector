/*      ch.cpp
 *
 *  This file is part of COLLATINUS.
 *
 *  COLLATINUS is free software; you can redistribute it and/or modify
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
 *  along with COLLATINUS; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * © Yves Ouvrard, 2009 - 2016
 */

/**
 * \file ch.cpp
 * \brief constantes et utilitaires de traitement
 *        des chaînes de caractères

 *
 */




#include "ch.h"
#include <QRegExp>

#include <QDebug>

/**
 * \fn Ch::ajoute (QString mot, QStringList liste)
 * \brief Ajoute mot au début de chaque item de liste.
 */
QStringList Ch::ajoute(QString mot, QStringList liste)
{
    QStringList res;
    foreach (QString s, liste)
        res.append(s.prepend(mot));
    return res;
}

/**
 * \fn Ch::allonge(QString *f)
 * \brief modifie f pour que sa dernière voyelle
 *        devienne longue.
 */
void Ch::allonge(QString *f)
{
    if (f->isEmpty()) return;
    int taille = f->size();
    // Je sais que le morceau à attacher commence par une consonne.
    if (consonnes.contains(f->at(taille - 1)) &&
        !QString("\u0101e \u0101u \u0113u \u014de")
             .contains(f->mid(taille - 3, 2).toLower()))
    {
        f->replace(QRegExp("[a\u0103]([" + consonnes + "])$"), "\u0101\\1");
        f->replace(QRegExp("[e\u0115]([" + consonnes + "])$"), "\u0113\\1");
        f->replace(QRegExp("[i\u012d]([" + consonnes + "])$"), "\u012b\\1");
        f->replace(QRegExp("[o\u014F]([" + consonnes + "])$"), "\u014d\\1");
        f->replace(QRegExp("[u\u016d]([" + consonnes + "])$"), "\u016b\\1");
        f->replace(QRegExp("[y\u0233]([" + consonnes + "])$"), "\u045e\\1");
        f->replace(QRegExp("[A\u0102]([" + consonnes + "])$"), "\u0100\\1");
        f->replace(QRegExp("[E\u0114]([" + consonnes + "])$"), "\u0112\\1");
        f->replace(QRegExp("[I\u012c]([" + consonnes + "])$"), "\u012a\\1");
        f->replace(QRegExp("[O\u014e]([" + consonnes + "])$"), "\u014c\\1");
        f->replace(QRegExp("[U\u016c]([" + consonnes + "])$"), "\u016a\\1");
        f->replace(QRegExp("[Y\u0232]([" + consonnes + "])$"), "\u040e\\1");
    }
}

/**
 * \fn Ch:atone(QString a, bool bdc)
 * \brief supprime tous les diacritiques de la chaîne a
 *        si bdc est à true, les diacritiques des majuscules
 *        sont également supprimés.
 */
QString Ch::atone(QString a, bool bdc)
{
    // Supprimer le combining breve à la fin du mot
    // if (a.endsWith(0x0306)) a.chop(1);
    // minuscules
    a.replace(0x0101, 'a');
    a.replace(0x0103, 'a');  // ā ă
    a.replace(0x0113, 'e');
    a.replace(0x0115, 'e');  // ē ĕ
    a.replace(0x012b, 'i');
    a.replace(0x012d, 'i');  // ī ĭ
    a.replace(0x014d, 'o');
    a.replace(0x014f, 'o');  // ō ŏ
    a.replace(0x016b, 'u');
    a.replace(0x016d, 'u');  // ū ŭ
    a.replace(0x0233, 'y');
    a.replace(0x045e, 'y');  // ȳ ў
    if (!bdc)
    {
        // majuscule
        a.replace(0x0100, 'A');
        a.replace(0x0102, 'A');  // Ā Ă
        a.replace(0x0112, 'E');
        a.replace(0x0114, 'E');  // Ē Ĕ
        a.replace(0x012a, 'I');
        a.replace(0x012c, 'I');  // Ī Ĭ
        a.replace(0x014c, 'O');
        a.replace(0x014e, 'O');  // Ō Ŏ
        a.replace(0x016a, 'U');
        a.replace(0x016c, 'U');  // Ū Ŭ
        a.replace(0x0232, 'Y');
        a.replace(0x040e, 'Y');  // Ȳ Ў
    }
    // combining breve
    a.remove(0x0306);  //ō̆ etc.
    return a;
}

/**
 * \fn Ch:communes(QString g)
 * \brief note comme communes toutes les voyelles qui ne portent pas de quantité.
 */
QString Ch::communes(QString g)
{
    bool maj = g[0].isUpper();
    g = g.toLower();
    if (g.contains("a") || g.contains("e") || g.contains("i") || g.contains("o") || g.contains("u") || g.contains("y"))
    {
        g.replace("a","ā̆");
        g.replace("[^āăō]e","ē̆");
        g.replace("i","ī̆");
        g.replace("o","ō̆");
        g.replace("[^āēq]u","ū̆");
        g.replace("[^ā]y","ȳ̆");
    }
    if (maj) g[0] = g[0].toUpper();
    return g;
}

/**
 * \fn Ch::deQuant(QString *c)
 * \brief utilisée en cas d'élision.
 * supprime la quantité de la voyelle finale de la chaine c
 * lorsque cette voyelle est en fin de mot ou suivie d'un "m".
 */
void Ch::deQuant(QString *c)
{
    if (c->endsWith("\u0306"))
        c->chop(1);  // Supprimer le combining breve à la fin du mot
    c->replace(QRegExp("[\u0101\u0103](m?)$"), "a\\1");  // ā ă
    c->replace(QRegExp("[\u0113\u0115](m?)$"), "e\\1");
    c->replace(QRegExp("[\u012b\u012d](m?)$"), "i\\1");
    c->replace(QRegExp("[\u014d\u014f](m?)$"), "o\\1");
    c->replace(QRegExp("[\u016b\u016d](m?)$"), "u\\1");
    c->replace(QRegExp("[\u0232\u0233](m?)$"), "y\\1");
}

/**
 * \fn Ch::deAccent(QString *c)
 * \brief Supprime tous les accents d'un texte (acute, macron, breve)
 */
QString Ch::deAccent(QString c)
{
    c = c.normalized(QString::NormalizationForm_D, QChar::currentUnicodeVersion());
    c.remove("\u0301");
    c.remove("\u0306");
    c.remove("\u0304");
    return c;
}

/**
 * \fn QString Ch::deramise(QString r)
 * \brief retourne une graphie non-ramiste
 *        de r, càd dont tous les j deviennent i,
 *        et tous les v deviennent u. Les V majuscules
 *        initiaux sont ignorés.
 */
QString Ch::deramise(QString r)
{
    r.replace('J', 'I');
    r.replace('j', 'i');
    r.replace('v', 'u');
    r.replace("æ", "ae");
    r.replace("Æ", "Ae");
    r.replace("œ", "oe");
    r.replace("Œ", "Oe");
    r.replace(0x1ee5, 'u');  // ụ le u muet de suavis, suadeo, etc...
    if (!r.startsWith('V')) r.replace ('V', 'U');
    return r;
}

/**
 * \fn Ch::elide(QString *mp)
 * \brief met entre crochets la dernière syllabe de mp.
 */
void Ch::elide(QString *mp)
{
    //"Tāntāene"
    //bool debog = (*mp == "Tāntāene");
    //if (debog) qDebug() << "tantaene" << *mp;
    int taille = mp->size();
    if ((taille > 1) && ((mp->endsWith('m') || mp->endsWith("\u0101e")) ||
                         mp->endsWith("\u0306")) &&
        voyelles.contains(mp->at(taille - 2)))
    {
        deQuant(mp);
        mp->insert(taille - 2, '[');
        mp->append(']');
    }
    else if (voyelles.contains(mp->at(taille - 1)) && *mp != "\u014d")
    {
        deQuant(mp);
        mp->insert(taille - 1, '[');
        mp->append(']');
    }
}

void Ch::genStrNum(const QString s, QString *ch, int *n)
{
    ch->clear();
    *n = 0;
    for (int i = 0; i < s.length(); ++i)
        if (!s.at(i).isNumber())
            ch->append(s.at(i));
        else
        {
            *n = s.mid(i).toInt();
            break;
        }
}

/**
 * \fn Ch::sort_i(const QString &a, const QString &b)
 * \brief compare a et b sans tenire compte des diacritiques ni de la casse.
 */
bool Ch::sort_i(const QString &a, const QString &b)
{
    return QString::compare(atone(a), atone(b), Qt::CaseInsensitive) < 0;
}

QString Ch::jviu(QString ch)
{
    ch.replace('J', 'I');
    ch.replace('j', 'i');
    ch.replace('v', 'u');
    ch.replace('V', 'U');
    return ch;
}

QString Ch::ote_diacritiques (QString k)
{
    // minuscules
    k.replace (0x0101, 'a'); k.replace (0x0103, 'a');  // ā ă
    k.replace (0x0113, 'e'); k.replace (0x0115, 'e');  // ē ĕ
    k.replace (0x012b, 'i'); k.replace (0x012d, 'i');  // ī ĭ
    k.replace (0x014d, 'o'); k.replace (0x014f, 'o');  // ō ŏ
    k.replace (0x016b, 'u'); k.replace (0x016d, 'u');  // ū ŭ
    k.replace (0x0233, 'y'); k.replace (0x045e, 'y');  // ȳ ў
    // majuscule
    k.replace (0x0100, 'A'); k.replace (0x0102, 'A');  // Ā Ă
    k.replace (0x0112, 'E'); k.replace (0x0114, 'E');  // Ē Ĕ
    k.replace (0x012a, 'I'); k.replace (0x012c, 'I');  // Ī Ĭ
    k.replace (0x014c, 'O'); k.replace (0x014e, 'O');  // Ō Ŏ
    k.replace (0x016a, 'U'); k.replace (0x016c, 'U');  // Ū Ŭ
    k.replace (0x0232, 'Y'); k.replace (0x040e, 'Y');  // Ȳ Ў
    // combining breve
    k.remove (0x0306);
    return k;
}
