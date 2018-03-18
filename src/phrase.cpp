/* phrase.cpp */

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// textes :
// ~/prof/latin/txt/catalogue.txt
// ../corpus/marcus.txt
// TODO XXX FIXME 

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
    peupleRegles("regles.la");
    peupleHandicap();
    setGr(t);
    lemmatise();
}

bool Phrase::accord(MotFlechi* ma, MotFlechi* mb, QString cgn)
{
    QString cma = ma->morpho();
    QString cmb = mb->morpho();
    // ajouter une personne si elle est requise par cgn
    if (cgn.contains('p'))
    {
        // trouver le non-verbe
        if (!ma->lemme()->pos().contains('v') && !ma->lemme()->pos().contains('w'))
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

void Phrase::ajListeR(Requete* req)
{
    if (!_listeR.contains(req))
    {
        _listeR.append(req);
    }
}

void Phrase::ajRequete(Requete* req)
{
    // s'assurer que la requête n'est pas redondante
    if (req->close())
    {
        for (int i=0;i<_requetes.count();++i)
        {
            Requete* ri = _requetes.at(i);
            if (!ri->close()) continue;
            //if (ri->super()->egale(req->super())
            if (ri->super() == req->super()
                && ri->id() == req->id()
                && ri->sub() == req->sub())
            {
                ri->ajHist("réutilisée par "+req->debog());
                return;
            }
        }
    }
    req->setNum(_requetes.count());
    _requetes.append(req);
}

QString Phrase::arbre(QString format, bool trace)
{
    // signetArbre
    QString gv;
    QTextStream fl(&gv);
    // en-tête du graphe
    QString grlabel = _gr.simplified();
    grlabel.replace("\"", "\\""");
    if (format == "dot")
        fl  << "digraph arbre {\n"
            << "graph [ordering=\"out\"];\n"
            << "labelloc=\"t\";\n"
            << "label=\""<<grlabel<<"\";\n"
            //<< "label=\""<<_num<<". "<<_grlabel<<"\";\n"
            << "rotate=90;\n"
            << "node [shape=box];\n";
    else if (format=="easy")
        fl << "["<<_gr<<"]\n";
        //fl << "["<<_num<<" "<<_gr<<"]\n";

    for (int i=0;i<_mots.count();++i)
    {
        Mot* mc = _mots.at(i);
        //debog = mc->gr() == "fuit";
        // définition du nœud
        if (format=="dot")
            fl <<"N"<<mc->rang()<<" [label=\""<<mc->gr()<<"\"];\n";
        // lemmatisation et analyse morpho
        mc->setMorphos(_lemmatiseur->lemmatiseM(mc->gr()));

        // calcul de tous les fléchis de mc
        for (int il=0;il<mc->morphos().keys().count();++il)
        {
            Lemme *l = mc->morphos().keys().at(il);
            for (int im=0;im<mc->morphos()[l].count();++im)
            {
                QString m = mc->morphos()[l].at(im).morpho;
                MotFlechi* mf = new MotFlechi(l, m, mc);
                mc->ajFlechi(mf);
            }
        }

        // essai de résolution des requêtes en attente.
        for (int im=0;im<mc->nbFlechis();++im)
        {
            MotFlechi* mf = mc->flechi(im);
            // copie de la liste des requêtes
            _listeR.clear();
            for (int ir=0;ir<_requetes.count();++ir)
            {
                Requete* r = _requetes.at(ir);
                //r->initExx();
                ajListeR(r);
            }

            while (!_listeR.empty()) 
            {
                Requete *req = _listeR.takeFirst();
                if (mf->resout(req))
                {
                    // si la requête est close, il faut pouvoir la 
                    // filtrer sous un autre numéro.
                    if (req->close())
                    {
                        Requete* reqtemp = req->clone();
                        reqtemp->setRequis(mf, "requis de requête clonée sur la req. "+req->numc());
                        filtre(reqtemp);
                        if (reqtemp->close())
                        {
                            req->setRequis(reqtemp->requisFl(), "requis pris sur la req. temporaire");
                            req->ajHist("CHANGEMENT de requis : "+mf->morpho());
                        }
                    }
                    else
                    {
                        // d'abord, fermer la requête
                        req->setRequis(mf, "non close, résolue");
                        filtre(req);
                        if (req->close()) req->ajHist("FILTRÉE : "+req->humain());
                    }
                    // détection de boucle de liens
                    boucle(req);
                }
            }
        }
        // lancement de nouvelles requêtes si mc n'est pas le dernier mot
        if (i < _mots.count()-1) mc->lance();
    }

    // suppression des coord1 restées sans coord2
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->close() && req->aff()=="coord2")
        {
            for (int irc=0;irc<req->super()->nbReqSub();++irc)
            {
                Requete* rcc = req->super()->reqSub(irc);
                if (rcc->close() && !accord(req->sub(), rcc->super(), req->regle()->accord()))
                    rcc->annuleRequis("ne s'accorde pas avec le 2d membre de la coordination");
            }
        }
    }

    nettoieHomonymes("conjcoordAdv");
    nettoieHomonymes("conjcoordN");
    nettoieHomonymes("conjcoordV");
    // supprimer les coord1 sans coord2
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->close() && req->aff()=="coord1"
            && !req->sub()->estSuperParAff("coord2"))
            req->annuleRequis("coordination sans second membre");
    }

    // relecture pour raccrocher les mots perdus
    //relecture();

    // ajout des liens syntaxiques dans le source GraphViz
    QStringList liens;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        // signetArbre
        if (req->close())
        {
            liens.append(req->gv(format));
        }
        else req->ajHist("REJETÉE "+req->humain());
        if (trace) qDebug().noquote()<<req->hist();
    }
    // debog récapitulation
    if (trace) 
    {
        qDebug()<<"       =====";
        qDebug()<<"   Requêtes closes:";
        for (int i=0;i<_requetes.count();++i)
        {
            Requete* req = _requetes.at(i);
            if (req->close()) qDebug()<<_requetes.at(i)->debog();
        }
    }
    // fin debog
    liens.removeDuplicates();
    fl<<liens.join("\n");
    if (format=="dot") fl << "}";
    return gv;
}

