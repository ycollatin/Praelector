/*                 lemmatiseur.cpp
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
 * \file lemmatiseur.cpp
 * \brief module de lemmatisation des formes latines
 */

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <iostream>

#include "ch.h"
#include "lemmatiseur.h"

#include <QDebug>
// #include <QElapsedTimer>
// #define DEBOG

/**
 * \fn Lemmat::Lemmat (QObject *parent)
 * \brief Constructeur de la classe Lemmat.
 *
 * Il définit quelques constantes, initialise
 * les options à false, et appelle les fonctions
 * de lecture des données : modèles, lexique,
 * traductions et irréguliers.
 */
Lemmat::Lemmat(QObject *parent, QString resDir) : QObject(parent)
{
    if (resDir.isEmpty())
        _resDir = qApp->applicationDirPath() + "/data/";
    else if (resDir.endsWith("/")) _resDir = resDir;
    else _resDir.append("/");
    // options
    _alpha = false;
    _formeT = false;
    _html = false;
    _majPert = false;
    _morpho = false;
    _extension = false;
    _extLoaded = false;
    // suffixes
    suffixes.insert("ne", "nĕ");
    suffixes.insert("que", "quĕ");
    suffixes.insert("ue", "vĕ");
    suffixes.insert("ve", "vĕ");
    suffixes.insert("st", "st");
    // assimilations
    ajAssims();
    // contractions
    ajContractions();
    // POS
    ajPos();
    // lecture des morphos
    _morphos = lignesFichier(_resDir + "morphos.la");

    lisModeles();
    lisLexique();
    lisTraductions(true, false);
    posCf();
    lisIrreguliers();
    lisLexicoSyntaxe();
}

/**
* \fn void Lemmat::ajAssims ()
* \brief définit les débuts de mots
* non assimilés, et associe à chacun sa
* forme assimilée.
*/
void Lemmat::ajAssims()
{
    // peupler la QMap assims
    QStringList lignes = lignesFichier(_resDir + "assimilations.la");
    foreach(QString lin, lignes)
    {
        if (lin.isEmpty()) continue;
        QStringList liste = lin.split(':');
        assimsq.insert(liste.at(0), liste.at(1));
        assims.insert(Ch::atone(liste.at(0)), Ch::atone(liste.at(1)));
    }
}

/**
 * \fn void Lemmat::ajContractions ()
 * \brief Établit une liste qui donne, pour chaque
 * contraction, la forme non contracte qui lui
 * correspond.
 */
void Lemmat::ajContractions()
{
    // peupler la QMap _contractions
    QStringList lignes = lignesFichier(_resDir + "contractions.la");
    foreach(QString lin, lignes)
    {
        QStringList liste = lin.split(':');
        _contractions.insert(liste.at(0), liste.at(1));
    }
}

/**
 * \fn void Lemmat::ajDesinence (Desinence *d)
 * \brief ajoute la désinence d dans la map des
 * désinences.
 */
void Lemmat::ajDesinence(Desinence *d)
{
    _desinences.insert(Ch::deramise(d->gr()), d);
}

/**
 * \fn void Lemmat::ajRadicaux (Lemme *l)
 * \brief Calcule tous les radicaux du lemme l,
 *  en se servant des modèles, les ajoute à ce lemme,
 *  et ensuite à la map *  des radicaux de la classe Lemmat.
 *
 */
