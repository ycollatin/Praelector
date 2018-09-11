/*
   flexfr - module de flexion pour la langue française.
   Copyright (C) 2001-2004 Yves Ouvrard.

   Ce programme est un logiciel libre ; vous pouvez le redistribuer et/ou le
   modifier conformément aux dispositions de la Licence Publique Générale GNU,
   telle que publiée par la Free Software Foundation ; version 2 de la licence,
   ou encore (à votre choix) toute version ultérieure.
   Ce programme est distribué dans l'espoir qu'il sera utile, mais SANS AUCUNE
   GARANTIE ; sans même la garantie implicite de COMMERCIALISATION ou
   D'ADAPTATION
   A UN OBJET PARTICULIER.
   Pour plus de détail, voir la Licence Publique Générale GNU .
   Vous devez avoir reçu un exemplaire de la Licence Publique Générale GNU en
   même temps que ce programme ; si ce n'est pas le cas, écrivez à la Free
   Software Foundation Inc., 675 Mass Ave, Cambridge, MA 02139, Etats-Unis.
   Pour tout contact avec l'auteur : yves.ouvrard@collatinus.org
   */

#include <flexfr.h>
#include <QStringList>
#include <QTextStream>

#include <QDebug>

using namespace std;

// chaînes de champs morpho
QString personne[7] = {" ", "je", "tu", "i", "nous", "vous", "ils"};

/*
   QString temps[9] = {" ", "présent", "futur", "imparfait", "passé simple",
   "passé [composé]", "futur antérieur",
   "plus-que-parfait", "passé antérieur"};
*/

QString mode[8] = {" ",
    "indicatif",     // 1
    "subjonctif",
    "conditionnel",
    "impératif",
    "infinitif",     // 5
    "participe",
    "gérondif"};

// QString voix[3] = {" ", "actif", "passif"};

QString genre[3] = {" ", "masculin", "féminin"};

QString nombre[3] = {" ", "singulier", "pluriel"};

QString degre[4] = {" ", "positif", "comparatif", "superlatif"};

QStringList AuxEtre = QStringList()
    << "accourir"
    << "advenir"
    << "aller"
    << "arriver"
    << "décéder"
    << "devenir"
    << "échoir"
    << "enlaidir"
    << "entrer,"
    << "intervenir"
    << "monter"
    << "mourir"
    << "naître"
    << "obvenir"
    << "partir"
    << "parvenir"
    << "provenir"
    << "redevenir"
    << "rentrer"
    << "reparaître"
    << "repartir"
    << "ressortir"
    << "rester"
    << "retomber"
    << "retourner"
    << "revenir"
    << "sortir"
    << "survenir"
    << "tomber"
    << "venir";

QStringList Eter = QStringList()
    << "acheter"
    << "racheter"
    << "bégueter"
    << "corseter"
    << "crocheter"
    << "fileter"
    << "fureter"
    << "haleter"
    << "celer"
    << "déceler"
    << "receler"
    << "ciseler"
    << "démanteler"
    << "écarteler"
    << "encasteler"
    << "geler"
    << "dégeler"
    << "congeler"
    << "surgeler"
    << "marteler"
    << "modeler"
    << "peler";

QStringList intransitifs = QStringList()
    << "aboutir"
    << "agir"
    << "appartenir"
    << "bavarder"
    << "boiter"
    << "bondir"
    << "bourdonner"
    << "briller"
    << "circuler"
    << "consister"
    << "contribuer"
    << "daigner"
    << "déjeuner"
    << "déplaire"
    << "dîner"
    << "durer"
    << "errer"
    << "étinceler"
    << "exister"
    << "falloir"
    << "frémir"
    << "frissonner"
    << "gambader"
    << "grelotter"
    << "grincer"
    << "hésiter"
    << "insister"
    << "jouir"
    << "luire"
    << "lutter"
    << "marcher"
    << "mentir"
    << "obéir"
    << "paître"
    << "parler"
    << "paraître"
    << "plaire"
    << "pleuvoir"
    << "pouvoir"
    << "procéder"
    << "profiter"
    << "réagir"
    << "résonner"
    << "resplendir"
    << "ressembler"
    << "retentir"
    << "rire"
    << "ronronner"
    << "rougir"
    << "ruisseler"
    << "sembler"
    << "sévir"
    << "souper"
    << "sourire"
    << "suffire"
    << "tarder"
    << "tinter"
    << "tournoyer"
    << "tressaillir"
    << "triompher"
    << "voltiger"
    << "voyager";

const int actif = 1;
const int passif = 2;

const int ind = 1;
const int subj = 2;
const int cond = 3;
const int imper = 4;
const int infin = 5;
const int part = 6;
// const int ger = 7;

const int pres = 1;
const int fut = 2;
const int impf = 3;
const int psimple = 4;
const int pcompose = 5;
const int futant = 6;
const int pqp = 7;
// const int pant = 8;

QChar derniere(QString s)
{
    if (s.isEmpty ()) return '\0';
    return s.at(s.length() - 1);
}

QString dernieres(QString s, size_t n)
{
    if (s.isEmpty ()) return "";
    return s.right(n);
}

QString deuxder(QString s)
{
    return s.right(2);
}
QString npremieres(QString s, int n)
{
    return s.left(n);
    // return s.substr (0, n);
}

QString otepremieres(QString s, int n)
{
    return s.remove(0, n);
    // return s.substr (n, s.length ());
}

QString otedernieres(QString s, int n)
{
    s.chop(n);
    return s;
}

bool IsLast(QString chaine, QString mot)
{
    // vrai si mot se termine par chaine
    return mot.endsWith(chaine);
}

QString morphoLatFr(QString lat)
{
    lat.replace("subjonctif imparfait", "conditionnel présent");
    lat.replace("subjonctif parfait", "subjonctif passé_composé");
    lat.replace("impératif futur", "impératif présent");
    lat.replace("infinitif parfait", "indicatif passé_composé 3ème singulier"); 
    lat.replace("impératif futur", "impératif présent");
    lat.replace("gérondif génitif", "infinitif présent actif");
    // uideor - paraître. pê pas le bon endroit
    //if (c == "paraître") lat.replace("passif", "actif");
    return lat;
}

/**
  Verbe est la classe de base. J'ai pris le premier groupe comme référence,
  puisqu'il représente la majorité des verbes}
  */
Verbe::Verbe(QString i)
{
    inf = i;
    pronominal = 0;
    modele = "Verbe";
}

Verbe::~Verbe() {}

const QStringList personnes = QStringList()
    << "1ère" << "2ème" << "3ème";
const QStringList temps = QStringList()
    << "présent"
    << "futur"
    << "imparfait"
    << "parfait"  //"passé simple"
    << "passé_composé"
    << "futur antérieur"
    << "PQP"
    << "passé antérieur";
const QStringList modes = QStringList()
    << "indicatif"
    << "subjonctif"
    << "conditionnel"
    << "impératif"
    << "infinitif"
    << "participe"
    << "gérondif";
const QStringList voix = QStringList()
    << "actif" << "passif";
const QStringList genres = QStringList()
    << "masculin" << "féminin";
const QStringList nombres = QStringList()
    << "singulier" << "pluriel";

QString Verbe::GetModele()
{
    return modele;
}

QString Verbe::GetDesFut(int index)
{
    QString D[7] = {"", "ai", "as", "a", "ons", "ez", "ont"};
    return D[index];
}

QString Verbe::GetDesImpf(int index)
{
    QString D[7] = {"", "ais", "ais", "ait", "ions", "iez", "aient"};
    return D[index];
}

QString Verbe::GetDesPsAi(int index)
{
    QString D[7] = {"", "ai", "as", "a", "âmes", "âtes", "èrent"};
    return D[index];
}

QString Verbe::GetDesPsI(int index)
{
    QString D[7] = {"", "is", "is", "it", "îmes", "îtes", "irent"};
    return D[index];
}

QString Verbe::GetDesPsU(int index)
{
    QString D[7] = {"", "us", "us", "ut", "ûmes", "ûtes", "urent"};
    return D[index];
}

