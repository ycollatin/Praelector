/*
 FLEXFR - module de flexion pour la langue française.
 Copyright (C) 2001-2004 Yves Ouvrard.

Ce programme est un logiciel libre ; vous pouvez le redistribuer et/ou le
modifier conformment aux dispositions de la Licence Publique Gnrale GNU,
telle que publie par la Free Software Foundation ; version 2 de la licence,
ou encore ( votre choix) toute version ultrieure.
Ce programme est distribu dans l'espoir qu'il sera utile, mais SANS AUCUNE
GARANTIE ; sans mme la garantie implicite de COMMERCIALISATION ou D'ADAPTATION
A UN OBJET PARTICULIER.
Pour plus de dtail, voir la Licence Publique Gnrale GNU .
Vous devez avoir reu un exemplaire de la Licence Publique Gnrale GNU en mme
temps que ce programme ; si ce n'est pas le cas, crivez  la
Free Software Foundation Inc., 675 Mass Ave, Cambridge, MA 02139, Etats-Unis.
Pour tout contact avec l'auteur : yves.ouvrard@collatinus.org
*/

#ifndef LIBFLEXFR_H
#define LIBFLEXFR_H

#include <QString>

/**
Verbe est la classe de base. J'ai pris le premier groupe comme référence,
 puisqu'il représente la majorité des verbes}
 */
class Verbe
{
	protected:
		QString inf;
		QString modele;
		int pronominal;
	public:
		Verbe (QString i);
		virtual ~Verbe ();

		virtual QString GetModele ();
		virtual QString GetDesFut(int index);
		virtual QString GetDesImpf(int index);
		virtual QString GetDesPsAi(int index);
		virtual QString GetDesPsI(int index);
		virtual QString GetDesPsU(int index);
		virtual QString GetDesSubjPres(int index);
		// utiles
		virtual QString circonflexe();
		// auxiliaire
		virtual QString auxiliaire();
		// radicaux
		virtual QString RadPres(int P);
		virtual QString RadFut();
		virtual QString RadPs();
		// manipulations : pronoms-radical-dsinence
		virtual QString elide(QString A, QString B);
		virtual QString Pron(int P, QString F, bool refl);
		virtual QString RD (QString R, QString D);
		virtual QString compose (QString A, int P, int T, int M, int V);
		// temps
		virtual QString IndPres (int P);
		virtual QString IndFut (int P);
		virtual QString IndImpf (int P);
		virtual QString IndPs (int P);
		virtual QString SubjPres (int P);
		virtual QString ImperPres (int P);
		virtual QString PartPres ();
		virtual QString OteReflechi (QString F);
		virtual QString PP ();
		// procdure de flexion
		virtual QString conjugue (int P, int T, int M, int V, bool pr=false, int g=0, int n=0);
};

class TVeter: public Verbe
{
	public:
		TVeter (QString i): Verbe (i) {modele="TVeter";}
		QString RD (QString R, QString D);
};

class  TVettell: public Verbe
{
	/* Verbes en -eter et -eler qui doublent leur t (ou l)
		devant  les dsinences en -e */
	public:
		TVettell (QString i): Verbe (i) {modele="TVeTTel";}
		QString RD (QString R, QString D);
};

class  TVebrer: public Verbe
{
	public:
		TVebrer (QString i): Verbe (i) {modele="TVebrer";}
		virtual QString RD(QString R, QString D);
};

class TVyer: public Verbe
{
	public:
		TVyer (QString i): Verbe (i) {}
		virtual QString RD(QString R, QString D);
		virtual QString RadFut();
};

class TVavoir: public Verbe
{
	public:
		TVavoir (QString i): Verbe (i) {}
		virtual QString RadFut ();
		virtual QString IndPres (int P);
		virtual QString IndImpf (int P);
		virtual QString IndPs (int P);
		virtual QString SubjPres (int P);
		virtual QString ImperPres(int P);
		virtual QString PartPres();
		virtual QString PP ();
};

class TVetre: public Verbe
{
	public:
		TVetre (QString i): Verbe (i) {}
		virtual QString RadFut ();
		QString RadPs ();
		virtual QString IndPres (int P);
		virtual QString IndImpf(int P);
		virtual QString IndPs (int P);
		QString SubjPres(int P);
		QString ImperPres (int P);
		QString PartPres();
		QString PP();
};

