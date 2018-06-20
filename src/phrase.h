/* phrase.h */
/* Copyright (C) 2015 Yves Ouvrard

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PHRASE_H
#define PHRASE_H

#include <QList>
#include <QObject>
#include <QString>

#include <dialogue.h>
#include <handicap.h>
#include <motflechi.h>
#include <regle.h>
#include <requete.h>

class Mot;

class Phrase: public QObject
{

    Q_OBJECT

    private:
        QStringList                entreMots;
	    QString                    _chAdditions;
        QStringList                _feminins;
        QString                    _gr;
        QMap<QString,Handicap*>    _handicaps;
        int                        _imot;
        Lemmat*                    _lemmatiseur;
        QMap<QString, QStringList> _mapLgr;
        QList<Mot*>                _mots;
        QString                    _num;
        int                        _numReq;
        QString                    _prae;
        QList<Regle*>              _regles;
        QStringList                _relect;
        QString                    _reponse;
        QList<Requete*>            _requetes;
        QString                    _tr;

        bool                    enr();
        bool                    estFeminin(QString n);
        void                    initFeminins ();
        void                    initLgr();
        void                    majAffichage();
        bool static             sortR(Requete* ra, Requete* rb);

    public:
        Phrase(QString t);
        bool static       accord(MotFlechi* ma, MotFlechi* mb, QString cgn);
        void              additions();
        void              ajRequete(Requete* req, bool force=false);
        void              annuleLemme(Mot* m, Lemme* l);
        void              choixFlechi(MotFlechi* mf);
        void              choixReq(Requete* req);
        bool              compatible(MotFlechi* mf, Mot* m);
        bool              compatible(MotFlechi* ma, MotFlechi* mb);
        bool              contigue(Requete* req);
        bool              contigus(Mot *a, Mot *b);
        QString           droite(Mot* m);
        void              ecoute (QString m);
        bool              estVbRelative(MotFlechi* mf);
        bool              filtre(Requete* req);
        QString           gauche(Mot *m);
        QString           gr();
        QString           grLu();
        int               handicap(MotFlechi* mf);
        QList<Requete*>   homolexes(Requete* req);
        QList<Requete*>   homonymes(Requete* req);
        QList<Requete*>   homonymes(QString id);
        QString           htmlLiens();
        QStringList       lgr(char pos);
        bool              isomorph(QString ma, QString mb);
        void              lemmatise();
        QList<Requete*>   lReqSub(MotFlechi* mf, bool closes=false);
        QList<Requete*>   lReqSup(MotFlechi* mf, bool closes=false);
        Requete*          montante(Mot* m);
        Mot*              motCourant();
        Mot*              motNo(int i);
        int               nbMots();
        int               nbPonct(QChar p, Mot* ma, Mot* mb);
        int               nbRegles();
        int               nbRequetes();
        int               nbSuper(MotFlechi* mf);
        QString           num();
        int               numNouvReq();
        void              nettoieHomonymes(QString id);
        void              peupleHandicap();
        void              peupleRegles(QString nf);
        QList<Mot*>       portee(int a, int b);
        Regle*            regle(int i);
        Regle*            regle(QString id);
        void              reinit();
        QList<Requete*>   reqCC(Mot* m);
        Requete*          requete(int n);
        QString static    saisie (QString l, QString s);
        void              setGr(QString t);
        void              setLiens();
        QList<Mot*>       supersDe(Mot* m);
        QString           tr();
        MotFlechi*        vbRelative(MotFlechi* mf);
        MotFlechi*        vbSuper(MotFlechi* mf);
    signals:
        void              repondu(const QString);
    public slots:
        void              traceReq();
};

namespace Chaines {

    const QString menu = QString::fromUtf8
	    ("<hr/>"
         "<a href=\"-corpus\">choisir une phrase</a><br/>"
		 "<a href=\"-enr\">choisir une phrase enregistrée</a><br/>"
		 "<a href=\"-nouvPhr\">Saisir une phrase</a><br/>"
         "<a href=\"-clav\">clavier</a><br/>"
         "<a href=\"-init\">annuler</a> <a href=\"-quitter\">quitter</a><br/>");

	const QString affichage = QString::fromUtf8
        ("%1%2<br/>"
		 "<hr/><a href=\"-reinit\">réinitialiser</a>&nbsp;"
         "<a href=\"-prec\">reculer</a> <a href=\"-suiv\">avancer</a><hr/>"
         "%3"
		 "<hr/><strong>Morphologies et traductions du mot</strong><br/>\n%4"
         "<a href=\"-trace\">enregistrer</a>&nbsp;"
         "<a href=\"-quitter\">quitter</a>"
		 "<hr/><strong>Liens syntaxiques</strong><br/>%5\n"
		 "<hr/><strong>&Eacute;tat de la traduction</strong><br/>\n%6"
         "<hr/>%7");

	const QString documentation = QString::fromUtf8
		(
		 "<a href=\"-init\">Menu</a><br/>\n"
		 "<strong>Saisie de la phrase</strong><br/>\n"
		 "Un clic sur le lien <em>Saisir une phrase</em> "
		 "permet non seulement de saisir, mais aussi de coller "
		 "la phrase qu'on veut lire. Pour valider, la touche "
		 "<em>Entrée</em> est l'équivalent d'un clic sur le bouton OK."
		 "De même, la touche <em>Échapp</em> est l'équivalent d'un "
		 "clic sur le bouton Cancel.<br/>\n"
		 "<strong>Pilotage</strong><br/>\n"
         "On peut piloter Praelector à la souris, mais en saisissant "
         "le caractère /, L'interface affiche en rouge des caractères "
         "qui, saisis au clavier, activent le lien qui les suit.<br/>"
		 "<strong>Morphologie et sémantique</strong><br/>\n"
		 "Les liens <em>Reculer</em> et <em>Avancer</em> permettent "
		 "de se déplacer dans la phrase. Il est conseillé de faire "
		 "le plus de choix possibles avant de passer au mot suivant."
		 "On peut se déplacer de plusieurs mots en utilisant les "
		 "caractères de soulignement qui précèdent chaque mot déjà "
		 "lu. Parmi les choix, <ul>"
		 "<li> choix du déterminant ou du sujet ;"
		 "<li> choix de la traduction ; "
		 "<li> Rejet de la morphologie proposéë, ou validation "
		 "comme seule morphologie possible.</ul>"
		 "<strong>Liens syntaxiques</strong><br/>\n"
		 "À partir du deuxième mot de la phrase, tous les "
		 "liens syntaxiques possibles sont proposés. On peut "
		 "rejeter ceux qui paraissent impossibles ou incorrects, "
		 "et valider ceux qu'on accepte. Souvent, plusieurs "
		 "liens concurrents sont possibles. Pour choisir, il "
		 "faut examiner avec soin la fonction des liens proposés, plus "
		 "que leur traduction, qui est souvent approximative. Des liens "
		 "permettent de déplacer, d'ajouter et de supprimer des éléments "
		 "de ces traductions. L'état de la traduction est affiché au "
		 "bas de la fenêtre. Bon courage !");

         /*
         // test
         "<br/><select>"
         "<option>un</option>"
         "<option>deux</option>"
         "<option>trois</option>"
         "</select>"
         */

	const QString titrePraelector =
		"<h2 style=\"font-family:serif;\"><center>&#x2619; PRAELECTOR &#x2767;</center></h2>";

	const QString affInit = QString::fromUtf8
		("Saisis ci-dessous la phrase à lire");

	const QString ajTrEntree = QString::fromUtf8
		("Pour ajouter une entrée :\n "
		 "lemme;modèle;r1;r2;indMorph:tr1(comm.)[comm.],tr2;tr3\n"
		 "Pas d'espace avant les deux points ; le modèle peut être "
		 "donné par un numéro, ou par un mot du même modèle.\n");

	/* déterminants */
	const QStringList detMS = QStringList () <<"le" << "un";
	const QStringList detFS = QStringList () <<"la" << "une";
	const QStringList detP = QStringList () <<"les" << "des";
	
}


#endif
