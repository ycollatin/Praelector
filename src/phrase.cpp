/* phrase.cpp */

/* Copyright (C) 2018 Yves Ouvrard

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// textes :
// ~/prof/latin/txt/catalogue.txt
// bin/data/regles.la
// bin/corpus/phrases.txt

//                          FIXME
//        - motflechi.cpp : elidefr(), contraction à le : exclure "là le".
//        - L'ablatif absolu postposé n'est pas proposé, même après validation. epitheteV ?
//        - /ira furor breuis/ : Il semble que la relecture, en cas de choix autre que celui qui
//          est surligné, se met à surligner plusieurs autres choix.
//        - risisse est traduit par un indicatif passé composé : confusion de la morpho latine
//          et de son équivalent calculé pour le français.
//        - Requêtes doublonnées, pê après un retour arrière.
//        - Échec pour les phrases longues : Socrates quidem dicebat multos homines, etc.
//
//                           TODO
//        - Revoir l'algo de mise à la forme négative.
//        - remettre à l'indicatif ou au subj. la tr. du verbe de la proposition infinitive
//        - Trouver une règle pour tum ... cum.
//        - Lecture enregistrée : il faudrait que l'auteur de l'enregistrement puisse expliquer ses choix.
//        - Ajouter un conseil : supprimer toutes les requêtes impossibles
//        - L'utilisateur devrait pouvoir créer un lien sans que l'appli l'ait proposé, et lui donner le nom qu'il veut.
//        - Possibilité de changer la place du groupe subordonné courant.
//        - Permettre le chargement d'un texte sans retour à la ligne entre chaque phrase.
//        - vérifier la mise à jour des requêtes proposées après validation
//          de l'une d'entre elles.
//        - l'application du champ tr des règles est complexe et mal contrôlé.
//        - Possibilité de forcer un lien syntaxique entre deux mots ?
//        - Lien phrase suivante.
//        - faire disparaître les hyperliens inutiles.
//        - Liens à ajouter :
//          * voir la traduction enregistrée
//        - Trace :
//          * Mentionner l'auteur de la trace ;
//          * Comparaision de traduction en fin de lecture
//        - Comment lier ?
//              . un pronom sujet non exprimé peut avoir une apposition :
//                ibam forte uia sacra nescio quid meditans....
//              . un groupe elliptique dont le noyau est absent : homo homini lupus.
//        - Lexique personnel
//        - accorder la traduction de l'épithète : post equitem sedet *atra* cura :
//            si /cura/ est traduit par le masculin /souci/, /atra/ est traduit par "noir".
//
//                           XXX GRAMMAIRE
//        - règle nisi ubi : sauf quand ; nisi + nom.
//        - iussitque ut in : in prep iussit proposée : activer blocage ?
//        - 27 nullus dolor est quam : nombreuses erreurs.
//          . quem est traduit deux fois ;
//          . le déterminant fr s'accorde avec le masculin du latin dolor
//          . la relative ne s'accroche pas à l'antécédent
//          . minuat ac : la coord1 n'est pas traduite
//        - quid ergo ; opus est : règles particulières en include
//        - opus est + datif : règle à trois pattes.
//        - quid dulcius : quoi de plus doux ? Pas de règle.
//        - phr interrogatives : inversion du sujet et adv interrogatif en première place.
//        - vel dii : l'adv. porte sur le nom.
//        - sic coord ? traiter sic ut.
//        - une étiquette dans lexsynt.la pour les verbes à ppp substantivables (acta agis) ?
//          par ex. subst
//        - En vert : fonction des liens proposés
//

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMultiMap>
#include <QSettings>
#include <QStringList>
#include <iostream>
#include <QDebug>

#include <ch.h>
#include <dialogue.h>
#include <lemmatiseur.h>
#include <phrase.h>

Phrase::Phrase(QString t)
{
    _lemmatiseur = new Lemmat(this);
    initFeminins();
    initLgr();
    peupleRegles("regles.la");
    peupleHandicap();
    _numReq = -1;
    if (!t.isEmpty())
    {
        setGr(t);
        lemmatise();
    }
}

bool Phrase::accord(MotFlechi* ma, MotFlechi* mb, QString cgn)
{
    QString cma = ma->morpho();
    QString cmb = mb->morpho();
    // ajouter une personne si elle est requise par cgn
    if (cgn.contains('p'))
    {
        // trouver le non-verbe
        if (!QString("vw").contains(ma->pos()))
        {
            if (ma->gr() == "ego") cma.append(" 1ère");
            else if (ma->gr() == "tu") cma.append(" 2ème");
            else cma.append(" 3ème");
        }
        else
        {
            if (mb->gr() == "ego") cmb.append(" 1ère");
            else if (mb->gr() == "tu") cmb.append(" 2ème");
            else cmb.append(" 3ème");
        }

    }

    QString lch = "cgnpm";
    for (int ich=0;ich<cgn.size();++ich)
    {
        QChar ch=cgn.at(ich);
        int p = lch.indexOf(ch);
        QStringList liste;
        switch(p)
        {
            case 0: liste = Ch::cas;       break;
            case 1: liste = Ch::genres;    break;
            case 2: liste = Ch::nombres;   break;
            case 3: liste = Ch::personnes; break;
            case 4: liste = Ch::modes;     break;
            default: break;
        }
        for (int i=0;i<liste.count();++i)
        {
            QString trait = liste.at(i);
            bool mac = cma.contains(trait);
            bool mbc = cmb.contains(trait);
            if (mac && !mbc) return false;
            if (mbc && !mac) return false;
        }
    }
    return true;
}

/*
	L'utilisateur peut ajouter des entrées
	dans un lexique personnel. Ces additions
	sont lues à l'initialisation de Praelector.
 */