class TValler: public Verbe
{
	public:
		TValler (QString i): Verbe (i) {}
		virtual QString RadFut();
		virtual QString IndPres (int P);
		virtual QString SubjPres (int P);
		virtual QString ImperPres (int P);
};

class TVcueillir: public Verbe
{
	public:
		TVcueillir (QString i): Verbe (i) {}
		virtual QString RadFut();
		virtual QString PP();
};

class TVsst: public Verbe
{
	public:
		TVsst (QString i): Verbe (i) {}
		virtual QString RadPres(int P);
		virtual QString IndPres(int P);
		virtual QString IndPs(int P);
		virtual QString PP();
};

// héritiers de TVsst

class TVaitre: public TVsst		 // paraître
{
	protected:
		size_t pcirc;
	public:
		TVaitre (QString i);
		virtual QString RadPres(int P);
		QString RadPs();
		virtual QString IndPs(int P);
		QString PP();
};

class TVnaitre: public TVaitre
{
	public:
		TVnaitre (QString i): TVaitre (i) {modele="TVnaitre";}
		virtual QString RadPs();
		virtual QString IndPs(int P);
		virtual QString PP();
};

class TVpaitre: public TVaitre
{
	// distinguer repaître, qui a un pp.
	public:
		TVpaitre (QString i): TVaitre (i) {modele="TVpaitre";}
		virtual QString IndPs (int P);
		QString RadPs();
		QString PP();
};

class TVboire : public TVsst
{
	public:
		TVboire (QString i): TVsst(i) {modele="TVboire";}
		virtual QString RadPres(int P);
		virtual QString RadPs();
		virtual QString IndPs(int P);
		virtual QString SubjPres(int P);
		virtual QString PP();
};

class TVbouillir: public TVsst
{
	public:
		TVbouillir (QString i): TVsst(i) {}
		virtual QString RadPres(int P);
};