void Lemmat::ajRadicaux(Lemme *l)
{
    // ablŭo=ā̆blŭo|lego|ā̆blŭ|ā̆blūt|is, ere, lui, lutum
    //      0        1    2    3         4
    // ăvellāna=ăvēllāna,ăbēllāna|uita|||ae, f.|2
    Modele *m = modele(l->grModele());
    // insérer d'abord les radicaux définis dans lemmes.la
    foreach (int i, l->clesR())
    {
        QList<Radical *> lr = l->radical(i);
        foreach (Radical *r, lr)
            _radicaux.insert(Ch::deramise(r->gr()), r);
    }
    // pour chaque radical du modèle
    // test avec ăvellāna=ăvēllāna,ăbēllāna|uita|||ae, f.|2
    foreach (int i, m->clesR())
    {
        if (l->clesR().contains(i)) continue;
        QStringList gs = l->grq().split(',');
        foreach (QString g, gs)
        {
            Radical *r = NULL;
            QString gen = m->genRadical(i);
            // si gen == 'K', le radical est la forme canonique
            if (gen == "K")
                r = new Radical(g, i, l);
            else
            {
                // sinon, appliquer la règle de formation du modèle
                int oter = gen.section(',', 0, 0).toInt();
                QString ajouter = gen.section(',', 1, 1);
                if (g.endsWith(0x0306)) g.chop(1);
                g.chop(oter);
                if (ajouter != "0") g.append(ajouter);
                r = new Radical(g, i, l);
            }
            l->ajRadical(i, r);
            _radicaux.insert(Ch::deramise(r->gr()), r);
        }
    }
}

void Lemmat::ajPos()
{
    mapPos.insert("adj.", 'a');
    mapPos.insert("adv.", 'd');
    mapPos.insert("conj.", 'c');
    mapPos.insert("excl.", 'e');
    mapPos.insert("interj.", 'i');
    mapPos.insert("num.", 'm');
    mapPos.insert("pron.", 'p');
    mapPos.insert("prép.", 'r');
    mapPos.insert("subst.", 'n');
    mapPos.insert(" v.", 'V');
    mapPos.insert("vb.", 'V');
    mapPos.insert("intr.", 'v');
}

/**
 * \fn QString Lemmat::assim (QString a)
 * \brief Cherche si la chaîne a peut subir
 *        une assimilation, et renvoie
 *        cette chaîne éventuellement assimilée.
 *        *version avec quantités*
 */
QString Lemmat::assim(QString a)
{
    foreach (QString d, assims.keys())
        if (a.startsWith(d))
        {
            a.replace(d, assims.value(d));
            return a;
        }
    return a;
}

int Lemmat::aRomano (QString r)
{
    if (r.size () == 0) return 0;
    // création de la table de conversion : pourrait être créée ailleurs.
    QMap<QChar,int> conversion;
    conversion['I']=1;
    conversion['V']=5;
    conversion['X']=10;
    conversion['L']=50;
    conversion['C']=100;
    conversion['D']=500;
    conversion['M']=1000;
    // calcul du résultat : ajout d'un terme si l'ordre est normal, soustraction sinon.
    int res=0;
    int conv_s = conversion[r[0]];
    for (int i = 0; i < r.count()-1; i++)
    {
        int conv_c = conv_s;
        conv_s = conversion[r[i+1]];
        if (conv_c < conv_s)
            res -= conv_c;
        else res += conv_c;
    }
    res += conv_s;
    return res;
}

/**
 * \fn QString Lemmat::assimq (QString a)
 * \brief Cherche si la chaîne a peut subir
 *        une assimilation, et renvoie
 *        cette chaîne éventuellement assimilée.
 *        *version avec quantités*
 */
QString Lemmat::assimq(QString a)
{
    foreach (QString d, assimsq.keys())
        if (a.startsWith(d))
        {
            a.replace(d, assimsq.value(d));
            return a;
        }
    return a;
}

/*
QMap<QString, QString> Lemmat::cibles()
{
    return _cibles;
}
*/

/**
 * \fn QString Lemmat::desassim (QString a)
 * \brief Essaie de remplacer l'assimilation de a
 *        par sa forme non assimilée, et renvoie
 *        le résultat.
 */
QString Lemmat::desassim(QString a)
{
    foreach (QString d, assims.values())
    {
        if (a.startsWith(d))
        {
            a.replace(d, assims.key(d));
            return a;
        }
    }
    return a;
}

/**
 * \fn QString Lemmat::desassimq (QString a)
 * \brief Essaie de remplacer l'assimilation de a
 *        par sa forme non assimilée, et renvoie
 *        le résultat.
 */