void Phrase::additions()
{
    // TODO à implémenter
	QSettings settings ("Collatinus", "praelector");
	_chAdditions = settings.fileName();
	_chAdditions = QFileInfo(_chAdditions).absolutePath ();
	if (QDir(_chAdditions).exists (_chAdditions+"/lemmata.la"))
	{
		_lemmatiseur->lisFichierLexique(_chAdditions+"/lemmata.la");
		QFile flang (_chAdditions+"/lemmata.fr");
		flang.open (QIODevice::ReadOnly|QIODevice::Text);
        QTextStream flux (&flang);
		QString linea;
		while (!flux.atEnd ())
		{
			QString lin = flux.readLine ();
        	if (lin.isEmpty () || linea[0] == '!') continue;
        	QStringList ecl = lin.split ('|');
			if (ecl.count () < 2)
        	{
				std::cerr << "ligne mal formée" << qPrintable (linea)<<"\n";
            	continue;
        	}
        	Lemme* l = _lemmatiseur->lemme(Ch::jviu (ecl[0]));
        	if (l!=NULL)
            	l->ajTrad ("fr", ecl[1]);
		}
	}
}

void Phrase::ajRequete(Requete* req, bool force)
{
    // vérifier qu'une requête homonyme n'existe pas déjà
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* r = _requetes.at(i);
        if (!force && req->egale(r))
        {
            //std::cerr << qPrintable("\nRequête "+req->doc()+"\négale à la requête\n"+r->numc());
            return;
        }
    }
    _requetes.append(req);
}

void Phrase::annuleLemme(Mot* m, Lemme* l)
{
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req == 0 || req->rejetee()) continue;
        if (req->subRequis())
        {
            if (req->sub() != 0
                && req->sub()->mot() == m && req->sub()->lemme() == l)
            {
                if (req->close()) req->annuleRequis("lemme rejeté");
            }
            else if (req->super()->mot() == m && req->super()->lemme() == l)
            {
                req->setRejetee(true, "lemme rejeté");
            }
        }
        else
        {
            if (req->super() != 0
                && req->super()->mot() == m && req->super()->lemme() == l)
            {
                if (req->close()) req->annuleRequis("lemme rejeté");
            }
            else if (req->sub()->mot() == m && req->sub()->lemme() == l)
            {
                req->setRejetee(true, "lemme rejeté");
            }
        }
    }
    m->annuleLemme(l);
}

void Phrase::choixFlechi(MotFlechi* mf)
{
    Mot* m = mf->mot();
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req == 0 || req->rejetee()) continue;
        MotFlechi* msup = req->super();
        MotFlechi* msub = req->sub();
        if ((msup != 0 && msup->mot() == m && msup != mf)
            || (msub != 0 && msub->mot() == m && msub != mf))
            req->setRejetee(true, "choix du fléchi "+mf->morpho());
    }
    m->choixFlechi(mf);
}

/* Validation de la requête req, élimination des concurrentes */
void Phrase::choixReq(Requete* req)
{
    if (req == 0 || !req->close()) return;
    req->setValide(true);
    Mot* msub = req->sub()->mot();
    MotFlechi* mfsup = req->super();
    // tri des fléchis
    choixFlechi(req->super());
    choixFlechi(req->sub());

    // élimination des requêtes obsolètes
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* r = _requetes.at(i);
        if (r == 0 || req->rejetee() || r == req || r->valide())
        {
            continue;
        }
        // requêtes non closes, mais en contradiction avec req
        if (r->superRequis() && r->sub() == req->sub() && r->sub()->lemme()->cle() != "qui2")
           //|| (r->subRequis() && r->super()->mot() == req->super()->mot() && (!req->multi())))
        {
            r->setRejetee(true, "requête incompatible choisie "+req->doc());
        }
        if (!r->close()) continue;
            // même super, même aff, un seul sub permis sauf règles multi
        if ((r->super()->mot() == mfsup->mot() && r->aff() == req->aff() && !req->multi())
            // même super, même sub, id différents
            ||(r->sub() == req->sub() && r->super() == mfsup && r->id() != req->id())
            // même sub, requête non validée, exc. antécédent
            ||(r->sub()->mot() == msub && !r->valide() && !req->sub()->mot()->estRelatif()))
        {
            r->setRejetee(true, "req. incompatible choisie");
        }
    }
}

