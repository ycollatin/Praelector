/*      lemme.cpp
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

#include "lemme.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include "ch.h"
#include <iostream>
#include "lemmatiseur.h"
#include "modele.h"

#include <QDebug>

/////////////
// RADICAL //
/////////////
/**
 * \fn Radical::Radical (QString g, int n, QObject *parent)
 * \brief Créateur de la classe Radical. g est la forme
 *        canonique avec ses quantités, n est le numéro du radical
 */
Radical::Radical(QString g, int n, QObject *parent)
{
    _lemme = qobject_cast<Lemme *>(parent);
    _grq = g;
    _gr = Ch::atone(_grq);
    _numero = n;
}

/**
 * \fn QString Radical::gr ()
 * \brief Renvoie la graphie du radical
 *        dépourvue de diacritiques.
 */
QString Radical::gr()
{
    return _gr;
}

/**
 * \fn QString Radical::grq ()
 * \brief Renvoie la graphie du radical
 *        pourvue de ѕes diacritiques.
 */
QString Radical::grq()
{
    return _grq;
}

/**
 * \fn Lemme* Radical::lemme ()
 * \brief Le lemme auquel appartient le radical.
 */
Lemme *Radical::lemme() { return _lemme; }

/**
 * \fn Modele* Radical::modele ()
 * \brief Le modèle de flexion du radical
 */

Modele *Radical::modele() { return _lemme->modele(); }
/**
 * \fn int Radical::numRad ()
 * \brief Le numéro du radical.
 */

int Radical::numRad() { return _numero; }
///////////
// LEMME //
///////////

/**
 * \fn Lemme::Lemme (QString linea, QObject *parent)
 * \brief Constructeur de la classe Lemme à partire de la
 *        ligne linea. *parent est le lemmatiseur (classe Lemmat).
 */
Lemme::Lemme(QString linea, int origin, QObject *parent)
{
    // cădo|lego|cĕcĭd|cās|is, ere, cecidi, casum|687
    //   0   1    2     3          4              5
    // ăvellāna=ăvēllāna,ăbēllāna|uita|||ae, f.|2
    //   0                         1  23  4     5
    _lemmatiseur = qobject_cast<Lemmat *>(parent);
    QStringList eclats = linea.split('|');
    QStringList lg = eclats.at(0).split('=');
    _cle = Ch::atone(Ch::deramise(lg.at(0)));
    _grd = oteNh(lg.at(0), _nh);
    if (lg.count() == 1)
        _grq = _grd;
    else
        _grq = lg.at(1);
    _gr = Ch::atone(_grq);
    _grModele = eclats.at(1);
    _modele = _lemmatiseur->modele(_grModele);
    _hyphen.clear();
    _renvoi.clear();
    _origin = origin;
    if (eclats.count() < 6)
    {
        std::cerr << qPrintable("lemmes.la, ligne mal formée:"+linea);
        return;
    }
    // lecture des radicaux, champs 2 et 3
    for (int i = 2; i < 4; ++i)
        if (!eclats.at(i).isEmpty())
        {
            QStringList lrad = eclats.at(i).split(',');
            foreach (QString rad, lrad)
                _radicaux[i-1].append(new Radical(rad, i - 1, this));
        }
    _lemmatiseur->ajRadicaux(this);

    _indMorph = eclats.at(4);
    _pos.clear();
    if (_indMorph.contains("adj."))
        _pos.append('a');
    if (_indMorph.contains("conj. sub."))
        _pos.append('s');
    else if (_indMorph.contains("conj."))
        _pos.append('c');
    if (_indMorph.contains("excl"))
        _pos.append('e');
    if (_indMorph.contains("interj"))
        _pos.append('i');
    if (_indMorph.contains("num."))
        _pos.append('m');
    else if (_indMorph.contains("pron."))
        _pos.append('p');
    if (_indMorph.contains("prép."))
        _pos.append('r');
    if (_indMorph.contains("adv."))
        _pos.append('d');
    if (_indMorph.contains(" npr.") 
        || _indMorph.contains("subst."))
        _pos.append('n');
    if (_pos.isEmpty())
    {
        _pos.append(_modele->pos()); // v et d
        if (_indMorph.contains("intr.")) _pos = "w";
    }
    // genre des noms
    if (_pos.contains('n'))
    {
        if (_indMorph.contains(" f."))
            _genre = "féminin";
        else if (_indMorph.contains(" n."))
            _genre = "neutre";
        else if (_indMorph.contains(" m."))
            _genre = "masculin";
    }

    QRegExp c("cf\\.\\s(\\w+)$");
    int pos = c.indexIn(_indMorph);
    if (pos > -1)
        _renvoi = c.cap(1);
    else
        _renvoi = "";
    _freq = eclats.at(5).toInt();
}