QString Verbe::GetDesSubjPres(int index)
{
    QString D[7] = {"", "e", "es", "e", "ions", "iez", "ent"};
    return D[index];
}

// utiles
QString Verbe::circonflexe()
{
    QString result = IndPs(2);
    result = otedernieres(result, 1);
    // QString der = result.substr (result.length ()-1, 1);
    QString der = result.right(1);
    QString d;
    if (der == "a")
        d = "â";
    else if (der == "i" || der == "î")
        d = "î";
    else if (der == "u" || der == "û")
        d = "û";
    else
        d = "";
    result = otedernieres(result, 1);
    result.append(d);
    /*
       wchar_t der = derniere (result);
       wchar_t d;
       if (der == 'a') d = L'â';
       else if (der == 'i' || der == L'î') d = L'î';
       else if (der == 'u' || der == L'û') d = L'û';
       else d = '\0';
       result[result.size () - 1] = d;
       */
    return result;
}

// auxiliaire
QString Verbe::auxiliaire()
{
    if (AuxEtre.contains(inf)) return "être";
    return "avoir";
}

bool Verbe::pas_de_passif(QString inf)
{
    if (inf.startsWith("s'") || inf.startsWith("se ")) return true;
    return (intransitifs.contains(inf) || AuxEtre.contains(inf));
}

// radicaux
QString Verbe::RadPres(int P)
{
    if (P < 1 || P > 6) return "Verbe::RadPres, numéro de personne invalide ";
    return otedernieres(inf, 2);
}

QString Verbe::RadFut()
{
    if (inf.endsWith('r')) return inf;
    return otedernieres(inf, 1);
}

QString Verbe::RadPs()
{
    return otedernieres(inf, 2);
}

// manipulations : pronoms-radical-désinence
QString Verbe::elide(QString A, QString B)
{
    QString voyelles = "aâeéêiîoôuûy";
    // if (A.endsWith ('a') && voyelles.contains (B[0]))
    if (A.endsWith('e') && voyelles.contains(B[0]))
        return otedernieres(A, 1) + "'" + B;
    return A + " " + B;
}

QString Verbe::Pron(int P, QString F)
{
    QString result = personne[P];
    return elide(result, F);
}

QString Verbe::RD(QString R, QString D)
{
    // RD joint radical et désinence en gérant
    // les -e- et les cédilles.
    if (!D.isEmpty() && QString("oauâû").contains(D[0]))
    {
        if (R.endsWith('c'))
            R.replace(R.length() - 1, 1, "ç");
        else if (R.endsWith('g'))
            R.append("e");
    }
    return R + D;
}

// temps
QString Verbe::IndPres(int P)
{
    QString R = RadPres(P);
    if (R == "") return "";
    QString D[7] = {"", "e", "es", "e", "ons", "ez", "ent"};
    return RD(R, D[P]);
}

QString Verbe::IndFut(int P)
{
    return RD(RadFut(), GetDesFut(P));
}

QString Verbe::IndImpf(int P)
{
    return RD(RadPres(4), GetDesImpf(P));
}

QString Verbe::IndPs(int P)
{
    if (RadPs().isEmpty()) return "";
    return RD(RadPs(), GetDesPsAi(P));
}

QString Verbe::SubjPres(int P)
{
    if (P == 4 || P == 5) return RadPres(4) + GetDesSubjPres(P);
    return RD(RadPres(6), GetDesSubjPres(P));
}

QString Verbe::ImperPres(int P)
{
    if (P == 2)
    {
        if (deuxder(inf) == "er") return otedernieres(inf, 1);
        return IndPres(2);
    }
    else if (P == 4 || P == 5)
        return IndPres(P);
    return "";
}

QString Verbe::PartPres()
{
    return RadPres(4) + "ant";
}

QString Verbe::OteReflechi(QString F)
{
    // éliminer le pronom réfléchi.
    if (npremieres(F, sizeof("s'")) == "s'")
        return otepremieres(F, sizeof("s'"));
    else if (npremieres(F, 3) == "se ")
        return otepremieres(F, 3);
    return F;
}

QString Verbe::PP()
{
    // return otedernieres (inf, 2).append ("é");
    return RD(otedernieres(inf, 2), "é");
}

// procédure de flexion
QString Verbe::conjugue(int P, int T, int M, int V, bool pr, int g, int n)
{
    QString aux = "";
    bool PPPP = false;
    QString result = "";
    QString infinitif = inf;
    // vérifier la validité du champ P(ersonne)

    // pronominal
    if (npremieres(inf, 2) == "s'" || npremieres(inf, 3) == "se ")
    {
        pronominal = true;
        inf = OteReflechi(inf);
        aux = "être";
    }
    else
    {
        pronominal = false;
        aux = auxiliaire();
    }
    // invalider le passif des intransitifs
    if (V > 1 && pas_de_passif(inf) && M != part) return "(pas de passif)";  // pas de passif;

    // prévoir le PPPP (part. présent ou passé pronominal)
    PPPP = ((M == part) && pronominal);

    // éliminer le pronom sujet
    if (M == imper || M == part || M == infin) pr = false;

    // 1. voix
    if (V == actif)
    {
        if (M == ind)  // indicatif actif
        {
            if (T == pres)
                result = IndPres(P);
            else if (T == fut)
                result = IndFut(P);
            else if (T == impf)
                result = IndImpf(P);
            else if (T == psimple)
                result = IndPs(P);
            else
                result = compose(aux, P, T - 4, ind, actif);
        }
        else if (M == subj)  // subjonctif actif
        {
            if (T == pres)
            {
                result = SubjPres(P);
            }
            else if (T == impf)
                if (!RadPs().isEmpty())
                {
                    if (P == 3)
                        result = circonflexe() + "t";
                    else
                        result = IndPs(2) + "s" + GetDesSubjPres(P);
                }
                else
                    result = "";
            else if (T == pcompose)  // 5
                result = compose(aux, P, pres, subj, actif);
            else if (T == pqp)
                result = compose(aux, P, impf, subj, actif);
            else
                result = "";
        }
        else if (M == cond)
        {
            if (T == pres)
                // calcul à partir du futur
                result = RD(RadFut(), GetDesImpf(P));
            else if (T == pcompose)
                result = compose(aux, P, pres, cond, actif);
            else
                result = "";
        }
        else if (M == imper)
        {
            if (P == 1 || P == 3 || P == 6)
                result = "";
            else
            {
                if (T == pres)
                    result = ImperPres(P);
                else if (T == pcompose)
                    result = compose(aux, P, pres, imper, actif);
            }
        }
        else if (M == infin)  // inf:
        {
            if (T == pres)
                return infinitif;
            else if (T == pcompose)
                result = compose(aux, P, pres, infin, actif);
        }
        else if (M == part)
        {
            if (T == pres)
                result = PartPres();
            else if (T == pcompose)
                result = compose(aux, P, pres, part, actif);
            else if (T == psimple)
                result = PP();
        }
    }
    else if (V == passif)
    {
        /* si seul le part passé est demandé */
        if (M == part)
        {
            switch (T)
            {
                case pres:
                    return PartPres();
                case psimple:
                    {
                        QString res = PP();
                        if (g == 2) res.append("e");
                        if (n == 2 && (!res.endsWith("s"))) res.append("s");
                        return res;
                    }
                default:
                    break;
            }
        }
        if (PP() > "" && (aux == "avoir") && (inf != "être"))
            result = compose("être", P, T, M, actif);
        else
            result = "";
    }
    if (result.isEmpty()) return "";
    // éliminer le pronom
    if (result.size() > 2)
    {
        if (npremieres(result, 2) == "s'")
            result = otepremieres(result, 2);
        else if (npremieres(result, 3) == "se ")
            result = otepremieres(result, 3);
    }
    if (pronominal)
    {
        QString PR[7] = {"", "me", "te", "se", "nous", "vous", "se"};
        result.prepend(PR[P]+" ");
    }
    if (pr) result = Pron(P, result);
    else if (PPPP) result = elide("se", result);
    return result;
}