bool Phrase::boucle(Requete* req)
{
    //debog = req->num()==82;
    if (!req->close()) return false;
    Mot* msup = req->super()->mot();
    Requete* rmin = req;
    int poidsmin = req->poids();
    QList<Mot*> supers = supersDe(msup);
    if (supers.empty()) return false;
    Mot* ms = supers.at(0);
    // XXX dans la phrase
    // in omnem vitam unguento abstinemus, quoniam optimus odor in corpore est nullus.
    // boucle infinie. Provisoire : int iter
    int iter = 0;
    while (ms != msup && iter<10)
    {
        Requete* rm = montante(ms);
        if (rm == 0) return false;
        if (rm->poids() < poidsmin) 
        {
            poidsmin = rm->poids();
            rmin = rm;
        }
        ms = rm->super()->mot();
        ++iter;
    } 
    rmin->annuleRequis("requête la plus faible de la boucle");
    return true;
}

// Le MotFlechi mf est-il compatible avec toute requête close
// où figure m ?
bool Phrase::compatible(MotFlechi* mf, Mot* m)
{
    if (!m->estLie()) return false;
    for (int i=0;i<m->nbFlechis();++i)
        if (compatible(mf, m->flechi(i))) return true;
    return false;
}

bool Phrase::compatible(MotFlechi* ma, MotFlechi* mb)
{
    return ma->lemme() == mb->lemme() && isomorph(ma->morpho(), mb->morpho());
}