class TVcevoir: public TVsst
{
	public:
		TVcevoir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class  TVchoir: public TVsst
{
	public:
		TVchoir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPres (int P);
		virtual QString IndPs (int P);
		virtual QString SubjPres (int P);
		virtual QString PP() ;
};

class TVclore: public TVsst
{
	public:
		TVclore (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString IndPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVclure: public TVsst
{
	public:
		TVclure (QString i): TVsst(i) {}
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVconfire: public TVsst
{
	public:
		TVconfire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString PP();
};

class TVcourir: public TVsst
{
	public:
		TVcourir (QString i): TVsst(i) {modele="TVsst";}
		virtual QString RadFut();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVcroire: public TVsst
{
	public:
		TVcroire (QString i): TVsst(i) {modele="TVcroire";}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVcroitre: public TVsst
{
	public:
		TVcroitre (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVdevoir: public TVsst
{
	public:
		TVdevoir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVdire: public TVsst
{
	public:
		TVdire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPres (int P);
		virtual QString PP();
};

class TVdormir: public TVsst
{
	public:
		TVdormir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
};

class TVecrire: public TVsst
{
	public:
		TVecrire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString PP();
};

class TVfaillir: public TVsst
{
	public:
		TVfaillir (QString i): TVsst(i) {}
		virtual QString IndPres (int P);
};

class TVfaire: public TVsst
{
	public:
		TVfaire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		QString RadPs();
		virtual QString IndPres (int P);
		QString SubjPres (int P);
		QString PP();
};

class TVfuir: public TVsst
{
	public:
		TVfuir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
};

class TVgesir: public TVsst
{
	public:
		TVgesir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString IndFut (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVindre: public TVsst
{
	public:
		TVindre (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString PP();
};

class TVir: public TVsst
{
	public:
		TVir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVlire: public TVsst
{
	public:
		TVlire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVtaire: public TVlire
{
	public:
		TVtaire (QString i): TVlire (i) {}
		QString RadPs();
		virtual QString IndPs (int P);
};

class TVmettre: public TVsst
{
	public:
		TVmettre (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		QString RadPs();
		QString PP();
};

class TVmourir: public TVsst
{
	public:
		TVmourir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString IndPs (int P);
		QString PP();
};

class TVmouvoir: public TVsst
{
	public:
		TVmouvoir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		QString RadPs();
		virtual QString IndPs (int P);
		QString PP();
};

class TVouir: public TVsst
{
	public:
		TVouir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVplaire: public TVsst
{
	public:
		TVplaire (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVpleuvoir: public TVsst
{
	public:
		TVpleuvoir (QString i): TVsst (i) {}
		virtual QString IndPres (int P);
		virtual QString RadFut();
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString IndImpf  (int P);
		virtual QString SubjPres (int P);
		virtual QString PP();
		virtual QString conjugue(int P, int T, int M, int V, bool Pr);
};

class TVpouvoir: public TVsst
{
	public:
		TVpouvoir (QString i): TVsst (i) {}
		virtual QString RadFut();
		virtual QString RadPs();
		virtual QString IndPres (int P);
		virtual QString IndPs (int P);
		virtual QString SubjPres (int P);
		virtual QString PP();
};

class TVpourvoir: public TVsst
{
	public:
		TVpourvoir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVvoir: public TVsst
{
	public:
		TVvoir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString RadPs();
		QString PP();
};

class TVrire: public TVsst
{
	public:
		TVrire (QString i): TVsst (i) {}
		virtual QString RadPs();
};

class TVsavoir: public TVsst
{
	public:
		TVsavoir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		QString RadPs();
		virtual QString IndPs (int P);
		QString SubjPres (int P);
		QString ImperPres (int P);
		QString PartPres();
		QString PP();
};

class TVservir: public TVsst
{
	public:
		TVservir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
};

class TVquerir: public TVservir
{
    public:
        TVquerir (QString i): TVservir (i) {}
		virtual QString RadPres (int P);
		virtual QString PP();
		virtual QString RadPs();
		virtual QString IndPs (int P);
};

class TVsoudre: public TVsst
{
	public:
		TVsoudre (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVsuivre: public TVsst
{
	public:
		TVsuivre (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
};

class TVvivre: public TVsuivre
{
	public:
		TVvivre (QString i): TVsuivre (i) {}
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVsurseoir: public TVsst
{
	public:
		TVsurseoir (QString i): TVsst (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVvenir: public TVsst
{
	public:
		TVvenir (QString i): TVsst (i) {}
		virtual QString RadFut();
		virtual QString circonflexe();
		virtual QString RadPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

// ne pas permuter les 2 suivants
class TVvetir: public TVsst
{
	public:
		TVvetir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString PP();
};

class TVtir: public TVsst
{
	public:
		TVtir (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString IndPs (int P);
};
// ne pas permuter les 2 prcdents

class TVuire: public TVsst
{
	public:
		TVuire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString PP();
};

class TVtraire: public TVsst
{
	public:
		TVtraire (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs ();
		virtual QString PP();
};

class TVvaincre: public TVsst
{
	public:
		TVvaincre (QString i): TVsst(i) {}
		virtual QString RadPres (int P);
		virtual QString IndPres (int P);
		virtual QString RadPs();
		virtual QString PP();
};

// fin des hritiers de TVsst

class TVxxt: public TVsst
{
	public:
		TVxxt (QString i): TVsst(i) {}
		virtual QString RadPs();
		virtual QString IndPres (int P);
		virtual QString IndPs (int P);
};

//  hritiers de TVxxt

class TVvaloir: public TVxxt
{
	public:
		TVvaloir (QString i): TVxxt(i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString SubjPres(int P);
		virtual QString PP();
};

class TVfalloir: public TVvaloir
{
	public:
		TVfalloir (QString i): TVvaloir(i) {modele="TVfalloir";}
		virtual QString IndPres (int P);
		virtual QString IndFut(int P);
		virtual QString IndPs(int P);
		virtual QString SubjPres(int P);
		QString conjugue (int P, int T, int M, int V, bool pr);
};

class TVvouloir: public TVxxt
{
	public:
		TVvouloir (QString i): TVxxt(i) {}
		virtual QString RadPres (int P);
		virtual QString RadFut();
		virtual QString SubjPres (int P);
		virtual QString PP();
};

class TVdre: public Verbe
{
	public:
		TVdre (QString i): Verbe(i) {}
		virtual QString IndPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVasseoir: public TVdre
{
	public:
		TVasseoir (QString i): TVdre (i) {}
		virtual QString RadPres (int P);
		QString RadPs();
		virtual QString IndPres (int P);
		virtual QString IndImpf (int P);
		QString PP();
};

class TVcoudre: public TVdre
{
	public:
		TVcoudre (QString i): TVdre (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
};

class TVmoudre: public TVdre
{
	public:
		TVmoudre (QString i): TVdre (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString IndPs (int P);
};

class TVprendre: public TVdre
{
	public:
		TVprendre (QString i): TVdre (i) {}
		virtual QString RadPres (int P);
		virtual QString RadPs();
		virtual QString SubjPres (int P);
		virtual QString PP();
};

class TVouvrir: public Verbe
{
	public:
		TVouvrir (QString i): Verbe (i) {}
		virtual QString ImperPres (int P);
		virtual QString IndPs (int P);
		virtual QString PP();
};

class TVbattre: public TVtir
{
	public:
		TVbattre (QString i): TVtir (i) {}
		virtual QString RadPres (int P);
		QString PP();
};

bool IsLast(QString chaine, QString mot);
Verbe * verbe_m (QString inf);
QString conjugue(QString inf, int P=1, int T=1, int M=1, int V=1, bool Pr=false, int g=0, int n=0);
QString conjnat(QString inf, QString morpho, bool pr=false);

//QString tableau (QString verbe, int voix);

// ------------------------------------------------------------
//  Flexion des noms
// ------------------------------------------------------------

class  Nom
{
	protected:
		QString sing;
		QString modele;
	public:
		Nom (QString s);
		virtual ~Nom ();
		virtual QString getModele ();
		virtual QString pluriel();
};

class NomSXZ: public Nom
{
	public:
		NomSXZ (QString s): Nom (s) {modele="NomSXZ";}
		QString pluriel();
};

class NomAL: public Nom
{
	public:
		NomAL (QString s): Nom (s) {}
		virtual QString pluriel();
};

class NomAIL: public Nom
{
	public:
		NomAIL (QString s): Nom (s) {modele="NomAI";}
		virtual QString pluriel();
};

class NomAUEU: public Nom
{
	public:
		NomAUEU (QString s): Nom (s) {}
		virtual QString pluriel();
};

Nom * nom_m (QString n);

// ------------------------------------------------------------
//  Flexion des adjectifs
// ------------------------------------------------------------

class Adjectif
{
	protected:
		QString graphie;
		QString modele;
	public:
		Adjectif (QString a);
		virtual ~Adjectif ();
		virtual QString getModele ();
		virtual QString feminin();
		virtual QString pluriel (bool fem);
		virtual QString accorde(int g, int n);
};

// irrgularits :
// gu - gu

class Aigu: public Adjectif
{
	public:
		Aigu (QString a): Adjectif (a) {modele="Aigu";}
		virtual QString feminin();
};

class ElEil: public Adjectif
{
	public:
		ElEil (QString a): Adjectif (a) {modele="ElEi";}
		virtual QString feminin();
};

class Al: public Adjectif
{
	public:
		Al (QString a): Adjectif (a) {modele="A";}
		virtual QString pluriel (bool fem);
		virtual QString accorde(int g, int n);
};

class Gras: public Adjectif
{
	public:
		Gras (QString a): Adjectif (a) {modele="Gras";}
		virtual QString feminin ();
};

class Eux: public Adjectif
{
	public:
		Eux (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class El: public Adjectif
{
	public:
		El (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class Er: public Adjectif
{
	public:
		Er (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class AdjF: public Adjectif
{
	public:
		AdjF (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class AdjC: public Adjectif
{
	public:
		AdjC (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class Eau: public Adjectif
{
	public:
		Eau (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class Et: public Adjectif
{
	public:
		Et (QString a): Adjectif (a) {modele="Et";}
		virtual QString feminin();
};

class Mon: public Adjectif
{
	public:
		Mon (QString a): Adjectif (a) {}
		virtual QString feminin();
		virtual QString pluriel(bool fem);
};

class Bon: public Adjectif
{
	public:
		Bon (QString a): Adjectif (a) {}
		virtual QString feminin();
};

class Tout: public Adjectif
{
	public:
		Tout (QString a): Adjectif (a) {}
		virtual QString pluriel (bool fem);
};

bool pas_de_passif (QString inf);
QString tableau (QString verbe, int voix);
QString Pluriel(QString n);
QString Accorde(QString adj, int g, int n);
#endif