QString TVeter::RD(QString R, QString D)
{
    // repérer le futur/conditionnel par la finale du radical
    bool desR = (R.endsWith('r'));
    int p;
    // repérer la dernière occurence de e dans le radical
    // if (desR) p = R.rfind('e', R.length()-3);
    if (desR)
        p = R.lastIndexOf('e', R.length() - 3);
    else
        p = R.lastIndexOf('e', R.length() - 2);
    // transformer en è si l''initiale de D est e
    if ((D.startsWith('e') && (!D.endsWith('z'))) || desR) R.replace(p, 1, "è");
    return R + D;
}

QString TVettell::RD(QString R, QString D)
{
    // repérer le futur/conditionnel par la finale du radical
    bool desR = R.endsWith('r');
    int p;
    // repérer la dernière occurence de e dans le radical
    if (desR)
        p = R.lastIndexOf('e', R.length() - 3);
    else
        p = R.lastIndexOf('e', R.length() - 1);
    // transformer en è si l''initiale de D est e
    if ((D[0] == 'e' && !D.endsWith('z')) || desR)  // R[-1] == 'r'
    {
        // QString redoubl = R.substr (p+1,1);
        // R.insert (p+1, redoubl);
        R.insert(p + 1, R.at(p + 1));
    }
    return R + D;
}

QString TVebrer::RD(QString R, QString D)
{
    // repérer la dernière occurence de é dans le radical
    size_t p = R.lastIndexOf("é");
    // transformer en è si l''initiale de D est e.
    if (p < string::npos && D[0] == 'e' &&
        (!D.endsWith('z') || R.endsWith('r')))
        R.replace(p, 1, "è");
    return R + D;
}

QString TVyer::RD(QString R, QString D)
{
    // transformer en i si l''initiale de D est e.
    if (D[0] == 'e' && !D.endsWith('z')) R = otedernieres(R, 1) + "i";
    return R + D;
}

QString TVyer::RadFut()
{
    if (IsLast("envoyer", inf)) return otedernieres(inf, 4) + "err";
    return otedernieres(inf, 3) + "ier";
}

QString TVavoir::RadFut()
{
    return "aur";
}

QString TVavoir::IndPres(int P)
{
    QString D[7] = {"", "ai", "as", "a", "avons", "avez", "ont"};
    return D[P];
}

QString TVavoir::IndImpf(int P)
{
    return RD("av", GetDesImpf(P));
}

QString TVavoir::IndPs(int P)
{
    return RD("e", TVavoir::GetDesPsU(P));
}

QString TVavoir::SubjPres(int P)
{
    QString D[7] = {"", "aie", "aies", "ait", "ayons", "ayez", "aient"};
    return D[P];
}

QString TVavoir::ImperPres(int P)
{
    QString D[7] = {"", "", "aie", "", "ayons", "ayez", ""};
    return D[P];
}

QString TVavoir::PartPres()
{
    return "ayant";
}

QString TVavoir::PP()
{
    return "eu";
}

QString TVetre::RadFut()
{
    return "ser";
}

QString TVetre::RadPs()
{
    return "f";
}

QString TVetre::IndPres(int P)
{
    QString D[7] = {"", "suis", "es", "est", "sommes", "êtes", "sont"};
    return D[P];
}

QString TVetre::IndImpf(int P)
{
    return RD("ét", GetDesImpf(P));
}

QString TVetre::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVetre::SubjPres(int P)
{
    QString D[7] = {"", "sois", "sois", "soit", "soyons", "soyez", "soient"};
    return D[P];
}

QString TVetre::ImperPres(int P)
{
    if (P == 2 || P == 4 || P == 5)
        return SubjPres(P);
    else
        return "";
}

QString TVetre::PartPres()
{
    return "étant";
}

QString TVetre::PP()
{
    return "été";
}

QString TValler::RadFut()
{
    return otedernieres(inf, 5) + "ir";
}

QString TValler::IndPres(int P)
{
    QString D[7] = {"", "vais", "vas", "va", "allons", "allez", "vont"};
    return D[P];
}

QString TValler::SubjPres(int P)
{
    QString D = GetDesSubjPres(P);
    if (P == 4 || P == 5) return "all" + D;
    return "aill" + D;
}

QString TValler::ImperPres(int P)
{
    if (P == 2)
        return otedernieres(inf, 5) + "va";
    else
        return Verbe::ImperPres(P);
}

QString TVcueillir::RadFut()
{
    QString R = inf;
    R.replace(R.length() - 2, 1, "e");
    return R;
}

QString TVcueillir::PP()
{
    return otedernieres(inf, 2) + "i";
}

QString TVsst::RadPres(int P)
{
    if (P < 1 || P > 6) return "TVsst::RadPres, numéro de personne invalide ";
    return otedernieres(inf, 2);
}

QString TVsst::IndPres(int P)
{
    QString R = RadPres(P);
    QString D;
    if (R.isEmpty()) return "";
    if (P < 3)
        D = "s";
    else if (P == 3)
        D = "t";
    else
        return Verbe::IndPres(P);
    return RD(R, D);
}

QString TVsst::IndPs(int P)
{
    if (RadPs().isEmpty()) return "";
    return RD(RadPs(), GetDesPsI(P));
}

QString TVsst::PP()
{
    // QString radical = RadPs() + "i";
    return RadPs() + "i";
}

// héritiers de TVsst

TVaitre::TVaitre(QString i) : TVsst(i)
{
    modele = "TVaitre";
    pcirc = inf.lastIndexOf("î");  //, inf.length ());
}

QString TVaitre::RadPres(int P)
{
    // pcirc = inf.rfind ("î", inf.length ());
    if (P == 1 || P == 2) return npremieres(inf, pcirc) + "i";
    if (P == 3) return otedernieres(inf, 3);
    return npremieres(inf, pcirc) + "iss";
}

QString TVaitre::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVaitre::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVaitre::PP()
{
    // QString result = RadPs() + "u";
    return RadPs() + "u";
}

QString TVnaitre::RadPs()
{
    return npremieres(inf, pcirc) + "qu";
}

QString TVnaitre::PP()
{
    return npremieres(inf, pcirc - 1) + "é";
}

QString TVnaitre::IndPs(int P)
{
    return RadPs() + GetDesPsI(P);
}

QString TVpaitre::IndPs(int P)
{
    if (inf == "paître") return "";
    return RadPs() + GetDesPsU(P);
}

QString TVpaitre::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVpaitre::PP()
{
    if (inf == "paître") return "";
    return RadPs() + "u";
}

QString TVboire::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 2);
    if (P < 6) return otedernieres(inf, 4) + "uv";
    return otedernieres(inf, 2) + "v";
}

QString TVboire::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVboire::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVboire::SubjPres(int P)
{
    if (P == 4 || P == 5) return RadPres(4) + GetDesSubjPres(P);
    return TVsst::SubjPres(P);
}

QString TVboire::PP()
{
    return RadPs() + "u";
}

QString TVbouillir::RadPres(int P)
{
    if (P > 3) return otedernieres(inf, 2);
    return otedernieres(inf, 5);
}

QString TVcevoir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 6) + "çoi";
    if (P < 6) return otedernieres(inf, 3);
    return otedernieres(inf, 6) + "çoiv";
}

QString TVcevoir::RadFut()
{
    return otedernieres(inf, 3) + "r";
}

QString TVcevoir::RadPs()
{
    return otedernieres(inf, 6) + "c";
}

QString TVcevoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVcevoir::PP()
{
    return RD(RadPs(), "u");
}

QString TVchoir::RadPres(int P)
{
    if (P < 1 || P > 6) return "TVchoir, RadPres, numéro de personne invalide ";
    return otedernieres(inf, 1);
}

QString TVchoir::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVchoir::IndPres(int P)
{
    if (inf == "déchoir")
    {
        if (P == 4) return otedernieres(inf, 2) + "yons";
        if (P == 5) return otedernieres(inf, 2) + "yez";
    }
    if (P == 4 || P == 5) return "";
    return TVsst::IndPres(P);
}