// Le MotFlechi mf est-il compatible avec toute requête close
// où figure m ?
bool Phrase::compatible(MotFlechi* mf, Mot* m)
{
    if (!m->estLie()) return false;
    for (int i=0;i<m->nbFlechis();++i)
    {
        if (compatible(mf, m->flechi(i))) return true;
    }
    return false;
}

bool Phrase::compatible(MotFlechi* ma, MotFlechi* mb)
{
    return ma->lemme() == mb->lemme() && isomorph(ma->morpho(), mb->morpho());
}

bool Phrase::contigue(Requete* req)
{
    return (req->close() && contigus(req->super()->mot(), req->sub()->mot()));
}

bool Phrase::contigus(Mot *a, Mot *b)
{
    return abs(a->rang() - b->rang()) == 1;
}

QString Phrase::droite(Mot *m)
{
    if (m == 0) return "";
    if (m->rang() >= entreMots.count())
    {
        return "";
    }
    else if (m->rang()+1 < entreMots.count())
    {
        return entreMots.at(m->rang() + 1);
    }
    else return "";
}

/*
 * Phrase::ecoute (QString m)
 *  Quelques mots clés :
 *  - suiv, prec : progression ou régression d'un mot
 *  - nouvPhr : saisie d'une nouvelle phrase
 *  sinon, m est une Url en sections séparées par des points
 *   1 == mm = mot
 *     si 1 == 'm' :
 *      2. c = choisir et éliminer les concurrents,
 *        d = fixer le déterminant
 *        e = éditer
 *        i = rotation de la traduction du fléchi
 *        r = rejeter
 *     3 : numéro de la morpho sur laquelle agir.
 *  si 1 == 'l' lien
 *     2. c = choisir ce lien et éliminer les concurrents
 *        d = doc
 *        e = éditer la traduction de ce lien
 *        r = rejeter ce lien
 *        t = traduction suivante du lien
 *     3. entre virgules, toutes les morphos du mot courant
 *        affectées par le lien
 */