// conflit : ra doit être plus ancienne que rb.
void Phrase::conflit(Requete* ra, Requete* rb, QString cause)
{
    // signetConflit
    // ne traiter que des requêtes complètes
    if (!ra->close() || !rb->close()) return;

    // initialisations
    // Poids des requêtes. l'avantage contiguïté
    // y est compté.
    int rap = ra->poids();
    int rbp = rb->poids();

    QString h = cause +"\nra:"+ra->debog() +"\nrb:"+rb->debog();

    ra->ajHist("---\nconflit RA\n"+h);
    rb->ajHist("---\nconflit RB\n"+h);

    // distance
    int distanceA = ra->distance();
    int distanceB = rb->distance();

    // conflits de sujets
    if (ra->aff() == "sujet" && rb->aff() == "sujet")
    {
        // l'ordre le plus courant est sujet attribut verbe
        // ra est le premier lien sujet détecté, elle a la
        // priorité.
        if (ra->id() == "sujet" && rb->id() == "sujet")
        {
            rap += 20;
            /*
            int pra = ra->largeur();
            int prb = rb->largeur();
            int diff = abs(pra - prb);
            if (diff > 0 && diff < 5)
            {
                if (pra < prb) rbp += 5;
                else if (prb < pra) rap += 5;
            }
            else
            {
                if (pra < prb) rap += 5;
                else if (prb < pra) rbp += 5;
            }
            */
        }
    }
    else
    {
        // distance 
        if (distanceA < distanceB) rap += (distanceB - distanceA);
        else if (distanceB < distanceA) rbp += (distanceA - distanceB);
    }
    
    // de la fréquence des lemmes :
    int freqa = ra->freq(); 
    int freqb = rb->freq();
    if (freqa < 8 && freqa - freqb > 2000) rap += 10;
    else if (freqb < 8 && freqb - freqa > 2000) rbp += 10;

    // avantage si liens associés
    if (!ra->regle()->subEstSup().isEmpty())
        rap += 40;
    if (!rb->regle()->subEstSup().isEmpty())
        rbp += 40;
    // annulation du requis de la requête perdante
    if (rap > rbp)
    {
        ra->ajHist("requête ra "+ra->numc()+" ANNULE la requête rb "+rb->numc()
                   +", poids ra "+QString::number(rap)+" rb "+QString::number(rbp));
        rb->annuleRequis("rb "+QString::number(rbp)+" poids moindre que ra "+QString::number(rap));
    }
    else if (rbp > rap)
    {
        rb->ajHist("requête rb "+rb->numc()+" ANNULE la requête ra "+ra->numc()
                   +", poids rb "+QString::number(rbp)+" ra "+QString::number(rap));
        ra->annuleRequis("ra "+QString::number(rap)+" poids moindre que rb "+QString::number(rbp));
    }
    // égalité de poids, avantage à l'antéposition
    else if (ra->superRequis() && rb->subRequis() && rb->sens() != '>')
    {
        ra->ajHist("requête "+ra->numc()+" ANNULE la requête "
                   +rb->numc()+" : antéposition");
        rb->annuleRequis(" par la requête " +ra->numc()+" : antéposition");
    }
    else if (rb->superRequis() && ra->subRequis() && ra->sens() != '>')
    {
        rb->ajHist("requête "+rb->numc()+" ANNULE la requête "
                   +ra->numc()+" : antéposition");
        ra->annuleRequis(" par la requête " +rb->numc()+" : antéposition");
    }
    else
    {
        ra->annuleRequis("ra annulée: Égalité, requête plus récente annulée");
        rb->ajHist("rb conservée: Égalité, requête plus ancienne conservée");
    }
}

bool Phrase::contigue(Requete* req)
{
    return (req->close() && !req->morte() && contigus(req->super()->mot(), req->sub()->mot()));
}

bool Phrase::contigus(Mot *a, Mot *b)
{
    return abs(a->rang() - b->rang()) == 1;
}

QString Phrase::droite(Mot *m)
{
    if (m->rang() >= entreMots.count())
        return "";
    else return entreMots.at(m->rang() + 1);
}