QString TVchoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVchoir::SubjPres(int P)
{
    if (inf == "échoir") return TVsst::SubjPres(P);
    return "";
}

QString TVchoir::PP()
{
    return RadPs() + "u";
}

QString TVclore::RadPres(int P)
{
    if (P < 3) return otedernieres(inf, 2);
    if (P == 3) return otedernieres(inf, 3) + "ô";
    if (P < 6)
    {
        if (inf != "clore") return otedernieres(inf, 2) + "s";
        return "";
    }
    return otedernieres(inf, 2) + "s";
}

QString TVclore::IndPres(int P)
{
    if (RadPres(P) > "") return TVsst::IndPres(P);
    return "";
}

QString TVclore::IndPs(int P)
{
    if (P < 1 || P > 6) return "TVclore, IndPs, numéro de personne invalide ";
    return "";
}

QString TVclore::PP()
{
    return otedernieres(inf, 2) + "s";
}

QString TVclure::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVclure::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVclure::PP()
{
    return RadPs() + "u";
}

QString TVconfire::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 2);
    if (inf != "frire") return otedernieres(inf, 2) + "s";
    return "";
}

QString TVconfire::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVconfire::PP()
{
    return RadPs() + "it";
}

QString TVcourir::RadFut()
{
    return otedernieres(inf, 2) + "r";
}

QString TVcourir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVcourir::PP()
{
    return RadPs() + "u";
}

QString TVcroire::RadPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 3) + "y";
    return otedernieres(inf, 2);
}

QString TVcroire::RadPs()
{
    // return otedernieres (inf, 4);
    return "cr";
}

QString TVcroire::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVcroire::PP()
{
    return RadPs() + "u";
}

QString TVcroitre::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 3);
    return otedernieres(inf, 4) + "iss";
}

QString TVcroitre::RadPs()
{
    return otedernieres(inf, 5) + "û";
}

QString TVcroitre::IndPs(int P)
{
    QString D[7] = {"", "s", "s", "t", "mes", "tes", "rent"};
    return RD(RadPs(), D[P]);
}

QString TVcroitre::PP()
{
    return otedernieres(inf, 4) + "û";
}

QString TVdevoir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 5) + "oi";
    if (P < 6) return otedernieres(inf, 3);
    return otedernieres(inf, 5) + "oiv";
}

QString TVdevoir::RadFut()
{
    return otedernieres(inf, 3) + "r";
}

QString TVdevoir::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVdevoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVdevoir::PP()
{
    return RadPs() + "û";
}

QString TVdire::RadPres(int P)
{
    if (P > 0 and P < 4) return otedernieres(inf, 2);
    return otedernieres(inf, 2) + "s";
}

QString TVdire::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVdire::IndPres(int P)
{
    if (inf == "dire" && P == 5) return "dites";
    return TVsst::IndPres(P);
}

QString TVdire::PP()
{
    return otedernieres(inf, 2) + "t";
}

QString TVdormir::RadPres(int P)
{
    if (P > 3) return otedernieres(inf, 2);
    return otedernieres(inf, 3);
}

QString TVecrire::RadPres(int P)
{
    if (P > 3) return otedernieres(inf, 2) + "v";
    return otedernieres(inf, 2);
}

QString TVecrire::RadPs()
{
    return RadPres(4);
}

QString TVecrire::PP()
{
    return otedernieres(inf, 2) + "t";
}

QString TVfaillir::IndPres(int P)
{
    QString D[7] = {"",         "faux",    "faux",    "faut",
                    "faillons", "faillez", "faillent"};
    return D[P];
}

QString TVfaire::RadPres(int P)
{
    if (P > 3) return TVsst::RadPres(P) + "s";
    return TVsst::RadPres(P);
}

QString TVfaire::RadFut()
{
    return otedernieres(inf, 4) + "er";
}

QString TVfaire::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVfaire::IndPres(int P)
{
    if (P == 5) return otedernieres(inf, 2) + "tes";
    if (P == 6) return otedernieres(inf, 4) + "ont";
    return TVsst::IndPres(P);
}

QString TVfaire::SubjPres(int P)
{
    return otedernieres(inf, 3) + "ss" + GetDesSubjPres(P);
}

QString TVfaire::PP()
{
    return IndPres(3);
}

QString TVfuir::RadPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 2) + "y";
    return otedernieres(inf, 1);
}

QString TVgesir::RadPres(int P)
{
    if (P < 3) return "gi";
    if (P == 3) return "gî";
    return "gis";
}

QString TVgesir::IndFut(int P)
{
    if (P < 1 || P > 6) return "numéro de personne invalide ";
    return "";
}

QString TVgesir::IndPs(int P)
{
    if (P < 1 || P > 6) return "numéro de personne invalide ";
    return "";
}

QString TVgesir::PP()
{
    return "";
}

QString TVindre::RadPres(int P)
{
    if (P < 4)
        return otedernieres(inf, 3);
    else
        return otedernieres(inf, 4) + "gn";
}

QString TVindre::RadPs()
{
    return RadPres(4);
}

QString TVindre::PP()
{
    return IndPres(3);
}

QString TVir::RadPres(int P)
{
    QString result = otedernieres(inf, 1);
    if (P > 3) return result + "ss";
    return result;
}

QString TVir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsI(P));
}

QString TVir::PP()
{
    return otedernieres(inf, 2) + "i";
}

QString TVlire::RadPres(int P)
{
    if (P > 3) return otedernieres(inf, 2) + "s";
    return otedernieres(inf, 2);
}

QString TVlire::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVlire::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVlire::PP()
{
    return RadPs() + "u";
}

QString TVtaire::RadPs()
{
    return otedernieres(inf, 4);
}
QString TVtaire::IndPs(int P)
{
    return RadPs() + GetDesPsU(P);
}

QString TVmettre::RadPres(int P)
{
    if (P == 1 || P == 2) return otedernieres(inf, 3);
    if (P == 3) return otedernieres(inf, 4);
    return otedernieres(inf, 2);
}

QString TVmettre::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVmettre::PP()
{
    return RadPs() + "is";
}

QString TVmourir::RadPres(int P)
{
    if (P == 4 || P == 5) return "mour";
    return "meur";
}

QString TVmourir::RadFut()
{
    return otedernieres(inf, 2) + "r";
}

QString TVmourir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVmourir::PP()
{
    return "mort";
}

QString TVmouvoir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 6) + "eu";
    if (P < 6) return otedernieres(inf, 3);
    return otedernieres(inf, 6) + "euv";
}

QString TVmouvoir::RadFut()
{
    return otedernieres(inf, 3) + "r";
}

QString TVmouvoir::RadPs()
{
    return otedernieres(inf, 6);
}

QString TVmouvoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVmouvoir::PP()
{
    if (inf == "mouvoir") return "mû";
    return RadPs() + "u";
}

QString TVouir::RadPres(int P)
{
    if (P == 4 || P == 5) return "oy";
    return "oi";
}

QString TVouir::IndPs(int P)
{
    QString D[7] = {"", "ouïs", "ouïs", "ouït", "ouîmes", "ouîtes", "ouïrent"};
    return D[P];
}

QString TVouir::PP()
{
    return "ouï";
}

QString TVplaire::RadPres(int P)
{
    if (P < 3) return otedernieres(inf, 2);
    if (P == 3) return otedernieres(inf, 3) + "î";
    return otedernieres(inf, 2) + "s";
}

QString TVplaire::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVplaire::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVplaire::PP()
{
    return RadPs() + "u";
}

QString TVpleuvoir::IndPres(int P)
{
    if (P == 3) return otedernieres(inf, 4) + "t";
    if (P == 6) return otedernieres(inf, 3) + "ent";
    return "";
}

QString TVpleuvoir::RadFut()
{
    return otedernieres(inf, 3) + "r";
}