QString Lemmat::desassimq(QString a)
{
    foreach (QString d, assimsq.values())
    {
        if (a.startsWith(d))
        {
            a.replace(d, assimsq.key(d));
            return a;
        }
    }
    return a;
}

/**
 * Fonction de conversion
 * des chiffres romains en chiffres arabes.
 *
 */
bool Lemmat::estRomain(QString r)
{
    return !(r.contains(QRegExp ("[^IVXLCDM]"))
             || r.contains("IL")
             || r.contains("IVI"));
}

/**
 * \fn MapLem Lemmat::lemmatise (QString f)
 * \brief Le cœur du lemmatiseur
 *
 *  renvoie une QMap<Lemme*,QStringlist> contenant
 *  - la liste de tous les lemmes pouvant donner
 *    la forme f;
 *  - pour chacun de ces lemmes la QStringList des morphologies
 *    correspondant à la forme.
 */
MapLem Lemmat::lemmatise(QString f)
{
    MapLem result;
    if (f.isEmpty()) return result;
    QString f_lower = f.toLower();
    int cnt_v = f_lower.count("v");
    bool V_maj = f[0] == 'V';
    int cnt_ae = f_lower.count("æ");
    int cnt_oe = f_lower.count("œ");
    f = Ch::deramise(f);
    // formes irrégulières
    QList<Irreg *> lirr = _irregs.values(f);
    foreach (Irreg *irr, lirr)
    {
        foreach (int m, irr->morphos())
        {
            SLem sl = {irr->grq(), irr->lemme(), morpho(m), "", irr->lemme()->pos().at(0).toLatin1()};
            result[irr->lemme()].prepend(sl);
        }
    }
    // radical + désinence
    for (int i = 0; i <= f.length(); ++i)
    {
        QString r = f.left(i);
        QString d = f.mid(i);
        QList<Desinence *> ldes = _desinences.values(d);
        if (ldes.empty()) continue;
        // Je regarde d'abord si d est une désinence possible,
        // car il y a moins de désinences que de radicaux.
        // Je fais la recherche sur les radicaux seulement si la désinence existe.
        QList<Radical *> lrad = _radicaux.values(r);
        // ii noté ī
        // 1. Patauium, gén. Pataui : Patau.i -> Patau+i.i
        // 2. conubium, ablP conubis : conubi.s -> conubi.i+s
        if (d.startsWith('i') && !d.startsWith("ii") && !r.endsWith('i'))
            lrad << _radicaux.values(r + "i");
        if (lrad.empty()) continue;
        // Il n'y a rien à faire si le radical n'existe pas.
        foreach (Radical *rad, lrad)
        {
            Lemme *l = rad->lemme();
            foreach (Desinence *des, ldes)
            {
                if (des->modele() == l->modele() &&
                    des->numRad() == rad->numRad() &&
                    !l->estIrregExcl(des->morphoNum()))
                {
                    bool c = ((cnt_v==0)
                              ||(cnt_v == rad->grq().toLower().count("v")
                                 +des->grq().count("v")));
                    if (!c) c = (V_maj && (rad->gr()[0] == 'U')
                            && (cnt_v - 1 == rad->grq().toLower().count("v")));
                    c = c && ((cnt_oe==0)||(cnt_oe == rad->grq().toLower().count("ōe")));
                    c = c && ((cnt_ae==0)||(cnt_ae == rad->grq().toLower().count("āe")
                                            +des->grq().count("āe")));
                    if (c)
                    {
                        QString fq = rad->grq() + des->grq();
                        if (!r.endsWith("i") && rad->gr().endsWith("i"))
                            fq = rad->grq().left(rad->grq().size()-1) + "ī"
                                    + des->grq().right(des->grq().size()-1);
                        int numM;
                        if (l->modele()->gr() == "inv") numM = 416;
                        else numM = des->morphoNum();
                        QString morphosl = morpho(numM);
                        if (l->pos() == "n") morphosl.append(" "+l->genre());
                        SLem sl = {fq, l, morphosl, "", l->modele()->pos().toLatin1()};
                        result[l].prepend(sl);
                    }
                }
            }
        }
    }
    if (_extLoaded && !_extension && !result.isEmpty())
    {
        // L'extension est chargée mais je ne veux voir
        // les solutions qui en viennent que si toutes en viennent.
        MapLem res;
        foreach (Lemme *l, result.keys())
        {
            if (l->origin() == 0)
                res[l] = result[l];
        }

        if (!res.isEmpty()) result = res;
    }
    return result;
}