void Phrase::ecoute (QString m)
{
    _reponse.clear();
	if (m.isEmpty() || m == "-init")
	{
		majAffichage ();
		emit (repondu(_reponse));
		return;
	}
    /* Réinitialisation à partir du mot courant */
    else if (m == "-reinit")
    {
        reinit();
    }
	/* deux commandes de navigation */
    // 1. Avancer
	else if (m == "-suiv" && _imot < _mots.count()-1)
	{
        // tuer les requêtes clonées
        for (int i=0;i<_requetes.count();++i)
        {
            Requete* req = _requetes.at(i);
            if (!req->rejetee() && !req->multi() && req->clonee() && req->origine()->valide())
            {
                req->setRejetee(true, "clone obsolète");
            }
        }
        // passer au mot suivant
        ++_imot;
        // résolution des requêtes
        if (_imot > 0 && !motCourant()->reqLancees()) setLiens();
        // lancement de nouvelles requêtes
        if (_imot < _mots.count() - 1) motCourant()->lance();
    }
    // 2. Reculer
	else if (m == "-prec" && _imot > 0)
	{
		--_imot;
	}
	/* Nouvelle phrase saisie */
	else if (m == "-nouvPhr")
	{
        QString p = saisie("Saisir ou coller une phrase", "");
        if (!p.isEmpty())
        {
            setGr(p);
            lemmatise();
		    _imot = 0;
            motCourant()->lance();
        }
	}
	/* Nouvelle phrase choisie */
	else if (m.startsWith("-phr"))
	{
        QStringList ecl = m.split('_');
        // 0 : numéro de la phrase, en string
        _num = ecl.at(0);
        _num.remove(0,4);
        // 1 : la phrase
        setGr(ecl.at(1));
        lemmatise();
		_imot = 0;
        motCourant()->lance();
	}
	else // mots et liens
	{
		Mot *cour = motCourant();
		QStringList eclats = m.split ('.');
		QChar ml = eclats.at(0).at(0);
		switch (ml.unicode())
		{
			case 'i':  // retour rapide vers le mot de rang m[1]
                {
                    _imot = eclats.at(1).toInt();
                    break;
                }
			    /* PREMIÈRE SECTION : MORPHOS DU NOUVEAU MOT*/
			case 'm':  // m[0] : opération sur les morphos.
			    {
				    /* Calculer le numéro de la morpho sur laquelle agir */
				    int num = eclats.at(2).toInt ();
                    MotFlechi* mf = cour->flechi(num);
					switch (eclats.at (1).at(0).unicode())
					{
						case 'a':  // ajout d'une entrée dans les additions
							{
                                // TODO à implémenter
								QString nm = saisie(Chaines::ajTrEntree, "");
								if (nm.isEmpty()) break;
								QStringList eclats = nm.split (':');
								QString la = eclats.at (0);
								QString tr = eclats.at (1);
								QStringList eclLa = la.split (';');
								// TODO contrôle de saisie
								// dans le cas d'un modèle alphanumérique, chercher le modèle numérique
								QString modele = eclLa.at (1);
								la = eclLa.join ('|');
								QFile fla (_chAdditions+"/lemmata.la");
								fla.open (QIODevice::WriteOnly|QIODevice::Append);
								QTextStream (&fla) << la << "\n";
								fla.close ();
								QFile ffr (_chAdditions+"/lemmata.fr");
								ffr.open (QIODevice::WriteOnly|QIODevice::Append);
								QString lemme = Ch::ote_diacritiques (la.section('|',0,0));
								QString lat = lemme.section ('=',0,0);
								QTextStream (&ffr) <<lat << "|" << tr <<"\n";
								ffr.close ();
								additions ();
								QString grmot = motCourant()->gr();
								// rafraîchir le mot
								Mot *motA = new Mot(grmot, 0, 0, _imot, this);
								delete _mots.at(_imot);
                                _mots.removeAt(_imot);
                                _mots.insert(_imot, motA);
								break;
							}
						case 'c': // éliminer tous les autres fléchis
                            {
                                choixFlechi(mf);
							    break;
                            }
						case 'd': // rotation du déterminant
							{
								mf->setDet(estFeminin(mf->trNue()));
								break;
							}
						case 'e':  // éditer de la traduction
							{
                                if (eclats.count() > 2 && !eclats.at(2).isEmpty())
                                {
                                    QString v = mf->tr();
                                    if (eclats.count() > 3)
                                        v = eclats.at(3);
                                    QString t = saisie("traduction pour "+mf->gr(), v);
                                    if (! t.isEmpty())
                                    {
                                        mf->ajTrfl(t);
                                        emit(editTr(t));
                                    }
                                }
								break;
							}
                        case 'i': // traduction du fléchi
                            {
                                mf->setItr(eclats.at(3).toInt());
                                break;
                            }
                        case 't':
                            {
								QMessageBox mb;
								mb.setText(cour->flechi(num)->trs());
								mb.exec();
                                break;
                            }
						case 'r': // rejeter m.r.m = le lemme ; m.r.f la forme
                            if (eclats.count() > 3)
                            {
                                int n = eclats.at(3).toInt();
                                switch (eclats.at(2).at(0).unicode())
                                {
                                    case 'm':
                                        {
                                            Lemme* l = cour->flechi(n)->lemme();
                                            annuleLemme(cour, l);
                                            break;
                                        }
                                    case 'f':
                                        {
                                            cour->annuleFlechi(n);
                                            break;
                                        }
                                    default:
                                        break;
                                }
                            }
							break;
						case 's': // rotation du sujet
							{
								MotFlechi *mf = cour->flechi(num);
								mf->setSujet();
								break;
							}
						case 'v':
							if (eclats.count() > 3)
							{
								// l'une des traductions proposée a été cliquée
								//cour->setTradLemPostag (num, eclats.at (3));
							}
							//else cour->valideTr (num);
							break;
						default: std::cerr << qPrintable (m)
								 << " erreur d'url morpho"<<"\n"; break;
					}
					break;
				}
				/* SECONDE SECTION : LIENS SYNTAXIQUES */
 				/*     0. == 'l' : opération sur un lien
				 *     1. s : lien dont le mot courant est père
				 *        d : lien dont le mot courant est fils
				 *        i : demande de doc sur le lien
				 *        p : permutation fils-père dans l'ordre de la traduction : homme grand / grand homme
                 *        t : traduction suivante du lien
 	     		 *     2. d = déplacer le fils de ce lien vers la g ou la d
 	     		 *        e = éditer la traduction de ce lien
 	     		 *        r = rejeter ce lien
				 *        x,y,z = éditer les ajouts français avant, entre, après F et P, ou P et F.
				 *
 	     		 */
			case 'l':      // l == lien
				{
					QStringList lv;
					if (eclats.count() > 3)
						lv = eclats.at (3).split (',');
                    // liste des morphos affectées passées par m[3]
                    QList<Requete*> lreq;
					char gd = eclats.at(1).at(0).unicode();   // m[2] : opération à réaliser
                    int rang = eclats.at(2).toInt();
                    Requete* req = requete(rang);
					switch (gd)      // pos 1, idgp info et déplacement
					{
						case 'd':    // déplacement d'un groupe à droite
						case 'g':    // et à gauche
						case 'i':    // info sur le lien
						case 'p':    // changement de position dans la traduction
							{
                                /*
								   char d = eclats.at (2).at(0).unicode();
								//bool droite = gd == 'd';
								// définition de sub en utilisant pos 2.
								switch (d)
								{
								case 's': // le lien est un sub
								{
								for (int i=0;i<cour->subordonnes().count();++i)
								if (lv.contains (QString::number (i)))
								lreq.append (cour->subordonnes().at (i));
								break;
								}
								case 'd': // le lien est un dep
								{
								for (int i=0;i<cour->dependDe().count();++i)
								if (lv.contains (QString::number (i)))
								lreq.append (cour->dependDe().at (i));
								break;
								}
								default:break;
								}
								if (lreq.empty())
								break;
                                 */
								switch (gd)
								{
									case 'i':  // demande de doc sur le lien
										{
                                            Requete* req = _requetes.at(eclats.at(2).toInt());
											QMessageBox mb;
											mb.setText(req->regle()->doc());
											mb.exec();
											break;
										}
									case 'p':  // permutation P F
										{
                                            /*
											   foreach (LienSub *s, lreq)
											   s->swapG ();
                                             */
											break;
										}
									default:  // déplacement du groupe à g ou à d
										// foreach (LienSub *s, lreq) s->super()->deplaceFilsFr(s, droite);
										break;
								}
								break;
							}
                        case 't': // rotation de la traduction du lien
                            {
                                req->incItr();
                                break;
                            }
						case 'v':   // pos 1, valider
							{
                                if (req == 0)
                                {
                                    std::cerr << qPrintable("requête nulle");
                                }
                                else
                                {
                                    choixReq(req);
                                }
								break;
							}
							// x y et z servent à éditer le gabarit du lien :
                            // avant, entre et après les 2 nœuds père et fils.
						case 'x':
						    {
                                /*
								   LLienSub subs;
							       switch (eclats.at (2).at(0).unicode ())
							       {
								   case 's':
								   {
								   break;
								   }
								   case 'd':
								   {
								   break;
								   }
								   default: std::cerr << qPrintable (m)
								   << "à placer en tête de lien, erreur d'url\n"; break;
							       }
								   QString trav = saisie ("changer le lien syntaxique ...",  subs.at(0)->trAv());
								// recouvrir le gabarit s'il définit une tête de traduction
								if (trav.isEmpty()) trav = "_";
								foreach (LienSub *sub, subs)
								sub->setTrAv (trav);
                                 */
							    break;
							}
						case 'y':
							{
                                /*
								   LLienSub subs;
								   switch (eclats.at (2).at(0).unicode ())
								   {
								   case 's':
								   {
								   break;
								   }
								   case 'd':
								   {
								   break;
								   }
								   case 'a':
								   {
								   break;
								   }
								   default: std::cerr << qPrintable (m)
								   <<", édition de lien syntaxique, erreur d'Url\n"; break;
								   }
								   if (!subs.empty())
								   {
								   QString trin = saisie ("insérer dans la traduction", subs.at(0)->trInter());
								   foreach (LienSub *sub, subs)
								   sub->setTrInter (trin);
								   }
                                 */
								break;
							}
						case 'z':
						    {
                                /*
							       LLienSub subs;
							       switch (eclats.at (2).at(0).unicode ())
							       {
								   case 's':
								   {
								   break;
								   }
								   case 'd':
								   {
								   break;
								   }
								   default: std::cerr << qPrintable (m) <<", édition de lien syntaxique, erreur d'Url\n";
								   break;
								   }
								   if  (!subs.empty())
								   {

								   QString trseq = saisie ("ajouter à la traduction", subs.at(0)->trSeq ());
								   foreach (LienSub *sub, subs)
								   sub->setTrSeq (trseq);
								   }
                                 */
							    break;
							}
						case 'r':   // rejet d'un lien
					      	{
                                if (req == 0)
                                {
                                    std::cerr << qPrintable("Requête introuvable, rang ")<<rang;
                                    return;
                                }
                                req->annuleRequis("rejet demandé");
                                if (req->requerant() == cour)
                                {
                                    req->setRejetee(true, "rejet explicite");
                                }
								break;
					      	}
						default: std::cerr << qPrintable(m)<<", erreur d'url lien"<<"\n"; break;
					} // eclats[1}
					break;
				} // case 'l'
			default: std::cerr << qPrintable (m) << ", commande mal formée\n"; break;
		} // switch eclats[0]
        majAffichage();
        emit(repondu(_reponse));
        return;
	}
	majAffichage();
	emit(repondu(_reponse));
}