/*
 * Phrase::ecoute (QString m)
 *  Quelques mots clés :
 *  - suiv, prec : progression ou régression d'un mot
 *  - nouvPhr : saisie d'une nouvelle phrase
 *  sinon, m est une Url en sections séparées par des points
 *   1 == mm = mot
 *     si 1 == 'm' :
 *	   2. c = choisir et éliminer les concurrents,
 *        d = fixer le déterminant
 *        e = éditer
 *        r = rejeter
 *     3 : numéro de la morpho sur laquelle agir.
 *  si 1 == 'l' lien
 *     2. c = choisir ce lien et éliminer les concurrents
 *        d = déplacer le fils de ce lien à g. ou à d.
 *        e = éditer la traduction de ce lien
 *        r = rejeter ce lien
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
	/* deux commandes de navigation */
	else if (m == "-suiv" && (_imot < _mots.count() -1))
	{
		++_imot;
		if (_imot > _maxImot) _maxImot = _imot;
	}
	else if (m == "-prec" && _imot > 0)
	{
		--_imot;
	}
	/* Nouvelle phrase */
	else if (m == "-nouvPhr")
	{
        _mots.clear();
        // TODO : repeupler la phrase.
	}
	else if (m.startsWith("-phr-"))
	{
		m.remove (0, 5);
		_mots.clear();
        setGr(m);
        lemmatise();
		_imot = 0;
	}
	else // mots et liens
	{
		Mot *cour = motCourant();
		QStringList eclats = m.split ('.');
		QChar ml = eclats.at(0).at(0);
		switch (ml.unicode())
		{
			case 'i': _imot = eclats.at(1).toInt(); break;  // retour rapide vers le mot de rang m[1]
			/* PREMIÈRE SECTION : MORPHOS DU NOUVEAU MOT*/
			case 'm':  // m[0] : opération sur les morphos.
					  {
						  /* Calculer le numéro de la morpho sur laquelle agir */
						  int num = eclats.at (2).toInt ();
						  switch (eclats.at (1).at(0).unicode())
						  {
							  case 'a':  // ajout d'une entrée dans les additions
								  {
                                      // TODO à implémenter
									  QString nm = saisie (Chaines::ajTrEntree, "");
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
							  case 'c': // éliminer toutes les autres morphos
								  break;
							  case 'd': // rotation du déterminant
								  {
									  //MotFlechi *mm = cour->flechi(num);
									  //mm->setDet(estFeminin(mm->trNue ()));
									  break;
								  }
							  case 'e':  // édition de la traduction
								  {
									  QString lt;
									  QTextStream fl (&lt);
									  fl<<"Meilleure traduction pour " << cour->gr();
									  Dialogue *dialogue = new Dialogue ();
									  dialogue->setLabel (lt);
									  dialogue->setText (cour->flechi(num)->trfl());
									  int res = dialogue->exec ();
									  if (res == QDialog::Accepted)
                                          qDebug()<<"où est le fléchi ?";
										  //cour->setTraduction(dialogue->getText ());
									  delete dialogue;
									  break;
								  }
							  case 'l': // affichage complet de l'entrée : lemme, indMorph, traductions
								  {
                                      /*
									  QMessageBox mb;
									  mb.setText (cour->flechi(num)->definition ("fr"));
									  mb.exec ();
                                      */
									  break;
								  }
							  case 'r': // ??
								  break;
							  case 's': // rotation du sujet
								  {
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
 		 			   *     2. d = déplacer le fils de ce lien vers la g ou la d
 		 			   *        e = éditer la traduction de ce lien
 		 			   *        r = rejeter ce lien
					   *        x,y,z = éditer les ajouts français avant, entre, après F et P, ou P et F.
					   *
 		 			   */
			case 'l':      // l == lien
					  {
                          /*
						  QStringList lv;
						  if (eclats.count() > 3)
							  lv = eclats.at (3).split (',');       // liste des morphos affectées passées par m[3]
						  LLienSub lsub;
						  char gd = eclats.at(1).at(0).unicode();   // m[2] : opération à réaliser
						  switch (gd)      // pos 1, idgp info et déplacement
						  {
							  case 'd':    // déplacement d'un groupe à droite
							  case 'g':    // et à gauche
							  case 'i':    // info sur le lien
							  case 'p':    // changement de position dans la traduction
								  {
									  char d = eclats.at (2).at (0).unicode();
									  //bool droite = gd == 'd';
									  // définition de sub en utilisant pos 2.
									  switch (d)
									  {
										  case 's': // le lien est un sub
											  {
												  break;
											  }
										  case 'd': // le lien est un dep
											  {
												  break;
											  }
										  default:break;
									  }
									  if (lsub.empty())
									  	  break;
									  switch (gd)
									  {
										  case 'i':
											  {
											  	  QStringList info;
												  foreach (LienSub *sub, lsub)
												  	  info.append (sub->lien()->doc());
												  QMessageBox mb;
												  info.removeDuplicates ();
												  mb.setText (info.join ("<br/>"));
												  mb.exec ();
												  break;
											  }
										  case 'p':  // permutation P F
											  {
												  foreach (LienSub *s, lsub)
													  s->swapG ();
												  break;
											  }
										  default:  // déplacement du groupe à g ou à d
												  // foreach (LienSub *s, lsub) s->super()->deplaceFilsFr(s, droite);
												  break;
									  }
									  break;
								  }
							  case 'v':   // pos 1, valider
								  {
								  	  char sd = eclats.at (2).at(0).unicode ();
									  switch (sd)
									  {
										  case 's':  // m[2] 2, cour est super du lien validé-> dans les sub
											  {
												  break;
											  }
										  case 'a':  // m[2], cour est un relatif
										  case 'd':  // m[2], cour est fils du lien validé-> dans les deps
											  {
												  break;
											  }
										  default: std::cerr <<qPrintable (m) <<", erreur d'url validation lien"<<"\n";
									  }
									  break;
								  }
								  // x y et z servent à éditer le gabarit du lien : avant, entre et après les 2 nœuds père et fils.
							  case 'x':
							  	  {
								  	  //LienSub *sub = NULL;
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
								  	  break;
								  }
							  case 'y':
								  {
									  //LienSub *sub = NULL;
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
									  break;
								  }
							  case 'z':
							  	  {
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
								  	  break;
								  }
							  case 'r':   // rejet d'un lien
						  		  {
									  break;
						  		  }
							  default: std::cerr << qPrintable (m)<<"erreur d'url lien"<<"\n"; break;
						  }
                          */
						  break;
					  }
			default: std::cerr << qPrintable (m) << ", commande mal formée\n"; break;
		}
	}
	majAffichage ();
	emit (repondu (_reponse));
}

bool Phrase::estFeminin (QString n)
{
	return _feminins.contains(n);
}


bool Phrase::estSommet(Mot* m)
{
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (req->close() && req->sub()->mot() == m)
            return false;
    }
    return true;
}

void Phrase::initFeminins ()
{
	QFile fp (qApp->applicationDirPath ()+"/data/feminin.fr");
	fp.open (QIODevice::ReadOnly|QIODevice::Text);
	QTextStream fluxD (&fp);
	while (!fluxD.atEnd ())
	{
		_feminins.append (fluxD.readLine ());
	}
	fp.close ();
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

bool Phrase::filtre(Requete* req)
{
    // signetFiltre
    // XXX  19 épith
    //debog = req->num()==59||req->num()==93 || req->num()==101;

    // Une requête prep ou conj ne se résout pas tant que la prep n'a pas son régime,
    // ou tant que la conjonction n'a pas de verbe subordonné
    if ((req->super()->rang() > req->sub()->rang())
       && (((req->id() == "prep") && !(req->sub()->estSuperParId("regimeAbl") || req->sub()->estSuperParId("regimeAcc")))
      || (((req->id() == "conjSub") && (!req->sub()->estSuperParId("vInd") || req->sub()->estSuperParId("vSubj"))))))
    {
        req->annuleRequis("préposition ou conjonction antéposée sans sub");
        return false;
    }

    // un verbe conjugué ne doit pas séparer l'épithète du nom
    if ((req->aff() == "epithete" || req->aff()=="det")
        && req->sub()->morpho().contains("nominatif") && req->separeparVConj())
    {
        req->annuleRequis("un verbe entre nom et "+req->id());
        return false;
    }

    // un accusatif suivant immédiatement une prép+acc ne peut être objet
    if (req->aff()=="objet" && req->sub()->estSubParId("regimeAcc"))
    {
        req->annuleRequis("un régime de préposition ne peut être objet");
        return false;
    }


    //   Blocages de projectivité
    bool blocr = false; // blocage après préposition
    bool blocc = false; // blocage après conjonction
    //bool bloce = false; // blocage après coordination
    bool blocq = false; // blocage après relatif
    bool bloci = false; // blocage après le sujet de l'infinitive
    QList<Mot*> mb = req->portee();
    if (!mb.empty()) for (int ib=0;ib<_requetes.count();++ib)
    {
        Requete* rb = _requetes.at(ib);
        QString id = rb->id();
        QString af = rb->aff();
        bool crux = req->croise(rb);
        if (!rb->close() || rb == req) continue;
        // blocage conjonctive
        if ((id=="conjSub" || id=="vInd" || id=="vConj") && crux)
            blocc = true;
        else if ((id=="regimeAbl" || id=="regimeAcc" || id=="gerondAd") && crux)
        {
            blocr = true;
        }
        else if (id.contains("Inf") && req->super()->lemme()->synt("propinf"))
            bloci = true;
        else for (int i=0;i<mb.count();++i)
        {
            Mot* m = mb.at(i);
            // échec du lemmatiseur
            if (m->morphos().empty()) continue;
            // la préposition est sub d'un lien prep
            if (m->estPrep() && crux
                && !(m->estSuperParId("regimeAbl")
                     || m->estSuperParId("regimeAcc")
                     || m->estSuperParId("gerondAd")))
                blocr = true;
            else if (m->estRelatif()) blocq = true;
            if (req->super()->lemme()->synt("propinf") && (!req->id().contains("Inf")))
                bloci = true;
        }
    }
    if (blocc || blocr || blocq || bloci) // || bloce)
    {
        QString sb = "relative";
        if (blocr) sb = "préposition";
        else if (blocc) sb = "conjonctive";
        //else if (bloce) sb = "coordination";
        else if (bloci) sb = "infinitive";
        req->annuleRequis("blocage "+sb);
        return false;
    }

    // conflit avec les requêtes closes
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* ri = _requetes.at(i);

        if (ri == req || !ri->close()) continue;

        // Une proposition infinitive commence toujours par son sujet. Voir si le lien req ou ri
        // n'est pas contraire à la projectivité
        if (req->id()=="sujetPropInf")
        {
            int risr = ri->sub()->rang();
            int rsr = req->sub()->rang();
            if (ri->super()->rang() < req->sub()->rang()
                && risr > rsr && risr < req->super()->rang())
            {
                ri->annuleRequis("le sub du lien est à l'intérieur d'une infinitive");
                continue;
            }
        }

        // l'adj contigu avec un nom ne peut être attribut par un V si ce nom peut l'être aussi
        if (req->id() == "attrSA" && ri->id()=="attrSN" && contigus(req->sub()->mot(), ri->sub()->mot()))
        {
            req->annuleRequis("épithète de son concurrent");
            return false;
        }

        // conflits
        bool idSuper = req->super()->mot() == ri->super()->mot();
        bool idSub   = req->sub()->mot()   == ri->sub()->mot();
        bool idSuperFl = req->super() == ri->super();
        bool idSubFl   = req->sub()  == ri->sub();
        bool compat = false;

        // départager d'abord les fléchis du même mot
        if ((idSuper && !idSuperFl) || (idSub && !idSubFl))
        {
            conflit(req, ri,"utilisation de formes différentes du même mot");
            if (!req->close()) return false;
            if (!ri->close()) continue;
        }

        // autres conflits
        if (idSuper) compat = compatible(req->super(), ri->super()) && req->regle()->compatibleSupSub(ri->regle());
        if (idSub) compat = compatible(req->sub(), ri->sub()) && req->regle()->compatibleSubSub(ri->regle());

        // même super, même sub
        if (idSuper && idSub)
        {
            if (req->regle()->exclut(ri->regle()->id()))
                ri->annuleRequis(" par la requête prioritaire "+req->debog());
            else if (ri->regle()->exclut(req->regle()->id()))
                req->annuleRequis(" par la requête prioritaire "+ri->debog());
            else conflit(req, ri, "mêmes super et sub");
            if (!req->close()) return false;
            if (!ri->close()) continue;
        }

        // super et sub réciproques
        if ((ri->sub()->mot() == req->super()->mot()) && (ri->super()->mot() == req->sub()->mot()))
        {
            conflit(req, ri, "réciproques");
            if (!req->close()) return false;
            if (!ri->close())
            {
                continue;
            }
        }

        // sub de ri = super de req
        if ((ri->sub()->mot() == req->super()->mot()) && req->aff() != "antecedent"
            && (!compatible(req->super(), ri->sub()->mot()) || !req->regle()->compatibleSupSub(ri->regle())))
        {
            conflit(req, ri, "sub=super");
            if (!req->close()) return false;
            if (!ri->close())
            {
                continue;
            }
        }

        // super de ri = sub de req
        if (ri->super()->mot() == req->sub()->mot()
            && (!compatible(req->sub(), ri->super()->mot()) || !req->regle()->compatibleSubSup(ri->regle()))
            && ri->aff() != "antecedent")
        {
            conflit(req, ri, "super=sub, morphos incompatibles");
            if (!req->close()) return false;
            if (!ri->close())
            {
                continue;
            }
        }

        // même super, subs différents.
        if (idSuper && !idSub)
        {
            // conflit défini dans la règle de req
            if (req->regle()->conflit(ri->id()))
                conflit(req, ri, "subs incompatibles pour le meme super");
            if (!req->close()) return false;
            if (!ri->close())  continue;
            /*
            // le sub de req est déterminant du sub de ri
            if (req->id() == "det" || ri->id()=="det")
            {
                if (ri->sub()->estSubParId("det"))
                {
                    req->annuleRequis(ri->sub()->gr()+"le sub de ri est déterminant. Ce lien est prioritaire");
                    return false;
                }
                else if (req->sub()->estSubParId("det"))
                {
                    ri->annuleRequis("le lien det de req"+req->sub()->gr()+" est prioritaire");
                    continue;
                }
            }
            */

            // Champ aff identique, et un seul sub aff permis
            if (ri->aff() == req->aff() && !req->multi()) //&& compat)
            {
                if (!req->close())
                {
                    if (req->aff() == "coord2")
                    {
                        req->ajHist("ANNULÉE par la req."+ri->numc());
                        if (req->coord1() != 0)
                            req->coord1()->annuleRequis("le second membre de la coordination a été annulé");
                    }
                    return false;
                }
                /*
                if (ri->close())
                {
                    if (debog) qDebug()<<"   oks 1b";
                    // si le super est indicatif et n'a pas de sujet, et que req pourrait être sujet
                    if (req->aff() != "sujet"
                        && req->sub()->mot()->estAu("nominatif")
                        && !req->super()->estSuperParAff("sujet"))
                        return false;
                }
                else*/ if (ri->aff() == "coord2")
                {
                    if (ri->coord1() != 0)
                        ri->coord1()->annuleRequis("le second membre de la coordination a été annulé");
                    continue;
                }
                conflit(req, ri, "même super, même id");
                if (!req->close()) return false;
                if (!ri->close()) continue;
            }
            // champ conflit:<id> de regles.la
            else if (req->enConflit(ri->id()) && !req->multi() && compat)
            {
                conflit(req, ri, "même super, ids en conflit");
                if (!req->close()) return false;
                if (!ri->close())  continue;
            }
            else if (!req->regle()->compatibleSupSup(ri->regle()))
            {
                Mot* mis = req->super()->mot();
                bool comp = false;
                for (int is=0;is<mis->nbFlechis();++is)
                {
                    MotFlechi* mfi = mis->flechi(is);
                    comp = comp || mfi == ri->super();
                }
                if (!comp)
                {
                    conflit(req, ri, "même super - non egaux ");
                    if (!req->close()) return false;
                    if (!ri->close())
                    {
                        continue;
                    }
                }
            }
        }

        // même sub, supers différents
        if (idSub && !idSuper )
        {
            if (req->aff() != "antecedent" && ri->aff()!="antecedent" && ri->aff()!="coord1")
            {
                if (ri->regle()->exclut(req->regle()->id()))
                    req->annuleRequis(" par la requête prioritaire "+ri->debog());
                else conflit(req, ri, "même sub");
                if (!req->close()) return false;
                if (!ri->close())
                {
                    continue;
                }
            }
            // antécédent
            else if (req->aff() == "antecedent" && ri->aff() == "antecedent")
            {
                conflit(req, ri, "2e antécédent");
                if (!req->close()) return false;
                if (!ri->close())
                {
                    continue;
                }
            }
            else conflit(req, ri, "même sub, supers différents");
            if (!req->close()) return false;
        }
    }
    return true;
} // filtre


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
		if (i <= _maxImot && i != _imot)
			fl << " <a href=\"i."<<i<<"\">&bull;</a>";  // ou &middot;
		if (i == _imot)
			fl << " <span style=\"color:green;font-weight:bold\">";
		else if (i > _imot)
			fl << " <span style=\"color:lightgrey;\">";
		else fl << " <span>";
		//fl << _mots.at (i)->grBrute() << "</span> ";
		fl << _mots.at(i)->gr() << "</span> ";
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

void Phrase::lemmatise()
{
    for (int i=0;i<_mots.count();++i)
    {
        Mot* mc = _mots.at(i);
        // lemmatisation et analyse morpho
        mc->setMorphos(_lemmatiseur->lemmatiseM(mc->gr()));

        // calcul de tous les fléchis de mc
        for (int il=0;il<mc->morphos().keys().count();++il)
        {
            Lemme *l = mc->morphos().keys().at(il);
            QList<SLem> lsl = mc->morphos().value(l);
            for (int im=0;im<lsl.count();++im)
            {
                QString m = lsl.at(im).morpho;
                MotFlechi* mf = new MotFlechi(l, m, mc);
                mc->ajFlechi(mf);
            }
        }
    }
}

void Phrase::majAffichage()
{
	if (_mots.empty())
		_reponse = Chaines::initAff.arg (Chaines::titrePraelector);
	else _reponse = Chaines::affichage
		.arg (Chaines::titrePraelector)
		.arg (grLu())                            // %1
		.arg (motCourant()->htmlMorphos())       // %2
		//.arg ("motCourant()->htmlMorphos()")       // %2
		.arg ("htmlLiens()")                       // %3
		.arg ("traduction()");                     // %4
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

int Phrase::nbListeR()
{
    return _listeR.count();
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

void Phrase::peupleRegles(QString nf)
{
    QStringList lr = _lemmatiseur->lignesFichier(qApp->applicationDirPath() + "/data/"+nf);
    QStringList ll;
    for (int i=0;i<lr.count();++i)
    {
        QString lin = lr.at (i).simplified();
        if (lin.startsWith("include:"))
        {
            peupleRegles(lin.section(":",1));
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

Regle* Phrase::regle(QString id)
{
    for(int i=0;i<_regles.count();++i)
    {
        Regle* r = _regles.at(i);
        if (r->id() == id) return r;
    }
    return 0;
}

void Phrase::relecture()
{
    for (int i=0;i<_mots.count();++i)
    {
        Mot* m = _mots.at(i);
        if (estSommet(m))
        {
            for (int ifl=0;ifl<m->nbFlechis();++ifl)
            {
                MotFlechi* mf = m->flechi(ifl);
                for (int ir=_requetes.count()-1;ir>=0;--ir)
                {
                    Requete* req = _requetes.at(ir);
                    if (req->close()) continue;
                    if (req->subRequis() && req->super()->mot() == m) continue;
                    if (req->superRequis() && req->sub() != mf) continue;
                    if (req->requerantFl()->nbCloses() == 0) continue;
                    if (req->aff() != "epithete" && req->aff() != "det") continue;
                    if (mf->resout(req))
                    {
                        req->setRequis(mf,"validée en relecture");
                        req->ajHist("validée en RELECTURE");
                    }
                }
            }
        }
    }
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

Requete* Phrase::requete(int i)
{
    return _requetes.at(i);
}

void Phrase::rmListeR(Requete* req)
{
    _listeR.removeOne(req);
}

/*
 * QString Phrase::saisie (QString l, QString s)
 *   saisie par ouverture d'un dialogue
 *   l est le label, s est la chaîne à éditer
 */

QString Phrase::saisie (QString l, QString s)
{
	s = s.simplified();
	QString ret = s;
	_dialogue = new Dialogue ();
	_dialogue->setLabel (l);
	_dialogue->setText (s);
	int res = _dialogue->exec ();
	if (res == QDialog::Accepted)
		ret = _dialogue->getText ().simplified();
	delete _dialogue;
	return ret;
}

void Phrase::setGr(QString t)
{
    entreMots.clear();
    _listeR.clear();
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
    _gr = t.simplified();
    // les balises html
    _gr.remove(QRegExp("<[^>]*>"));
    // et les espaces de début de ligne
    _gr.replace("\n ", "\n");
    // initialisation des mots
    QString nm;   // nouveau mot
    int     dm=0;   // début du mot
    QString ne;   // nouvel entremots
    for (int i=0;i<t.length();++i)
    {
        QChar c = _gr.at(i);
        if (c.isLetter())
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

void Phrase::setNum(int n)
{
    _num = n;
}

QList<Mot*> Phrase::supersDe(Mot* m)
{
    QList<Mot*> ret;
    for (int i=0;i<_requetes.count();++i)
    {
        Requete* req = _requetes.at(i);
        if (!req->morte() && req->close() && req->sub()->mot() == m)
            ret.append(req->super()->mot());
    }
    return ret;
}

