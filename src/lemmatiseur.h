/*      lemmatiseur.h
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

#ifndef LEMMATISEUR_H
#define LEMMATISEUR_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QtCore/QCoreApplication>

#include "irregs.h"
#include "lemme.h"
#include "modele.h"

class Irreg;
class Lemme;
class Radical;

typedef struct
{
    QString grq;
    Lemme*  lemme;
    QString morpho;
    QString sufq;
    char pos;
} SLem;

typedef QMap<Lemme*, QList<SLem> > MapLem;

typedef QPair<QRegExp, QString> Reglep;

class Lemmat : public QObject
{
    Q_OBJECT

   private:
    // fonction d'initialisation
    void ajAssims();
    void ajContractions();
    void ajPos();
    int  aRomano (QString r);
    bool estRomain(QString r);
    void lisIrreguliers();
    void lisLexicoSyntaxe();
    void lisLexique();
    void lisModeles();
    void lisTraductions(bool base, bool extension);
    void posCf();
    // variables et utils
    QMap<QString, QString>           assims;
    QMap<QString, QString>           assimsq;
    QStringList                      cherchePieds(int nbr, QString ligne, int i, bool pentam);
    QMap<QString, QString>          _contractions;
    QMultiMap<QString, Desinence *> _desinences;
    QStringList                      formeq(QString forme, bool *nonTrouve, bool debPhr, int accent = 0);
    QMultiMap<QString, Irreg *>     _irregs;
    QString                         _cible;
    QMap<QString, QString>          _cibles;
    QMap<QString, Lemme *>          _lemmes;
    QMap<QString, QChar>             mapPos;
    QMap<QString, Modele *>         _modeles;
    QStringList                     _morphos;
    QMultiMap<QString, Radical *>   _radicaux;
    QList<Reglep>                   _reglesp;
    QString                         _resDir;
    QMap<QString, QString>          _variables;
    // options
    bool _alpha;
    bool _extension; // = false;
    bool _extLoaded;
    bool _formeT;
    bool _html;
    bool _majPert;
    bool _morpho;
    bool _nonRec;

   public:
    Lemmat(QObject *parent = 0, QString resDir="");
    void                   ajDesinence(Desinence *d);
    void                   ajModele(Modele *m);
    void                   ajRadicaux(Lemme *l);
    QString                assim(QString a);
    QString                assimq(QString a);
    QMap<QString, QString> cibles();
    QString                desassim(QString a);
    QString                desassimq(QString a);
    static QString         deramise(QString r);
    QStringList            frequences(QString txt);
    MapLem                 lemmatise(QString f);  // lemmatise une forme
    QStringList            lemmatiseF(QString f, bool deb);
    MapLem                 lemmatiseM(QString f, bool debPhr = true);
    Lemme                 *lemme(QString l);
    QStringList            lignesFichier(QString nf);
    void                   lisFichierLexique(QString filepath);
    Modele                *modele(QString m);
    QString                morpho(int i);
    QChar                  posChar(QString p);
    QString                posString(QChar p);
    QString                scandeTxt(QString texte, int accent = 0, bool stats = false);
    QMap<QString, QString> suffixes;
    QString                variable(QString v);
};

#endif