/**
 * \fn MapLem Lemmat::lemmatiseM (QString f, bool debPhr)
 * \brief Renvoie dans une MapLem les lemmatisations de la
 *        forme f. le paramètre debPhr à true indique qu'il
 *        s'agit d'un début de phrase, et la fonction
 *        peut tenir compte des majuscules pour savoir
 *        s'il s'agit d'un nom propre.
 */
MapLem Lemmat::lemmatiseM(QString f, bool debPhr)
{
    QString res;
    QTextStream fl(&res);
    // chiffres romains
    MapLem mm = lemmatise(f);
    if (estRomain(f))
    {
        QString lin = QString("%1|inv|||num.|1").arg(f);
        Lemme *romain = new Lemme(lin, 0, this);
        int nr = aRomano(f);
        romain->ajTrad(QString("%1").arg(nr), "fr");
        SLem sl = {f, 0, "inv","",'m'};
        QList<SLem> lsl;
        lsl.append(sl);
        mm.insert(romain, lsl);
    }
    if (f.isEmpty()) return mm;
    // suffixes
    foreach (QString suf, suffixes.keys())
        if (mm.empty() && f.endsWith(suf))
        {
            QString sf = f;
            sf.chop(suf.length());
            // TODO : aequeque est la seule occurrence
            // de -queque dans le corpus classique
            mm = lemmatiseM(sf, debPhr);
            bool sst = false;
            if (mm.isEmpty() && (suf == "st"))
            {
                sf += "s";
                mm = lemmatiseM(sf, debPhr);
                sst = true;
            }
            foreach (Lemme *l, mm.keys())
            {
                QList<SLem> ls = mm.value(l);
                for (int i = 0; i < ls.count(); ++i)
                    if (sst) mm[l][i].sufq = "t";
                    else mm[l][i].sufq += suffixes.value(suf); // Pour modoquest
            }
        }
    if (debPhr && f.at(0).isUpper())
    {
        QString nf = f.toLower();
        MapLem nmm = lemmatiseM(nf);
        foreach (Lemme *nl, nmm.keys())
            mm.insert(nl, nmm.value(nl));
    }
    // assimilations
    if (mm.empty())
    {
        QString fa = assim(f);
        if (fa != f)
        {
            MapLem nmm = lemmatiseM(fa);
            // désassimiler les résultats
            foreach (Lemme *nl, nmm.keys())
            {
                for (int i = 0; i < nmm[nl].count(); ++i)
                    nmm[nl][i].grq = desassimq(nmm[nl][i].grq);
                mm.insert(nl, nmm.value(nl));
            }
        }
    }
    if (mm.empty())
    {
        QString fa = desassim(f);
        if (fa != f)
        {
            MapLem nmm = lemmatise(fa);
            foreach (Lemme *nl, nmm.keys())
            {
                for (int i = 0; i < nmm[nl].count(); ++i)
                    nmm[nl][i].grq = assimq(nmm[nl][i].grq);
                mm.insert(nl, nmm.value(nl));
            }
        }
    }
    // contractions
    QString fd = f;
    foreach (QString cle, _contractions.keys())
        if (fd.endsWith(cle))
        {
            fd.chop(cle.length());
            if ((fd.contains("v") || fd.contains("V")))
                fd.append(_contractions.value(cle));
            else
                fd.append(Ch::deramise(_contractions.value(cle)));
            MapLem nmm = lemmatise(fd);
            foreach (Lemme *nl, nmm.keys())
            {
                int diff = _contractions.value(cle).size() - cle.size();
                // nombre de lettres que je dois supprimer
                for (int i = 0; i < nmm[nl].count(); ++i)
                {
                    int position = f.size() - cle.size() + 1;
                    // position de la 1ère lettre à supprimer
                    if (fd.size() != nmm[nl][i].grq.size())
                    {
                        // il y a une (ou des) voyelle(s) commune(s)
                        QString debut = nmm[nl][i].grq.left(position + 2);
                        position += debut.count("\u0306"); // Faut-il vérifier que je suis sur le "v".
                    }
                    nmm[nl][i].grq = nmm[nl][i].grq.remove(position, diff);
                }
                mm.insert(nl, nmm.value(nl));
            }
        }
    // majuscule initiale
    if (mm.empty())
    {
        f[0] = f.at(0).toUpper();
        MapLem nmm = lemmatise(f);
        foreach (Lemme *nl, nmm.keys())
            mm.insert(nl, nmm.value(nl));
    }
    return mm;
}