QString TVpleuvoir::RadPs()
{
    return otedernieres(inf, 6);
}

QString TVpleuvoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVpleuvoir::IndImpf(int P)
{
    return otedernieres(inf, 3) + GetDesImpf(P);
}

QString TVpleuvoir::SubjPres(int P)
{
    if (P == 3 || P == 6) return otedernieres(inf, 3) + GetDesSubjPres(P);
    return "";
}

QString TVpleuvoir::PP()
{
    return RadPs() + "u";
}

QString TVpleuvoir::conjugue(int P, int T, int M, int V, bool Pr)
{
    if (P == 3 || P == 6) return Verbe::conjugue(P, T, M, V, P, Pr);
    return "";
}

QString TVpouvoir::RadFut()
{
    return otedernieres(inf, 4) + "rr";
}

QString TVpouvoir::RadPs()
{
    return "p";
}

QString TVpouvoir::IndPres(int P)
{
    QString D[7] = {"", "peux", "peux", "peut", "pouvons", "pouvez", "peuvent"};
    return D[P];
}

QString TVpouvoir::IndImpf(int P)
{
    return RD(otedernieres(inf, 3), GetDesImpf(P));
}

QString TVpouvoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVpouvoir::SubjPres(int P)
{
    return "puiss" + GetDesSubjPres(P);
}

QString TVpouvoir::PP()
{
    return "pu";
}

QString TVpourvoir::RadPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 2) + "y";
    return otedernieres(inf, 1);
}

QString TVpourvoir::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVpourvoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVpourvoir::PP()
{
    return RadPs() + "u";
}

QString TVvoir::RadPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 2) + "y";
    return otedernieres(inf, 1);
}

/*
   QString TVvoir::RadFut()
   {
   if (inf == "voir" || inf == "revoir") return otedernieres (inf, 4) + "ier";
   if (P < 6) return R + "ér";
   return R + "ièr";
   }
   */

QString TVvoir::RadFut()
{
    return otedernieres(inf, 3) + "err";
}

QString TVvoir::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVvoir::PP()
{
    return otedernieres(inf, 3) + "u";
}

QString TVrire::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVsavoir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 4) + "i";
    return otedernieres(inf, 3);
}

QString TVsavoir::RadFut()
{
    return otedernieres(inf, 4) + "ur";
}

QString TVsavoir::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVsavoir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVsavoir::SubjPres(int P)
{
    return otedernieres(inf, 4) + "ch" + GetDesSubjPres(P);
}

QString TVsavoir::ImperPres(int P)
{
    QString D[7] = {"", "", "e", "", "ons", "ez", ""};
    if (D[P] > "") return otedernieres(inf, 4) + "ch" + D[P];
    return "";
}

QString TVsavoir::PartPres()
{
    return otedernieres(inf, 4) + "chant";
}

QString TVsavoir::PP()
{
    return RadPs() + "u";
}

QString TVservir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 3);
    return otedernieres(inf, 2);
}

QString TVquerir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 4) + "ier";
    if (P == 6) return otedernieres(inf, 4) + "ièr";
    return otedernieres(inf, 2);
}

QString TVquerir::PP()
{
    return otedernieres(inf, 4) + "is";
}

QString TVquerir::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVquerir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsI(P));
}

QString TVsoudre::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 3);
    return otedernieres(inf, 4) + "lv";
}

QString TVsoudre::RadPs()
{
    return otedernieres(inf, 4) + "";
}

QString TVsoudre::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVsoudre::PP()
{
    return RadPs() + "u";
}

QString TVsuivre::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 3);
    return otedernieres(inf, 2);
}

QString TVvivre::RadPs()
{
    return otedernieres(inf, 4) + "éc";
}

QString TVvivre::IndPs(int P)
{
    return RadPs() + GetDesPsU(P);
}

QString TVvivre::PP()
{
    return RadPs() + "u";
}

QString TVsurseoir::RadPres(int P)
{
    QString F = "surso";
    if (P == 4 || P == 5) return F + "y";
    return F + "i";
}

QString TVsurseoir::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVsurseoir::IndPs(int P)
{
    if (inf == "seoir") return "";
    return TVsst::IndPs(P);
}

QString TVsurseoir::PP()
{
    return IndPs(1);
}

QString TVvenir::RadFut()
{
    return otedernieres(inf, 4) + "iendr";
}

QString TVvenir::circonflexe()
{
    return otedernieres(inf, 4) + "în";
}

QString TVvenir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 4) + "ien";
    if (P < 6) return otedernieres(inf, 2);
    return otedernieres(inf, 4) + "ienn";
}

QString TVvenir::IndPs(int P)
{
    QString D[7] = {"", "ins", "ins", "int", "înmes", "întes", "inrent"};
    return otedernieres(inf, 4) + D[P];
}

QString TVvenir::PP()
{
    return otedernieres(inf, 2) + "u";
}
// ne pas permuter les 2 suivants

QString TVvetir::RadPres(int P)
{
    if (P == 3) return otedernieres(inf, 3);
    return TVsst::RadPres(P);
}

QString TVvetir::PP()
{
    return RadPs() + "u";
}

QString TVtir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 3);
    return TVsst::RadPres(P);
}

QString TVtir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsI(P));
}
// ne pas permuter les 2 précédents

QString TVuire::RadPres(int P)
{
    if (P > 3) return TVsst::RadPres(P) + "s";
    return TVsst::RadPres(P);
}

QString TVuire::RadPs()
{
    return RadPres(4);
}

QString TVuire::PP()
{
    if (inf == "luire" || inf == "nuire") return otedernieres(inf, 2);
    return IndPres(3);
}

QString TVtraire::RadPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 3) + "y";
    return otedernieres(inf, 2);
}

QString TVtraire::RadPs()
{
    return "";
}

QString TVtraire::PP()
{
    return IndPres(3);
}

QString TVvaincre::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 2);
    return otedernieres(inf, 3) + "qu";
}

QString TVvaincre::IndPres(int P)
{
    if (P == 3) return RadPres(3);
    return TVsst::IndPres(P);
}

QString TVvaincre::RadPs()
{
    return RadPres(4);
}

QString TVvaincre::PP()
{
    return otedernieres(inf, 3) + "cu";
}
// fin des héritiers de TVsst

// TVxxt, classe abstraite
QString TVxxt::RadPs()
{
    return otedernieres(inf, 3);
}

QString TVxxt::IndPres(int P)
{
    if (P < 3) return RadPres(P) + "x";
    return TVsst::IndPres(P);
}

QString TVxxt::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}
//  héritiers de TVxxt

QString TVvaloir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 4) + "u";
    return otedernieres(inf, 3);
}

QString TVvaloir::RadFut()
{
    return otedernieres(inf, 4) + "udr";
}

QString TVvaloir::SubjPres(int P)
{
    if (P == 4 || P == 5) return otedernieres(inf, 3) + GetDesSubjPres(P);
    return RD(otedernieres(inf, 4) + "ill", GetDesSubjPres(P));
}

QString TVvaloir::PP()
{
    return RadPs() + "u";
}

QString TVfalloir::IndPres(int P)
{
    if (P == 3) return "faut";
    return "";
}

QString TVfalloir::IndFut(int P)
{
    if (P == 3) return "faudra";
    return "";
}

QString TVfalloir::IndPs(int P)
{
    if (P == 3) return "fallut";
    return "";
}

QString TVfalloir::SubjPres(int P)
{
    if (P == 3) return "faille";
    return "";
}

QString TVfalloir::conjugue(int P, int T, int M, int V, bool pr)
{
    if (P != 3) return "";
    return Verbe::conjugue(P, T, M, V, pr);
}

QString TVvouloir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 6) + "eu";
    if (P < 6) return otedernieres(inf, 3);
    return otedernieres(inf, 6) + "eul";
}

QString TVvouloir::RadFut()
{
    return otedernieres(inf, 4) + "dr";
}