/**
 * \fn void Lemme::ajIrreg (Irreg *irr)
 * \brief Ajoute au lemme l'obet irr, qui représente
 *        une forme irrégulière. Lorsque les formes irrégulières
 *        sont trop nombreuses, ou lorsque plusieurs lemmes
 *        ont des formes analogues, mieux vaut ajouter un modèle
 *        dans data/modeles.la.
 */
void Lemme::ajIrreg(Irreg *irr)
{
    _irregs.append(irr);
    // ajouter les numéros de morpho à la liste
    // des morphos irrégulières du lemme :
    if (irr->exclusif()) _morphosIrrExcl.append(irr->morphos());
}

/**
 * \fn void Lemme::ajRadical (int i, Radical* r)
 * \brief Ajoute le radical r de numéro i à la map des
 *        radicaux du lemme.
 */
void Lemme::ajRadical(int i, Radical *r)
{
    _radicaux[i].append(r);
}

/**
 * \fn void Lemme::ajTrad (QString t, QString l)
 * \brief ajoute la traduction t de langue l à
 *        la map des traductions du lemme.
 */
void Lemme::ajTrad(QString t, QString l)
{
    _traduction[l] = t;
}

/**
 * \fn QString Lemme::ambrogio()
 * \brief Renvoie dans une chaîne un résumé
 *        de la traduction du lemme dans toutes les
 *        langues cibles disponibles.
 */
QString Lemme::ambrogio()
{
    QString retour;
    QTextStream ss(&retour);
    ss << "<hr/>" << humain() << "<br/>";
    ss << "<table>";
    foreach (QString lang, _traduction.keys())
    {
        QString trad = _traduction[lang];
        QString langue = _lemmatiseur->cibles()[lang];
        if (!trad.isEmpty())
            ss << "<tr><td>- " << langue << "</td><td>&nbsp;" << trad
               << "</td></tr>\n";
    }
    ss << "</table>";
    return retour;
}

/**
 * \fn QString Lemme::cle ()
 * \brief Renvoie la clé sous laquel le
 *        lemme est enregistré dans le lemmatiseur parent.
 */
QString Lemme::cle()
{
    return _cle;
}

/**
 * \fn QList<int> Lemme::clesR ()
 * \brief Retourne toutes les clés (formes non-ramistes
 *        sans diacritiques) de la map des radicaux du lemme.
 */
QList<int> Lemme::clesR()
{
    return _radicaux.keys();
}

bool Lemme::estDeponent()
{
    return _pos.contains('v') && _gr.endsWith("or");
}

/**
 * \fn bool Lemme::estIrregExcl (int nm)
 * \param nm : numéro de morpho
 * \brief Renvoie vrai si la forme irrégulière
 *        avec le n° nm remplace celle construite
 *        sur le radical , faux si la
 *        forme régulière existe aussi.
 */
bool Lemme::estIrregExcl(int nm)
{
    return _morphosIrrExcl.contains(nm);
}

int Lemme::freq() const
{
    return _freq;
}

QString Lemme::genre()
{
    return _genre;
}

/**
 * \fn return _gr;
 * \brief Retourne la graphie ramiste du lemme sans diacritiques.
 */
QString Lemme::gr()
{
    return _gr;
}

/**
 * \fn QString Lemme::grq ()
 * \brief Retourne la graphie ramiste du lemme sans diacritiques.
 */
QString Lemme::grq()
{
    return _grq;
}

/**
 * \fn QString Lemme::grModele ()
 * \brief Retourne la graphie du modèle du lemme.
 */
QString Lemme::grModele()
{
    return _grModele;
}

/**
 * \fn QString Lemme::humain (bool html, QString l)
 * \brief Retourne une chaîne donnant le lemme ramiste avec diacritiques,
 *        ses indications morphologiques et sa traduction dans la langue l.
 *        Si html est true, le retour est au format html.
 */
QString Lemme::humain(bool html, QString l)
{
    QString res;
    QString tr;
    if (!_renvoi.isEmpty())
    {
        Lemme *lr = _lemmatiseur->lemme(_renvoi);
        if (lr != 0) return lr->humain(html, l);
        else tr = "renvoi non trouvé";
    }
    else
        tr = traduction(l);
    if (html)
        QTextStream(&res) << "<strong>" << _grq << "</strong> "
                          << "<em>" << _indMorph << "</em> : " << tr;
    else
        QTextStream(&res) << _grq << ", " << _indMorph << " : " << tr;
    return res;
}