/**
 * \fn Lemme* Lemmat::lemme (QString l)
 * \brief cherche dans la liste des lemmes le lemme
 *        dont la clé est l, et retourne le résultat.
 */
Lemme *Lemmat::lemme(QString l)
{
    return _lemmes.value(l);
}

QStringList Lemmat::lignesFichier(QString nf)
{
    QFile f(nf);
    f.open(QFile::ReadOnly);
    QTextStream flux(&f);
    QStringList retour;
    while (!flux.atEnd())
    {
        QString lin = flux.readLine();
        if ((!lin.isEmpty()) && ((!lin.startsWith("!")) || lin.startsWith("! --- ")))
            retour.append(lin.simplified());
    }
    f.close();
    return retour;
}

/**
 * \fn void Lemmat::lisIrreguliers()
 * \brief Chargement des formes irrégulières
 *        du fichier data/irregs.la
 */
void Lemmat::lisIrreguliers()
{
    QStringList lignes = lignesFichier(_resDir + "irregs.la");
    foreach (QString lin, lignes)
    {
        if (lin.isEmpty()) continue;
        Irreg *irr = new Irreg(lin, this);
        if (irr != 0 && irr->lemme() != 0)
            _irregs.insert(Ch::deramise(irr->gr()), irr);
#ifdef DEBOG
        else
            std::cerr << "Irréguliers, erreur dans la ligne" << qPrintable(lin);
#endif
    }
    // ajouter les irréguliers aux lemmes
    foreach (Irreg *ir, _irregs)
        ir->lemme()->ajIrreg(ir);
}

/**
 * \fn void Lemmat::lisFichierLexique(filepath)
 * \brief Lecture des lemmes, synthèse et enregistrement
 *        de leurs radicaux
 */
void Lemmat::lisFichierLexique(QString filepath)
{
    int orig = 0;
    if (filepath.endsWith("ext.la")) orig = 1;
    QStringList lignes = lignesFichier(filepath);
    foreach (QString lin, lignes)
    {
        // contrôle de format
        QStringList eclats = lin.split('|');
        if (eclats.count() < 6)
        {
            std::cerr << qPrintable ("ligne mal formée:"+lin);
            continue;
        }
        Lemme *l = new Lemme(lin, orig, this);
        _lemmes.insert(l->cle(), l);
    }
}

void Lemmat::lisLexicoSyntaxe()
{
    QStringList lignes = lignesFichier(_resDir+"lexsynt.la");
    while (lignes.count() > 0)
    {
        QString lin = lignes.takeFirst();
        QStringList eclats = lin.split(':',QString::SkipEmptyParts);
        if (eclats.count() > 1)
        {
            Lemme *l = lemme(eclats.at(0));
            QStringList lsynt = eclats.at(1).split(',',QString::SkipEmptyParts);
            l->setLexSynt(lsynt);
        }
    }
}

/**
 * \fn void Lemmat::lisLexique()
 * \brief Lecture du fichier de lemmes de base
 */
void Lemmat::lisLexique()
{
    Lemmat::lisFichierLexique(_resDir + "lemmes.la");
}