QString TVvouloir::SubjPres(int P)
{
    QString D = GetDesSubjPres(P);
    QString R;
    if (D[1] == 'e')
        R = "veuill";
    else
        R = "voul";
    return R + D;
}

QString TVvouloir::PP()
{
    return RadPs() + "u";
}

QString TVdre::IndPres(int P)
{
    QString D;
    if (P < 3)
        D = "s";
    else if (P == 3)
        D = "";
    else
        return Verbe::IndPres(P);
    return RD(RadPres(P), D);
}

QString TVdre::IndPs(int P)
{
    return RD(RadPs(), GetDesPsI(P));
}

QString TVdre::PP()
{
    return RadPs() + "u";
}

QString TVasseoir::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 4) + "ie";
    return otedernieres(inf, 3) + "y";
}

QString TVasseoir::IndImpf(int P)
{
    return RD(otedernieres(inf, 3) + "y", GetDesImpf(P));
}

QString TVasseoir::RadPs()
{
    return otedernieres(inf, 4);
}

QString TVasseoir::IndPres(int P)
{
    QString D;
    if (P < 3)
        D = "ds";
    else if (P == 3)
        D = "d";
    else
        return TVdre::IndPres(P);
    return RD(RadPres(P), D);
}

QString TVasseoir::PP()
{
    return RadPs() + "is";
}

QString TVcoudre::RadPres(int P)
{
    if (P < 4) return TVdre::RadPres(P);
    return otedernieres(inf, 3) + "s";
}

QString TVcoudre::RadPs()
{
    return RadPres(4);
}

QString TVmoudre::RadPres(int P)
{
    if (P < 4) return TVdre::RadPres(P);
    return otedernieres(inf, 3) + "";
}

QString TVmoudre::RadPs()
{
    return RadPres(4);
}

QString TVmoudre::IndPs(int P)
{
    return RD(RadPs(), GetDesPsU(P));
}

QString TVprendre::RadPres(int P)
{
    if (P < 4) return otedernieres(inf, 2);
    if (P < 6) return otedernieres(inf, 3);
    return otedernieres(inf, 3) + "n";
}

QString TVprendre::RadPs()
{
    return otedernieres(inf, 5);
}

QString TVprendre::SubjPres(int P)
{
    QString D = GetDesSubjPres(P);
    QString R = RadPres(4);
    if (D[0] == 'e') R += "n";
    return R + D;
}

QString TVprendre::PP()
{
    return IndPs(1);
}

QString TVouvrir::IndPs(int P)
{
    return RD(RadPs(), GetDesPsI(P));
}

QString TVouvrir::ImperPres(int P)
{
    if (P == 2) return otedernieres(inf, 2) + "e";
    return Verbe::ImperPres(P);
}

QString TVouvrir::PP()
{
    return otedernieres(inf, 3) + "ert";
}

QString TVbattre::RadPres(int P)
{
    if (P == 3) return otedernieres(inf, 4);
    return TVtir::RadPres(P);
}

QString TVbattre::PP()
{
    return RadPs() + "u";
}

QString Verbe::compose(QString A, int P, int T, int M, int V)
{
    QString partp = PP();
    // vérifier l'existence du participe passé
    if (!partp.isEmpty())
    {
        Verbe *aux;
        if (A == "avoir")
            aux = new TVavoir("avoir");
        else
            aux = new TVetre("être");
        QString fx = aux->conjugue(P, T, M, V, false, 0, 0);
        delete aux;
        QString result;
        if (!fx.isEmpty())
            result = fx + " " + partp;
        else
            return "";
        if ((P > 3 && A == "être") && !result.endsWith('s'))
            result.append("s");  // = result + "s";
        return result;
    }
    return "";
}

Verbe *verbe_m(QString inf)
{
    Verbe *Vb;
    // lexique
    //  auxiliaires

    Vb = NULL;
    if (inf == "être")
        Vb = new TVetre(inf);
    else if (inf == "avoir")
        Vb = new TVavoir(inf);
    else if (inf == "pouvoir")
        Vb = new TVpouvoir(inf);
    else if (inf == "vouloir")
        Vb = new TVvouloir(inf);

    // autres verbes
    else if (inf == "aller")
        Vb = new TValler(inf);
    else if (inf == "asservir")
        Vb = new TVir(inf);
    else if (inf == "faillir")
        Vb = new TVfaillir(inf);
    else if (inf == "frire" || inf == "circoncire")
        Vb = new TVconfire(inf);
    else if (inf == "gésir")
        Vb = new TVgesir(inf);
    else if (inf == "mourir")
        Vb = new TVmourir(inf);
    else if (inf == "offrir" || inf == "souffrir")
        Vb = new TVouvrir(inf);
    else if (inf == "ouïr")
        Vb = new TVouir(inf);
    else if (inf == "rire" || inf == "sourire")
        Vb = new TVrire(inf);
    else if (inf == "voir" || inf == "prévoir" || inf == "revoir")
        Vb = new TVvoir(inf);
    else if (inf == "se voir" || inf == "se revoir")
        Vb = new TVvoir(inf);
    else if (inf == "surseoir")
        Vb = new TVsurseoir(inf);
    else if (inf == "taire")
        Vb = new TVtaire(inf);

    // variantes du premier groupe
    else if (Eter.contains(inf)  // find (" "+inf+" ", 0) < QString::npos
             || IsLast("emer", inf) || IsLast("ener", inf) ||
             IsLast("eper", inf) || IsLast("erer", inf) ||
             IsLast("eser", inf) || IsLast("ever", inf) || IsLast("evrer", inf))
        Vb = new TVeter(inf);
    else if (IsLast("ébrer", inf) || IsLast("écer", inf) ||
             IsLast("écher", inf) || IsLast("écrer", inf) ||
             IsLast("éder", inf) || IsLast("éger", inf) ||
             IsLast("égler", inf) || IsLast("égner", inf) ||
             IsLast("égrer", inf) || IsLast("éguer", inf) ||
             IsLast("éler", inf) || IsLast("émer", inf) ||
             IsLast("éner", inf) || IsLast("éper", inf) ||
             IsLast("équer", inf) || IsLast("érer", inf) ||
             IsLast("éser", inf) || IsLast("éter", inf) ||
             IsLast("étrer", inf) || IsLast("évrer", inf) ||
             IsLast("éyer", inf))
    {
        Vb = new TVebrer(inf);
    }
    else if (IsLast("eler", inf) || IsLast("eter", inf))
        Vb = new TVettell(inf);

    else if (IsLast("yer", inf))
        Vb = new TVyer(inf);

    // modèle général 1er groupe
    else if (IsLast("er", inf))
        Vb = new Verbe(inf);

    // naître, paître et paraître : ne pas séparer
    else if (IsLast("naître", inf) and inf.size() < 9)  // != connaître
        Vb = new TVnaitre(inf);
    else if (IsLast("paître", inf))
        Vb = new TVpaitre(inf);
    else if (IsLast("aître", inf))
        Vb = new TVaitre(inf);
    // ne pas séparer les 3 précédentes
    else if (IsLast("asseoir", inf))
        Vb = new TVasseoir(inf);
    else if (IsLast("battre", inf))
        Vb = new TVbattre(inf);
    else if (IsLast("boire", inf))
        Vb = new TVboire(inf);
    else if (IsLast("bouillir", inf))
        Vb = new TVbouillir(inf);
    else if (IsLast("cevoir", inf))
        Vb = new TVcevoir(inf);
    else if (IsLast("choir", inf))
        Vb = new TVchoir(inf);
    else if (IsLast("clore", inf))
        Vb = new TVclore(inf);
    else if (IsLast("clure", inf))
        Vb = new TVclure(inf);
    else if (IsLast("confire", inf))
        Vb = new TVconfire(inf);
    else if (IsLast("courir", inf))
        Vb = new TVcourir(inf);
    else if (IsLast("croire", inf))
        Vb = new TVcroire(inf);
    else if (IsLast("croître", inf))
        Vb = new TVcroitre(inf);
    else if (IsLast("cueillir", inf))
        Vb = new TVcueillir(inf);
    else if (IsLast("devoir", inf))
        Vb = new TVdevoir(inf);
    else if (IsLast("dire", inf))
        Vb = new TVdire(inf);
    else if (IsLast("dormir", inf))
        Vb = new TVdormir(inf);
    else if (IsLast("faire", inf))
        Vb = new TVfaire(inf);
    else if (IsLast("écrire", inf) || IsLast("scrire", inf))
        Vb = new TVecrire(inf);
    else if (IsLast("fuir", inf))
        Vb = new TVfuir(inf);
    else if (IsLast("lire", inf))
        Vb = new TVlire(inf);
    else if (IsLast("mouvoir", inf))
        Vb = new TVmouvoir(inf);
    else if (IsLast("ouvrir", inf))
        Vb = new TVouvrir(inf);
    else if (IsLast("mettre", inf))
        Vb = new TVmettre(inf);
    else if (IsLast("plaire", inf))
        Vb = new TVplaire(inf);
    else if (IsLast("pleuvoir", inf))
        Vb = new TVpleuvoir(inf);
    else if (IsLast("pourvoir", inf))
        Vb = new TVpourvoir(inf);
    else if (IsLast("prendre", inf))
        Vb = new TVprendre(inf);
    else if (IsLast("quérir", inf))
        Vb = new TVquerir(inf);
    else if (IsLast("saillir", inf))
        Vb = new Verbe(inf);
    else if (IsLast("savoir", inf))
        Vb = new TVsavoir(inf);
    else if (IsLast("servir", inf))
        Vb = new TVservir(inf);
    else if (IsLast("soudre", inf))
        Vb = new TVsoudre(inf);
    else if (IsLast("suivre", inf))
        Vb = new TVsuivre(inf);
    else if (IsLast("traire", inf))
        Vb = new TVtraire(inf);
    else if (IsLast("vaincre", inf))
        Vb = new TVvaincre(inf);
    else if (IsLast("venir", inf) || IsLast("tenir", inf))
        Vb = new TVvenir(inf);
    else if (IsLast("uire", inf))
        Vb = new TVuire(inf);
    else if (IsLast("valoir", inf))
        Vb = new TVvaloir(inf);
    else if (inf == "falloir")
        Vb = new TVfalloir(inf);
    else if (IsLast("vivre", inf))
        Vb = new TVvivre(inf);

    // -dre
    else if (IsLast("indre", inf))
        Vb = new TVindre(inf);
    else if (IsLast("coudre", inf))
        Vb = new TVcoudre(inf);
    else if (IsLast("moudre", inf))
        Vb = new TVmoudre(inf);
    else if (IsLast("dre", inf))
        Vb = new TVdre(inf);
    // -tir
    else if (IsLast("vêtir", inf))
        Vb = new TVvetir(inf);
    else if (IsLast("mentir", inf) || IsLast("sentir", inf) ||
             IsLast("partir", inf) || IsLast("repentir", inf) ||
             IsLast("sortir", inf))
        Vb = new TVtir(inf);

    // 2ème groupe
    else if (IsLast("ir", inf))
        Vb = new TVir(inf);

    else
        Vb = NULL;

    return Vb;
}