bool Phrase::estFeminin(QString n)
{
    if (n.isEmpty()) return "?";
	return _feminins.contains(n.toLower());
}

bool Phrase::estVbRelative(MotFlechi* mf)
{
    bool antec = false;
    bool qui2 = false;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->super() == mf && req->valide())
        {
            qui2 = qui2 || req->sub()->lemme()->cle() == "qui2";
        }
        if (req->sub() == mf && req->valide())
        {
            antec = antec || req->id() == "antecedent";
        }
        if (antec && qui2) return true;

        /*
            && req->valide() && req->id() != "antecedent"
            && req->sub()->lemme()->cle() == "qui2")
            return true;
        */
    }
    return false;
}

void Phrase::initFeminins()
{
	QFile fp (qApp->applicationDirPath ()+"/data/feminin.fr");
	if (fp.open (QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream fluxD (&fp);
        while (!fluxD.atEnd ())
        {
	        _feminins.append (fluxD.readLine());
        }
        fp.close ();
    }
    else std::cerr << "fichier feminin.fr introuvable";
}

void Phrase::initLgr()
{
	QFile fp (qApp->applicationDirPath ()+"/data/synt.fr");
	if (fp.open (QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream fluxD (&fp);
        QStringList lgr;
        QString pos;
        while (!fluxD.atEnd ())
        {
            QString lin = fluxD.readLine().simplified();
            if (lin.isEmpty() || lin.startsWith('!')) continue;
            if (lin.startsWith("pos:"))
            {
                if (!lgr.isEmpty())
                {
                    _mapLgr.insert(pos, lgr);
                }
                pos = lin.section(':',1,1);
                lgr.clear();
            }
            else lgr.append(lin);
        }
        // dernière liste :
        _mapLgr.insert(pos, lgr);
        fp.close ();
    }
    else std::cerr << "fichier synt.fr introuvable";
}

bool Phrase::isomorph(QString ma, QString mb)
{
    QStringList lma = ma.split(' ');
    QStringList lmb = mb.split(' ');
    QStringList lminus = lma;
    QStringList lplus = lmb;
    if (lplus.count() < lminus.count())
    {
        lminus = lmb;
        lplus = lma;
    }
    for (int i=0;i<lminus.count();++i)
        if (!lplus.contains(lminus.at(i))) return false;
    return true;
}

QString Phrase::gauche(Mot *m)
{
    return entreMots.at(m->rang());
}

QString Phrase::grLu()
{
	if (_mots.empty())
		return "erreur, liste vide.";
	QString ret;
	QTextStream fl (&ret);
	for (int i=0;i<_mots.count();++i)
	{
		if (i != _imot)
			fl << " <a href=\"i."<<i<<"\">&bull;</a>";  // ou &middot;
		if (i == _imot)
			fl << " <span style=\"color:green;font-weight:bold;\">";
		else if (i > _imot)
			fl << " <span style=\"color:lightgrey;\">";
		else fl << " <span>";
		//fl << _mots.at(i)->gr() << "</span> ";
		fl << _mots.at(i)->gr() << droite(_mots.at(i)) << "</span> ";
	}
	return ret;
}

// Attention, _lemme et _lemmeExclu sont mal nommés
int Phrase::handicap(MotFlechi* mf)
{
    QString gr = mf->gr().toLower();
    gr.replace('v','u');
    gr.replace('j','i');
    Handicap* h = _handicaps.value(gr, 0);
    if (h == 0) return 0;
    QString mflem = mf->lemme()->cle();
    if ((mflem != h->lemme()) || (mflem == h->lemmeExclu()))
        return h->hand();
    return 0;
}

QList<Requete*> Phrase::homolexes(Requete* req)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* ri = _requetes.at(i);
        if (ri->close()
            && ri != req
            && ri->super()->mot() == req->super()->mot()
            && ri->sub()->mot() == req->sub()->mot())
            ret.append(ri);
    }
    return ret;
}