/**
 * \fn void Lemmat::lisModeles()
 * \brief Lecture des modèles, synthèse et enregistrement
 *        de leurs désinences
 */
void Lemmat::lisModeles()
{
    QStringList lignes = lignesFichier(_resDir + "modeles.la");
    int max = lignes.count()-1;
    QStringList sl;
    for (int i=0;i<=max;++i)
    {
        QString l = lignes.at(i);
        if (l.startsWith('$'))
        {
            _variables[l.section('=', 0, 0)] = l.section('=', 1, 1);
            continue;
        }
        QStringList eclats = l.split(":");
        if ((eclats.at(0) == "modele" || i == max) && !sl.empty())
        {
            Modele *m = new Modele(sl, this);
            _modeles.insert(m->gr(), m);
            sl.clear();
        }
        sl.append(l);
    }
}

/**
 * \fn void Lemmat::lisTraductions()
 * \brief Lecture des fichiers de traductions
 *        trouvés dans data/, nommés lemmes, avec
 *        un suffixe corresponant à la langue cible
 *        qu'ils fournissent.
 */
void Lemmat::lisTraductions(bool base, bool extension)
{
    QDir rep;
    if (!base && !extension) return;
    if (base && extension) {
        rep = QDir(_resDir, "lem*.*");
    } else if (base) {
        rep = QDir(_resDir, "lemmes.*");
    } else {
        rep = QDir(_resDir, "lem_ext.*");
    }
    QStringList ltr = rep.entryList();
    if (base) {
        ltr.removeOne("lemmes.la");  // n'est pas un fichier de traductions
    }
    if (extension) {
        ltr.removeOne("lem_ext.la");  // n'est pas un fichier de traductions
    }
    foreach (QString nfl, ltr)
    {
        // suffixe
        QString suff = QFileInfo(nfl).suffix();
        QStringList lignes = lignesFichier(_resDir + nfl);
        // lire le nom de la langue
        QString lang = lignes.takeFirst();
        lang = lang.mid(1);
        _cibles[suff] = lang;
        foreach(QString lin, lignes)
        {
            Lemme *l = lemme(Ch::deramise(lin.section(':', 0, 0)));
            if (l != 0) l->ajTrad(lin.section(':', 1), suff);
#ifdef DEBOG
            else
                std::cerr << "traduction, erreur dans la ligne" << lin
                         << "\n  clé" << Ch::deramise(lin.section(':', 0, 0));
#endif
        }
    }
}

/**
 * \fn Modele * Lemmat::modele (QString m)
 * \brief Renvoie l'objet de la classe Modele dont le nom est m.
 */
Modele *Lemmat::modele(QString m) { return _modeles[m]; }
/**
 * \fn QString Lemmat::morpho (int m)
 * \brief Renvoie la chaîne de rang m dans la liste des morphologies
 *        donnée par le fichier data/morphos.la
 */
QString Lemmat::morpho(int m)
{
    if (m < 0 || m >= _morphos.count()+1) return "morpho, erreur d'indice";
    //if (m == _morphos.count() - 1) return "-";
    return _morphos.at(m - 1);
}

/**
 * \fn void Lemmat::posCf()
 * \brief récupère les entrées cf. pos des entrées renvoyées
 */
void Lemmat::posCf()
{
    foreach (Lemme *l, _lemmes)
    {
        if (!l->renvoi().isEmpty())
        {
            Lemme *lr = lemme(l->renvoi());
            if (lr != 0)
            {
                l->setPos(lr->pos());
                l->ajTrad(lr->traduction("fr"), "fr");
            }
        }
    }
}

QString Lemmat::posString(QChar p)
{
    return mapPos.key(p,"n.");
}

/**
 * \fn QString Lemmat::variable (QString v)
 * \brief permet de remplacer la métavariable v
 *        par son contenu. Ces métavariables sont
 *        utilisées par le fichier modeles.la, pour
 *        éviter de répéter des suites de désinences.
 *        Elles sont repérées comme en PHP, par leur
 *        premier caractère $.
 */
QString Lemmat::variable(QString v)
{
    return _variables[v];
}