QString Lemme::indMorph()
{
    return _indMorph;
}

/**
 * \fn QString Lemme::irreg (int i, bool *excl)
 * \brief Renvoie la forme irrégulière de morpho i. excl devient
 *        true si elle est exclusive, false sinon.
 */
QString Lemme::irreg(int i, bool *excl)
{
    foreach (Irreg *ir, _irregs)
    {
        if (ir->morphos().contains(i))
        {
            *excl = ir->exclusif();
            return ir->grq();
        }
    }
    return "";
}

/**
 * \fn Modele* Lemme::modele ()
 * \brief Renvoie l'objet modèle du lemme.
 */
Modele *Lemme::modele()
{
    return _modele;
}

/**
 * \fn int Lemme::nh()
 * \brief Renvoie le numéro d'homonymie du lemme.
 */
int Lemme::nh()
{
    return _nh;
}

int Lemme::origin()
{
    return _origin;
}

/**
 * \fn QString Lemme::oteNh (QString g, int &nh)
 * \brief Supprime le dernier caractère de g si c'est
 *        un nombre et revoie le résultat après avoir
 *        donné la valeur de ce nombre à nh.
 */
QString Lemme::oteNh(QString g, int &nh)
{
    int c = g.right(1).toInt();
    if (c > 0)
    {
        nh = c;
        g.chop(1);
    }
    else
        c = 1;
    return g;
}

/**
 * \fn QChar Lemme::pos ()
 * \brief Renvoie un caractère représentant la
 *        catégorie (part of speech, pars orationis)
 *        du lemme.
 */
QString Lemme::pos()
{
    return _pos;
}

/**
 * \fn QList<Radical*> Lemme::radical (int r)
 * \brief Renvoie le radical numéro r du lemme.
 */
QList<Radical *> Lemme::radical(int r)
{
    return _radicaux.value(r);
}

/**
 * \fn bool Lemme::renvoi()
 * \brief Renvoie true si le lemme est une forme
 *        alternative renvoyant à une autre entrée
 *        du lexique.
 */
QString Lemme::renvoi()
{
    return _renvoi;
}

bool Lemme::synt(QString s)
{
    return _lexSynt.contains(s);
}

/**
 * \fn QString Lemme::traduction(QString l)
 * \brief Renvoie la traduction du lemme dans la langue
 *        cible l (2 caractères).
 */
QString Lemme::traduction(QString l)
{
    if (_traduction.keys().contains(l))
        return _traduction[l];
    else if (_traduction.keys().contains("fr"))
        return _traduction["fr"];
    else return _traduction["uk"];
}

/**
 *  \fn QString Lemme::traduction(QString l, QString pos)
 *  \brief permet de donner la traduction du lemme
 *  selon le pos. En cas de pos multiple, les traductions
 *  des autres pos sont éliminées.
 */
QString Lemme::traduction(QString l, QString pos)
{
    if (!_indMorph.contains(pos))
        return traduction(l);
    //if (!_pos.contains(pos)) return traduction(l);
    const QString lpos = "(adj|conj|excl|interj|num|pron|prép|adv| n|nt|subst| v|vb|intr)";
    QString t = traduction(l);
    // supprimer les parenthèses dans la ligne de bin/data/lemmes.fr
    t.remove(QRegExp("[(\\[][^)^\\]]*[)\\]]"));
    // préparer l'expression régulière pour qu'elle prenne le point en compte.
    pos.replace(".","\\.");
    // isoler la traduction de la pos : ce qui précède
    t.remove(QRegExp("^.*"+pos));
    // ce qui suit.
    t.remove(QRegExp("()"+lpos+"\\..*$"));
    return t;
}


/**
 *  \fn QString Lemme::traduction(QString l, QChar pos)
 *  \brief appelle traduction(QString l, QString pos)
 *  après avoir converti le QChar pos en QString
 *  en utilisant la fonction posString du lemmatiseur.
 */
QString Lemme::traduction(QString l, QChar pos)
{
    QString p = _lemmatiseur->posString(pos);
    return traduction(l, p);
}

/**
 * @brief Lemme::setHyphen
 * @param h : indique où se fait la césure.
 * \brief stocke l'information sur la césure étymologique du lemme
 */
void Lemme::setHyphen(QString h)
{
    _hyphen = h;
}

void Lemme::setLexSynt(QStringList m)
{
    _lexSynt = m;
}

void Lemme::setPos(QString p)
{
    _pos = p;
}

/**
 * @brief Lemme::getHyphen
 * @return la césure étymologique du lemme
 */
QString Lemme::getHyphen()
{
    return _hyphen;
}