QList<Requete*> Phrase::homonymes(Requete* req)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* ri = _requetes.at(i);
        if (ri->close()
            && ri != req
            && ri->super()->mot() == req->super()->mot()
            && ri->id() == req->id()
            && ri->sub()->mot() == req->sub()->mot())
            ret.append(ri);
    }
    return ret;
}

QList<Requete*> Phrase::homonymes(QString id)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* ri = _requetes.at(i);
        if (ri->close() && ri->id() == id)
            ret.append(ri);
    }
    return ret;
}

bool Phrase::sortR(Requete* ra, Requete* rb)
{
    if (ra->poids() > rb->poids()) return true;
    return false;
}

QString Phrase::htmlLiens()
{
    Mot* mc = motCourant();
    QList<Requete*> lr;
    // lr, liste des requêtes closes
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->close() && !req->rejetee()
            //&& (req->requerant() == mc || req->requis() == mc))
            && (req->requis() == mc))
            {
                lr.append(req);
            }
    }
    // tri en fonction de la probabilité
    qSort(lr.begin(), lr.end(), sortR);
    QStringList ll;
    for (int i=0;i<lr.count();++i)
    {
        Requete* req = lr.at(i);
        ll.append(req->html());
    }
    ll.removeDuplicates();
    return ll.join("\n");
}

QStringList Phrase::lgr(char pos)
{
    for (int i=0;i<_mapLgr.keys().count();++i)
    {
        QString cle = _mapLgr.keys().at(i);
        if (cle.contains(pos))
            return _mapLgr.value(cle);
    }
    return QStringList() << "-";
}

/**
 * \fn QList<Requete*> Phrase::lReqSub(MotFlechi* mf, bool closes)
 * \brief Liste des requêtes dont mf est sub. Si 'closes', seules
 *  sont renvoyées les requêtes closes.
 */
QList<Requete*> Phrase::lReqSub(MotFlechi* mf, bool closes)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if ((!closes || req->close()) && req->sub() == mf)
            ret.append(req);
    }
    return ret;
}

/**
 * \fn QList<Requete*> Phrase::lReqSup(MotFlechi* mf, bool closes)
 * \brief Liste des requêtes dont mf est sup. Si 'closes', seules
 *  sont renvoyées les requêtes closes.
 */
QList<Requete*> Phrase::lReqSup(MotFlechi* mf, bool closes)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if ((!closes || req->close()) && req->super() == mf)
            ret.append(req);
    }
    return ret;
}

void Phrase::lemmatise()
{
    for (int i=0;i<_mots.count();++i)
    {
        Mot* mc = _mots.at(i);
        mc->setFlechis(_lemmatiseur->lemmatiseM(mc->gr()));
    }
}