// ------------------------------------------------------------
//  Flexion des noms
// ------------------------------------------------------------

Nom::Nom(QString s)
{
    sing = s;
    modele = "Nom";
}

Nom::~Nom()
{}

QString Nom::getModele()
{
    return modele;
}

QString Nom::pluriel()
{
    const QStringList als = QStringList()
        << "bal"
        << "cal"
        << "carnaval"
        << "chacal"
        << "festival"
        << "récital"
        << "régal";

    if (sing == "oeil" || sing == "œil")
        return "yeux";
    else if (sing == "ail")
        return "aulx";
    return sing + "s";
}

QString NomSXZ::pluriel()
{
    return sing;
}

QString NomAL::pluriel()
{
    return otedernieres(sing, 2) + "aux";
}

QString NomAIL::pluriel()
{
    return otedernieres(sing, 3) + "aux";
}

QString NomAUEU::pluriel()
{
    return sing + "x";
}

// ------------------------------------------------------------
//  Flexion des adjectifs
// ------------------------------------------------------------

Adjectif::Adjectif(QString a)
{
    graphie = a;
    modele = "Adjectif";
}

Adjectif::~Adjectif()
{}

QString Adjectif::getModele()
{
    return modele;
}

QString Adjectif::feminin()
{
    QString mascs[13] = {"bénin", "doux",   "faux", "favori", "fou",
                         "frais", "jaloux", "long", "malin",  "mou",
                         "muet",  "roux",   "sot"};
    QString fems[13] = {"bénigne", "douce",   "fausse", "favorite", "folle",
                        "fraîche", "jalouse", "longue", "maligne",  "molle",
                        "muette",  "rousse",  "sotte"};
    int i = index_t(mascs, graphie, 12);
    if (i > -1) return fems[i];
    if (derniere(graphie) == 'e') return graphie;
    return graphie + "e";
}

QString Adjectif::pluriel(bool fem)
{
    Nom *nom;
    if (fem)
        nom = new Nom(feminin());
    else
        nom = nom_m(graphie);
    return nom->pluriel();
}

QString Adjectif::accorde(int g, int n, int d)
{
    QString ret;
    if (g == 1)
        ret = feminin();
    else
        ret = graphie;
    if (n == 1) ret = pluriel(g > 0);
    if (d > 1) ret.prepend ("plus ");
    if (d == 3) ret.prepend ("le ");
    return ret;
}

// irrégularités :
// gu - guë

QString Gras::feminin()
{
    return graphie + "se";
}

QString Aigu::feminin()
{
    return graphie + "ë";
}

QString ElEil::feminin()
{
    return graphie + "le";
}

QString Al::pluriel(bool fem)
{
    if (fem) return Adjectif::pluriel(1);
    QString liste_al[7] = {"banal",   "bancal", "fatal", "final",
                           "glacial", "natal",  "naval"};
    if (index_t(liste_al, graphie, 6) > -1)
    {
        if (fem) return graphie + "es";
        return graphie + "s";
    }
    return otedernieres(graphie, 1) + "ux";
}

QString Al::accorde(int g, int n)
{
    if (n) return pluriel(g);
    return Adjectif::accorde(g, 0);
}

QString Eux::feminin()
{
    if (graphie == "vieux") return "vieille";
    return otedernieres(graphie, 1) + "se";
}

QString El::feminin()
{
    return otedernieres(graphie, 2) + "elle";
}

QString Er::feminin()
{
    return otedernieres(graphie, 2) + "ère";
}

QString AdjF::feminin()
{
    return otedernieres(graphie, 1) + "ve";
}

QString AdjC::feminin()
{
    QString mascs[4] = {"caduc", "grec", "public", "sec"};
    QString fems[4] = {"caduque", "grecque", "publique", "sèche"};
    int i = index_t(mascs, graphie, 3);
    if (i > -1) return fems[i];
    return graphie + "he";
}

QString Eau::feminin()
{
    return otedernieres(graphie, 2) + "lle";
}

QString Et::feminin()
{
    QString liste[10] =
    {
        "complet",   "concret",   "désuet",  "discret",
        "incomplet", "indiscret", "inquiet", "quiet",
        "replet",    "secret"
    };
    if (index_t(liste, graphie, 9) > -1)
        return otedernieres(graphie, 2) + "ète";
    return graphie + "te";
}

QString Mon::feminin()
{
    return otedernieres(graphie, 2) + "a";
}

QString Mon::pluriel(bool fem)
{
    if (fem) {};  // éviter un avertissement du compilateur;
    return otedernieres(graphie, 2) + "es";
}

QString Notre::pluriel(bool fem)
{
    if (fem) {}; // éviter avertissement
    return otedernieres(graphie, 3) + "s";
}

QString Bon::feminin()
{
    return graphie + "ne";
}

QString Tout::pluriel(bool fem)
{
    if (fem) return "toutes";
    return "tous";
}

