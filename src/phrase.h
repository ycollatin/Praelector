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
        QStringList             entreMots; 
	    QString                 _chAdditions;
        Dialogue*               _dialogue;
        QStringList             _feminins;
        QString                 _gr;
        QMap<QString,Handicap*> _handicaps;
        int                     _imot;
        Lemmat*                 _lemmatiseur;
        QList<Requete*>         _listeR;
        int                     _maxImot;
        QList<Mot*>             _mots;
        int                     _num;
        QList<Regle*>           _regles;
        QString                 _reponse;
        QList<Requete*>         _requetes;

        QString                 saisie (QString l, QString s);
        void                    majAffichage();

    public:
        Phrase(QString t);
        bool static       accord(MotFlechi* ma, MotFlechi* mb, QString cgn);
        void              additions();
        void              ajListeR(Requete* req);
        void              ajRequete(Requete* req);
        QString           arbre(QString format="dot", bool trace=false);
        bool              boucle(Requete* req);
        void              choixFlechi(MotFlechi* mf);
        bool              compatible(MotFlechi* mf, Mot* m);
        bool              compatible(MotFlechi* ma, MotFlechi* mb);
        void              conflit(Requete* ra, Requete* rb, QString cause);
        bool              contigue(Requete* req);
        bool              contigus(Mot *a, Mot *b);
        QString           droite(Mot* m);
        void              ecoute (QString m);
        bool              estFeminin(QString n);
        bool              filtre(Requete* req);
        QString           gauche(Mot *m);
        QString           grLu();
        int               handicap(MotFlechi* mf);
        QList<Requete*>   homolexes(Requete* req);
        QList<Requete*>   homonymes(Requete* req);
        QList<Requete*>   homonymes(QString id);
        QString           htmlLiens();
        void              initFeminins ();
        bool              isomorph(QString ma, QString mb);
        void              lance();
        void              lemmatise();
        Requete*          montante(Mot* m);
        Mot*              motCourant();
        Mot*              motNo(int i);
        int               nbMots();
        int               nbListeR();
        int               nbPonct(QChar p, Mot* ma, Mot* mb);
        int               nbRegles();
        int               nbRequetes();
        void              nettoieHomonymes(QString id);
        int               num();
        void              peupleHandicap();
        void              peupleRegles(QString nf);
        QList<Mot*>       portee(int a, int b);
        Regle*            regle(int i);
        Regle*            regle(QString id);
        //void              relecture();
        QList<Requete*>   reqCC(Mot* m);
        Requete*          requete(int i);
        void              rmListeR(Requete* req);
        void              setGr(QString t);
        void              setLiens();
        void              setNum(int n);
        QList<Mot*>       supersDe(Mot* m);
        QString           tr();
    signals:
        void              repondu(const QString);
};

namespace Chaines {

	const QString affichage = QString::fromUtf8
		( "%1<hr/>%2"
		 "<hr/><strong>Morphologies et traductions du mot</strong><br/>%3"
		 "<hr/><a href=\"-prec\">reculer</a> <a href=\"-suiv\">avancer</a> d'un mot "
         "&nbsp;<a href=\"-quitter\">quitter</a>"
		 "<hr/><strong>Nouveaux liens syntaxiques</strong><br/>%4"
		 "<hr/><strong>&Eacute;tat de la traduction</strong><br/>%5"
		 "<hr/><a href=\"-nouvPhr\">Saisir une phrase</a> "
		 "<a href=\"-corpus\">choisir une phrase</a>&nbsp;<a href=\"-quitter\">quitter</a>");

	const QString affSubAPrendre = QString::fromUtf8
		("%1 <span style=\"color:blue;font-style:italic\">%2</span> "
		 "<a href=\"l.v.%3\">valider</a> <a href=\"l.r.%3\">rejeter</a>");

	const QString affSubValide = QString::fromUtf8
		("%1 <span style=\"color:green;font-style:italic;font-weight:bold\">%2</span> "
		 "<a href=\"l.g.%3\">&larr;</a><a href=\"l.p.%3\">&#x21c6;</a><a href=\"l.d.%3\">&rarr;</a>"
		 "<a href=\"l.r.%3\">rejeter</a>");

	const QString documentation = QString::fromUtf8
		(
		 "<a href=\"-init\">Menu</a><br/>"
		 "<strong>Saisie de la phrase</strong><br/>"
		 "Un clic sur le lien <em>Saisir une phrase</em> "
		 "permet non seulement de saisir, mais aussi de coller "
		 "la phrase qu'on veut lire. Pour valider, la touche "
		 "<em>Entrée</em> est l'équivalent d'un clic sur le bouton OK."
		 "De même, la touche <em>Échapp</em> est l'équivalent d'un "
		 "clic sur le bouton Cancel.<br/>"
		 "<strong>Morphologie et sémantique</strong><br/>"
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
		 "<strong>Liens syntaxiques</strong><br/>"
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

	const QString initAff = QString::fromUtf8
		("%1<hr/>"
		 "est une aide à la lecture de la phrase latine. "
		 "<br/><a href=\"-nouvPhr\">Saisir une phrase</a>  "
		 "<a href=\"-corpus\">choisir une phrase</a><br/>"
		 "<a href=\"-aide\">Documentation</a> "
		 "<a href=\"-quitter\">Quitter</a>");

	const QString titrePraelector = 
		"<h2 style=\"font-family:serif;letter-spacing:2em\"><center>&#x2619; PRAELECTOR &#x2767;</center></h2>";

	const QString affSubGris = QString::fromUtf8
		("%1 <span style=\"color:grey;font-style:italic\">%2</span> "
		 "<a href=\"l.v.%3\">valider</a>");

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