void Phrase::majAffichage()
{
    // TODO - ajouter un texte d'accueil et d'aide
	if (_mots.empty())
		//_reponse = Chaines::initAff.arg (Chaines::titrePraelector);
        _reponse = QString("%1%2%3")
            .arg(Chaines::titrePraelector)
            .arg(Chaines::menu)
            .arg(Chaines::documentation);
	else
    {
        _reponse = Chaines::affichage
	        .arg(Chaines::titrePraelector)
            .arg(Chaines::menu)
	        .arg(grLu())
	        .arg(motCourant()->html())
	        .arg(htmlLiens())
	        .arg(tr())
            .arg(Chaines::menu);
    }
}

Requete* Phrase::montante(Mot* m)
{
    for (int i=0; i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if(req->close() && req->sub()->mot() == m)
            return req;
    }
    return 0;
}

Mot* Phrase::motCourant()
{
    return _mots.at(_imot);
}

Mot* Phrase::motNo(int i)
{
    if (i > _mots.count()-1)
        return 0;
    return _mots.at(i);
}

int Phrase::nbMots()
{
    return _mots.count();
}

int Phrase::nbPonct(QChar p, Mot* ma, Mot* mb)
{
    // trouver le premier mot
    Mot* prem = ma;
    Mot* der = mb;
    if (ma->rang() > mb->rang())
    {
        prem = mb;
        der = ma;
    }
    int ret = 0;
    for (int i=prem->fin();i<der->debut();++i)
    {
        if (_gr.at(i) == p) ++ret;
    }
    return ret;
}

int Phrase::nbRegles()
{
    return _regles.count();
}

int Phrase::nbRequetes()
{
    return _requetes.count();
}

int Phrase::nbSuper(MotFlechi* mf)
{
    int ret = 0;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->valide() && req->sub() == mf)
            ++ret;
    }
    return ret;
}

QString Phrase::num()
{
    return _num;
}

void Phrase::nettoieHomonymes(QString id)
{
    Requete* rprox = 0;
    QList<Requete*> lrc = homonymes(id);
    for (int i=0;i<lrc.count();++i)
    {
        Requete* req = lrc.at(i);
        if (rprox == 0) rprox = req;
        else if (req->sub()->mot() == rprox->sub()->mot())
        {
            if (req->distance() < rprox->distance())
            {
                rprox->annuleRequis(" distance plus longue que celle de la req. "+rprox->numc());
                rprox = req;
            }
        }
    }
}

int Phrase::numNouvReq()
{
    return ++_numReq;
}

void Phrase::peupleRegles(QString nf)
{
    QStringList lr = _lemmatiseur->lignesFichier(qApp->applicationDirPath() + "/data/"+nf);
    QStringList ll;
    QMap<QString,QString> vars;
    for (int i=0;i<lr.count();++i)
    {
        QString lin = lr.at (i).simplified();
        if (lin.startsWith("include:"))
        {
            peupleRegles(lin.section(":",1));
        }
        else if (lin.startsWith("$"))
        {
            lin.remove(0,1);
            QStringList ecl = lin.split('=');
            vars[ecl.at(0)] = ecl.at(1);
        }
        else
        {
            if (lin.startsWith("id:") || i==lr.count()-1)
            {
                if (i==lr.count()-1) ll.append(lin);
                if (!ll.empty())
                {
                    Regle* nr = new Regle(ll);
                    _regles.append(nr);
                    ll.clear();
                }
            }
            ll.append(lin);
        }
    }
}

void Phrase::peupleHandicap()
{
    QStringList lr = _lemmatiseur->lignesFichier(qApp->applicationDirPath() + "/data/handicap.la");
    QStringList ll;
    for (int i=0;i<lr.count();++i)
    {
        QStringList ecl = lr.at (i).simplified().split(':');
        _handicaps.insert(ecl.at(0), new Handicap(ecl));
    }
}

/**
 * liste des mots entre a et b, a et b non compris.
 */
QList<Mot*> Phrase::portee(int a, int b)
{
    int d;
    int f;
    if (a < b)
    {
        d = a;
        f = b;
    }
    else
    {
        d = b;
        f = a;
    }
    QList<Mot*> ret;
    for (int i=d+1;i<f;++i)
        ret.append(_mots.at(i));
    return ret;
}

Regle* Phrase::regle(int i)
{
    return _regles.at(i);
}

//void Phrase::reinit(Mot* m)
void Phrase::reinit()
{
    for (int i=_imot;i<_mots.count();++i)
    {
        _mots.at(i)->reinit();
    }
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req == 0) continue;
        if (req->close()
             && (req->super()->mot()->rang() >= _imot
                 || req->sub()->mot()->rang() >= _imot))
            {
            req->setRejetee(false);
            req->setValide(false);
            }
    }
}

Regle* Phrase::regle(QString id)
{
    for(int i=0;i<_regles.count();++i)
    {
        Regle* r = _regles.at(i);
        if (r->id() == id) return r;
    }
    return 0;
}