Pronom::Pronom()
{
    map.insert("ce", "cette,ces,ces");
    map.insert("celui-ci", "celle-ci,ceux-ci,celles-ci");
    map.insert("celui", "celle,ceux,celles");
    map.insert("le sien","la sienne,les siens,les siennes");
    map.insert("le nôtre","la nôtre,les nôtres,les nôtres");
    map.insert("tout","toute chose,toutes choses,toutes choses");
    map.insert("un", "une,des,des");
}

QString Pronom::accorde(QString p, QString m)
{
    p = p.simplified();
    QString ret;
    bool acc = m.contains("acc");
    bool fem = m.contains("fém");
    bool plur = m.contains("plur");
    if (p == "il")
    {
        if (plur)
        {
            if (acc)
            {
                ret = "les";
            }
            else if (fem)
                ret = "elles";
            else ret = "ils";
        }
        else  // singulier
        {
            if (acc)
            {
                if (fem) ret = "la";
                else ret = "le";
            }
            else if (fem) ret = "elle";
            //else ret = p;
        }
    }
    else if (p == "qui")
    {
        if (acc) ret = "que";
        else if (m.contains("gén")) ret = "dont";
    }
    else if (fem)
    {
        if (plur) ret = map.value(p).section(',', 2, 2);
        else ret =  map.value(p).section(',', 0, 0);
    }
    else if (plur)
        ret = map.value(p).section(',', 1, 1);
    if (ret.isEmpty()) ret = p;
    return ret;
}

Flechisseur::Flechisseur(QObject *parent) : QObject(parent)
{
}

QString Flechisseur::conjugue(QString inf, int P, int T, int M, int V, bool Pr, int g, int n)
{
    Verbe *Vb;
    Vb = verbe_m(inf);
    // lexique
    //  auxiliaires
    if (Vb != NULL)
    {
        QString result = Vb->conjugue(P, T, M, V, Pr, g, n);
        delete Vb;
        return result;
    }
    return "";
}

QString Flechisseur::conjnat(QString inf, QString morpho)
{
    // Un verbe est souvent traduit par plusieurs mots.
    // praesto:l'emporter sur, être garant, fournir (praestat : imp. : il vaut mieux)
    // Il faut pouvoir trouver quel est le verbe, ne
    // conjuguer que lui, et le remettre à sa place.
    inf = inf.simplified();
    if (inf.isEmpty()) return "requête vide, conjugaison impossible";

    if (inf == "paraître") morpho.replace("passif", "actif");

    // formes réfléchies
    bool se = inf.startsWith("se ");
    if (inf.contains(" ") && !se)
    {
        return conjnat(inf.section(" ",0,0), morpho) +" "+ inf.section(" ",1);
    }

    int p = 0;
    int n = 0;
    int t = 0;
    int m = 0;
    int v = 0;
    int g = 0;

    // passé composé
    // morpho.replace("passé composé", "passé_composé");
    QStringList lm = morpho.split(' ');

    foreach (QString trait, lm)
    {
        if (personnes.contains(trait))    p = personnes.indexOf(trait)+1;
        else if (nombres.contains(trait)) n = nombres.indexOf(trait)+1;
        else if (temps.contains(trait))   t = temps.indexOf(trait)+1;
        else if (modes.contains(trait))   m = modes.indexOf(trait)+1;
        else if (voix.contains(trait))    v = voix.indexOf(trait)+1;
        else if (genres.contains(trait))  g = genres.indexOf(trait)+1;
    }
    if (p > 0 && n > 1) p+=3;
    if (se)
    {
        return conjugue(inf.section(" ",0,1), p, t, m, v, (p!=3 && p!=6), g, n)
            + " " + inf.section(" ",2);
    }
    return conjugue(inf, p, t, m, v, (p != 3 && p != 6), g, n);
}

int Flechisseur::index_t(QString t[], QString s, int limite)
{
    for (int i = 0; i <= limite; i++)
        if (t[i] == s)
        {
            return i;
        }
    return -1;
}

Nom* Flechisseur::nom_m(QString n)
{
    QString als[11] = {"bal", "cal", "carnaval", "cérémonial", "chacal",
                       "festival", "pal", "récital", "régal", "santal"};
    QString ail[7] = {"bail", "corail", "émail", "soupirail",
                      "travail", "vantail", "vitrail"};
    QString aueus[6] = {"landau", "sarrau", "bleu",
                        "pneu",   "émeu",   "lieu (poisson)"};
    QString oux[7] = {"bijou", "caillou", "chou", "genou",
                      "hibou", "joujou",  "pou"};
    QString inex = "pas de plurie";
    QString result = "nom_m Échec de la recherche";
    // QString nom = Nom(n);
    Nom *nom = NULL;
    QChar d = derniere(n);
    if (QString("sxz").contains(d))
        nom = new NomSXZ(n);
    else if (deuxder(n) == "al" && index_t(als, n, 9) < 0)
        nom = new NomAL(n);
    else if (n == "bétail")
        result = inex;
    else if (dernieres(n, 3) == "ail" && index_t(ail, n, 6) > -1)
        nom = new NomAIL(n);
    else if ((IsLast("eu", n) || IsLast("au", n)) && index_t(aueus, n, 5) < 0)
        nom = new NomAUEU(n);
    else if (index_t(oux, n, 6) > -1)
        nom = new NomAUEU(n);  // result = n + "x";
    else
        nom = new Nom(n);
    return nom;
}

QString Flechisseur::pluriel(QString l, QString n)
{
    if (!n.contains("plur")) return l;
    QString irregs[6] = {"bonhomme", "grand-mère",   "grand-père",
                         "madame",   "mademoiselle", "monsieur"};
    QString irregp[6] = {"bonshommes", "grands-mères",   "grands-pères",
                         "mesdames",   "mesdemoiselles", "messieurs"};
    int i = index_t(irregs, l, 5);
    if (i > -1) return irregp[i];
    QString result;
    Nom *nom = nom_m(l);
    if (nom != NULL)
    {
        result = nom->pluriel();
        delete nom;
        return result;
    }
    return "Échec de la recherche";
}

QString Flechisseur::accorde(QString adj, QString m)
{
    QString result;
    int genre = 0;
    int nombre = 0;
    int degre = 0;
    if (m.contains("féminin")) genre = 1;
    if (m.contains("pluriel")) nombre = 1;
    if (m.contains("comparatif"))      degre = 2;
    else if (m.contains("superlatif")) degre = 3;
    Adjectif *inst = NULL;
    if (IsLast("el", adj) || IsLast("eil", adj) || adj == "gentil" ||
        adj == "nul")
        inst = new ElEil(adj);
    else if (IsLast("al", adj))
        inst = new Al(adj);
    else if (IsLast("gu", adj))
        inst = new Aigu(adj);
    else if (IsLast("eux", adj))
        inst = new Eux(adj);
    else if (IsLast("el", adj))
        inst = new El(adj);
    else if (IsLast("er", adj))
        inst = new Er(adj);
    else if (IsLast("f", adj))
        inst = new AdjF(adj);
    else if (IsLast("c", adj))
        inst = new AdjC(adj);
    else if (IsLast("eau", adj))
        inst = new Eau(adj);
    else if (IsLast("et", adj))
        inst = new Et(adj);
    else if (adj == "las" || adj == "gras" || adj == "gros")
        inst = new Gras(adj);
    else if ((QStringList()
              << "mon"
              << "ton"
              << "son").contains(adj))
        inst = new Mon(adj);
    else if ((QStringList()
              << "notre"
              << "votre").contains(adj))
        inst = new Notre(adj);
    else if (IsLast("ien", adj) || IsLast("on", adj) || IsLast("yen", adj))
        inst = new Bon(adj);
    else if (adj == "tout")
        inst = new Tout(adj);
    else
        inst = new Adjectif(adj);
    if (inst != NULL)
    {
        result = inst->accorde(genre, nombre, degre);
        delete inst;
    }
    else
        result = "Échec de la recherche.";
    return result;
}