QList<Requete*> Phrase::reqCC(Mot* m)
{
    QList<Requete*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* ri = _requetes.at(i);
        if (ri->close()
            && (ri->sub()->mot() == m)
            && (ri->aff() == "coord1"))
        {
            ret.append(ri);
        }
    }
    return ret;
}

Requete* Phrase::requete(int n)
{
    if (n < _requetes.count())
        return _requetes.at(n);
    return 0;
}

/*
 * QString Phrase::saisie (QString l, QString s)
 *   saisie par ouverture d'un dialogue
 *   l est le label, s est la chaîne à éditer
 */

QString Phrase::saisie(QString l, QString s)
{
	s = s.simplified();
	QString ret = s;
	Dialogue* dialogue = new Dialogue ();
	dialogue->setLabel (l);
	dialogue->setText (s);
	int res = dialogue->exec();
	if (res == QDialog::Accepted)
		ret = dialogue->getText ().simplified();
	delete dialogue;
	return ret;
}

QString Phrase::gr()
{
    return _gr;
}

void Phrase::setGr(QString t)
{
    // initialisations
    _gr = t.simplified();
    /*
    // Ajouter une ponctuation finale manquante
    if (_gr.at(_gr.length()-1).isLetter())
        _gr.append('.');
    */
    // ôter les balises html
    _gr.remove(QRegExp("<[^>]*>"));
    // et les espaces de début de ligne
    _gr.replace("\n ", "\n");

    // vider les données de la phrase précédente
    entreMots.clear();
    _numReq = -1;
    while (!_requetes.empty())
    {
        Requete* req = _requetes.takeFirst();
        delete req;
    }
    while (!_mots.empty())
    {
        Mot* m = _mots.takeFirst();
        delete m;
    }

    // initialisation des mots
    QString nm;   // nouveau mot
    int     dm=0;   // début du mot
    QString ne;   // nouvel entremots
    for (int i=0;i<_gr.length();++i)
    {
        QChar c = _gr.at(i);
        bool lettre = c.isLetter();
        if (i == _gr.length()-1)
        {
            if (lettre)
            {
                nm.append(c);
            }
            else
            {
                ne.append(c);
            }
            if (!nm.isEmpty())
            {
                _mots.append(new Mot(nm, dm, i, _mots.count(), this));
            }
            if (!ne.isEmpty())
                entreMots.append(ne);
        }
        else if (lettre)
        {
            if (!ne.isEmpty() || i==0)
            {
                entreMots.append(ne);
                ne.clear();
                dm = i;
            }
            nm.append(c);
        }
        else
        {
            if (!nm.isEmpty())
            {
                Mot *nmot = new Mot(nm, dm, i, _mots.count(), this);
                _mots.append(nmot);
                nm.clear();
            }
            ne.append(c);
        }
    }
}

void Phrase::setLiens()
{
    Mot* mc = motCourant();
    // essai de résolution des requêtes en attente.
    for (int im=0;im<mc->nbFlechis();++im)
    {
        MotFlechi* mf = mc->flechi(im);
        // copie de la liste des requêtes
        QList<Requete*> listeR;
        for (int ir=0;ir<_requetes.count();++ir)
        {
            Requete* r = _requetes.at(ir);
            if (r != 0 && !r->close()) listeR.append(r);
        }

        while (!listeR.empty())
        {
            Requete *req = listeR.takeFirst();
            if (mf->resout(req))
            {
                Requete* nr = req->clone();
                ajRequete(nr, true);
                req->setRequis(mf, "résolue");
            }
        }
    }
}

QList<Mot*> Phrase::supersDe(Mot* m)
{
    QList<Mot*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->close() && req->sub()->mot() == m)
            ret.append(req->super()->mot());
    }
    return ret;
}


QString Phrase::tr(bool color)
{
    _tr.clear();
    QTextStream fl(&_tr);
    QStringList sl;
    for(int i=0;i<=_imot;++i)
	{
        Mot* m = _mots.at(i);
		if (m->estSommet())
        {
            QString g = m->trGroupe();
            // si la coloration est demandée
            if (i == _imot && color)
            {
                //                                        moccasin
                g.prepend("<span style=\"background-color:peachpuff;\">");
                g.append("</span>");
            }
            // le nouveau mot est sommet tant qu'un lien n'a pas été validé
            sl.append(g);
        }
	}
    _tr = sl.join("<br/>");
    return _tr;
}

void Phrase::traceReq()
{
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req != 0) std::cout << qPrintable("\n----------\n"+req->hist());
        else std::cout << qPrintable("\n---\nrequête "+QString::number(i)+" nulle");
    }
}

MotFlechi* Phrase::vbRelative(MotFlechi* mf)
{
    if (mf->lemme()->gr() != "qui") return 0;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->valide() && req->aff() != "antecedent" && req->sub() == mf)
            return req->super();
    }
    return 0;
}

MotFlechi* Phrase::vbSuper(MotFlechi* mf)
{
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->valide() && req->sub() == mf
            && QString("wv").contains(req->super()->pos()))
            return req->super();
    }
    return 0;
}
