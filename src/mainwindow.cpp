/*                  mainwindow.cpp  */

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

//#define TRACEREQ

#include <QDebug>
#include <QDir>
#include <QSettings>

#include "mainwindow.h"

Editeur::Editeur(QWidget *parent) : QTextBrowser(parent)
{
    setOpenLinks(false);
}

void Editeur::emet(QUrl url)
{
    emit anchorClicked(url);
}

Raccourcis::Raccourcis()
{
    alpha  = "abcdefghijklmnopqrstuv";
    alpha2 = "abcdefghijklmnopqrstuvwxyz";
    wxyz   = "xyz";
    reinit();
}

QString Raccourcis::racc(QString url)
{
    if (url.isEmpty()) return "";
    if (url=="-zoom") return "+";
    if (url=="-dezoom") return "-";
    int p = 0;
    while (!(url.at(p).isLetter() || url.at(p).isDigit()) && p < url.length()) ++p;
    QChar c = url.at(p).toLower();
    if (!c.isLetter()) return "";
    if (a.isEmpty())
    {
        if (z == "-")
        {
            z = "w";
        }
        else if (wxyz.isEmpty() && a2.isEmpty()) return "";
        else if (a2.isEmpty())
        {
            z.clear();
            z.append(wxyz.at(0));
            wxyz.remove(0,1);
            a2 = alpha2;
        }
        QString ret;
        ret.append(z);
        if (a2.contains(c))
        {
            ret.append(c);
            a2.remove(c);
            return ret;
        }
        else
        {
            ret.append(a2.at(0));
            a2.remove(0,1);
            return ret;
        }
    }
    else if (a.contains(c))
    {
        a.remove(c);
        return QString(c);
    }
    else
    {
        QString ret;
        ret.append(a.at(0));
        a.remove(0,1);
        return ret;
    }
    return "";
}

void Raccourcis::reinit()
{
    a  = alpha;
    a2 = alpha2;
    z = "-";
    wxyz = "xyz";
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(834, 734);
    centralWidget = new QWidget();
    font.setPointSize(14);
	centralWidget->setFont (font);

    verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);

    textBrowser = new Editeur(centralWidget);
    verticalLayout->addWidget(textBrowser);

    setCentralWidget(centralWidget);
    raccourcis = new Raccourcis;
    // chercher un paramétrage dans QSettings
    lisSettings();

	phrase = new Phrase();
	connect(phrase, SIGNAL(repondu(QString)),this,SLOT(parle(QString)));
	connect(phrase, SIGNAL(editTr(QString)),this,SLOT(traceMf(QString)));
	connect(textBrowser, SIGNAL(anchorClicked(QUrl)),this, SLOT(calcul(QUrl)));
    clavier = false;
    relect = false;
    lurl = QStringList()
    << "-prec"
    << "-suiv"
    << "-pprec"
    << "-psuiv"
    << "i."
    << "m.r.m."
    << "m.r.f."
    << "m.i."
    << "m.e."
    << "m.c."
    << "m.d."
    << "m.s."
    << "l.r."
    << "l.v."
    << "l.t.";

    // construire la liste des clés alpha
    //alphabet = "cen/+-aq;,lrpsbdfgijkmotuv";
    /*
    alphabet = "ACEN/+-QPSrapsebcdfghijklmnoqtuv";
    alpha2 = "abcdefghijklmnopqrstuvwxyz";
    */
    /*
       c choisir une phrase
       e " enregistrée
       n saisir une Nouvelle phrase
       / clavier
       + taille police
       - taille police
       a annuler
       q quitter
       j phrase précédente
       l phrase suivante
       r réinitialiser
       p mot précédent
       s mot suivant
     */
    /*
    for (int i=0;i<alphabet.count();++i)
    {
        clesL.append(alphabet.at(i));
    }
    wxyz = "wxyz";
    for (int i=0;i<wxyz.length();++i)
    {
        QString cle(wxyz.at(i));
        for (int j=0;j<alpha2.count();++j)
        {
            clesL.append(cle+alpha2.at(j));
        }
    }
    */
	phrase->ecoute("-init");
}

MainWindow::~MainWindow()
{
    delete raccourcis;
}

void MainWindow::ajTouches()
{
    if (texte.isEmpty() || !texteT.isEmpty()) return;
    QString lien = "<span style=\"color:red;font-size:small;vertical-align:bottom;\">%1</a></span>";
    int i = 0;
    QString t = texte;
    QTextStream fl(&texteT);
    raccourcis->reinit();
    urls.clear();
    while (!t.isEmpty())
    {
        if (t.startsWith("<a href"))
        {
            // calcul de l'url
            QString url;
            t.remove(0,9);
            while (t.at(0) != '"')
            {
                url.append(t.at(0));
                t.remove(0,1);
            }
            // une instance de la classe Raccourcis renvoie le raccourci clavier
            QString cle = raccourcis->racc(url);
            fl << lien.arg(cle);
            urls.insert(cle, url);
            ++i;
            fl<<"<a href=\""<<url;
        }
        else
        {
            fl << t.at(0);
            t.remove(0,1);
        }
    }
}

void MainWindow::ajTrace(QString cmd)
{
    for (int i=0;i<lurl.count();++i)
        if (cmd.startsWith(lurl.at(i)))
        {
            trace.append(cmd);
        }
}

/**
 * \fn MainWindow::calcul (Qurl url)
 * \brief traite l'hyperlien cliqué.
 * Il le renvoie si nécessaire à la méhtode homonyme de Phrase.
 */
void MainWindow::calcul (QUrl url)
{
    //acceptNavigationRequest(url, QWebtextBrowserePage::NavigationTypeLinkClicked, true);
    texte.clear();
    texteT.clear();
    QString cmd = url.fromPercentEncoding(url.toString().toUtf8());
    // lecture d'une traduction éditée
    if (relect && cmd == trace.at(ienr))
    {
        ++ienr;
    }
	if (cmd == "-quitter")
	{
		// TODO : détruire MainWindow (et donc phrase), mais pê dans main.cpp.
		close ();
	}
	else if (cmd == "-aide")
	{
		parle(Chaines::documentation);
	}
    else if (cmd == "-clav")
    {
        clavier = !clavier;
        calcul(QUrl("-init"));
    }
	else if (cmd == "-corpus" || cmd == "-enr")
	{
        // supprimer le préfixe '-'
        cmd.remove(0,1);
        // nom du fichier de corpus
        _fCorpus = cmd;
		// charger la liste des fichiers du rép.
        QString page = catalogue(cmd);
        parle(page);
	}
	else if (cmd.startsWith("f."))
	{
        lenr.clear();
        QString nf = cmd.remove(0,2);
        parle(choixPhr(nf));
        setFTrace(nf);
	}
    else if (cmd.startsWith("-trace"))
    {
        enr();
    }
    else if (cmd == "-zoom")
    {
        textBrowser->zoomIn();
    }
    else if (cmd == "-dezoom")
    {
        textBrowser->zoomOut();
    }
    else if (cmd=="-pprec")
    {
        int np = phrase->num().toInt();
        --np;
        if (np > 2)
        {
            cmd = _phrases.at(np-1);
            calcul(cmd);
        }
    }
    else if (cmd=="-psuiv")
    {
        int np = phrase->num().toInt();
        ++np;
        if (np <= _phrases.count())
        {
            cmd = _phrases.at(np-1);
            calcul(cmd);
        }
    }
	else
	{
        if (cmd.startsWith("-phr"))
        {
            // nouvelle phrase : réinitaliser la trace
            trace.clear();
            // séparer éventuellement la phrase de l'enregistrement maître
            relect = cmd.contains("<!--,");
            if (relect)
            {
                QRegExp re = QRegExp("(-phr)(\\d)+_\\d*,([^%]*)<!--,(.*)-->$");
                int pos = re.indexIn(cmd);
                if (pos > -1)
                {
                    cmd = re.cap(1)+"_"+re.cap(3);
                    trace = re.cap(4).split(',');
                }
                ienr = 0;
            }
        }
        // phrases saisies
        else if (cmd == "-nouvPhr")
        {
            setFTrace("saisie");
        }
        // éventuellement, enregistrer la commande dans trace
        else if (!relect)
        {
            ajTrace(cmd);
        }
        // passer la commande à phrase
		phrase->ecoute(cmd);
	}
}

QString MainWindow::catalogue(QString rep)
{
        QString titre = "Choisir une phrase";
        if (rep == "enr")  titre.append(" enregistrée");
        rep = QString("%1/%2").arg(qApp->applicationDirPath()).arg(rep);
        QDir repertoire = QDir(rep);
    	QStringList ls = repertoire.entryList(QDir::Files);
		for (int i=0;i<ls.count();++i)
		{
			QString nf = ls.at(i);
			QString lin;
            QTextStream(&lin) << "<a href=\"f."<<rep<<"/"<<nf <<"\">"<<nf<<"</a>";
			ls[i] = lin;
		}
        QString page;
        QTextStream(&page)
            << Chaines::titrePraelector
            << Chaines::menu
            << "<hr/>"
            << titre << "<br/>"
            << ls.join("<br/>");
        return page;
}

void MainWindow::clav()
{
    clavier = !clavier;
    if (clavier)
    {
        if (texteT.isEmpty()) ajTouches();
        textBrowser->setHtml(texteT);
    }
    else
    {
        textBrowser->setHtml(texte);
    }
    prefixe.clear();
}

QString MainWindow::choixPhr(QString c)
{
	QFile f(c);
	f.open (QIODevice::ReadOnly|QIODevice::Text);
	QTextStream fluxD (&f);
	QStringList lp;
	int i=1;
    _phrases.clear();
	while (!fluxD.atEnd ())
	{
		QString lin = fluxD.readLine().simplified ();
        if (lin.isEmpty()) continue;
		QString p;
		// commentaires
		if (lin.startsWith("!"))
		{
			lin.remove (0, 1);
			QTextStream (&p) << lin;
		}
		else
        {
            // TODO vérifier lien vide
            QString aff;
            aff = lin;
            QString url = QUrl::toPercentEncoding(lin);
            QTextStream fl(&p);
            fl << "<a href=\"-phr"<<i<<"_"<<url<<"\">"<<i<<"-</a> "<< aff;
            QString phr;
            QTextStream(&phr)<<"-phr"<<i<<"_"<<url;
            _phrases.append(phr);
            ++i;
        }
		lp.append (p);
	}
	f.close ();
    QString page;
    QTextStream(&page)
        << Chaines::titrePraelector
        << Chaines::menu
        << "<hr/>"
        << lp.join("<br/>\n")
        << "<hr/>"
        << Chaines::menu;

    return page;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
# ifdef TRACEREQ
    phrase->traceReq();
# endif
    QSettings settings("Collatinus", "praelector");
    settings.beginGroup("fenetre");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();
    settings.beginGroup("interface");
    settings.setValue("zoom", textBrowser->font().pointSize());
    settings.endGroup();
    QMainWindow::closeEvent(event);
}

/**
 * \fn void MainWindow::enr()
 * \brief enregistre la trace de la lecture courante dans le répertoire
 *  ./enr/, à la bonne place dans un fichier homonyme de suffixe .enr.
 */
void MainWindow::enr()
{
    QString vest;
    QTextStream flv(&vest);
    flv << phrase->num()<<","<<phrase->gr() << "<!--," << trace.join(',')
        << "," << phrase->tr(false)<<"-->";

    // si le fichier trace n'existe pas
    if (!fTrace.exists())
    {
        fTrace.open(QIODevice::WriteOnly);
        QTextStream (&fTrace)<< vest << "\n";;
        fTrace.close();
        return;
    }

    fTrace.open(QIODevice::ReadOnly);
    // lire les lignes
    QTextStream fl(&fTrace);
    bool ins = false;
    QStringList lignes;
    QStringList neotrace;
    int ne = phrase->num().toInt();   // numéro de la trace à enregistrer
    QString lin;
    do
    {
        // chercher la phrase par son numéro
        lin = fl.readLine().simplified();
        if (lin.startsWith('!'))
        {
            neotrace.append(lin);
            ins = true;
            continue;
        }
        QString n = lin.section(',',0,0);
        int nl = n.toInt();      // numéro de la ligne lue
        // si la trace à enregistrer existe, l'écraser
        if (ne == nl && !ins)
        {
            neotrace.append(vest);
            ins = true;
            continue;
        }
        // si son n° est inférieur à la ligne lue, l'insérer
        if (ne < nl && !ins)
        {
            neotrace.append(vest);
            ins = true;
        }
        // puis enregistrer la ligne lue
        if (!lin.isEmpty()) neotrace.append(lin);
    }
    while (!lin.isNull());

    // si la trace n'est toujours pas enregistrée il faut l'ajouter à la fin.
    if (!ins)
    {
        neotrace.append(vest);
    }
    fTrace.close();

    // enregistrer neotrace
    fTrace.open(QIODevice::WriteOnly);
    QTextStream flneo(&fTrace);
    for (int i=0;i<neotrace.count();++i)
        flneo << neotrace.at(i)<<'\n';
    fTrace.close();
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    QString t = ev->text();
    QString prfs = "wxyz";
    if (t.isEmpty())
    {
        QMainWindow::keyPressEvent(ev);
        prefixe.clear();
        return;
    }
    if (ev->key() == Qt::Key_Escape)
    {
        prefixe.clear();
    }
    else if (ev->key() == Qt::Key_Slash && prefixe.isEmpty())
    {
        clav();
    }
    else if (prfs.contains(t) && prefixe.isEmpty())
    {
        prefixe = t;
    }
    else if (clavier && (urls.contains(t)
             || (!prefixe.isEmpty() && prfs.contains(t))))
    {
        t.prepend(prefixe);
        QUrl url = urls[t];
        textBrowser->emet(url);
        prefixe.clear();
    }
    else QMainWindow::keyPressEvent(ev);
}

void MainWindow::lisSettings()
{
    QSettings settings("Collatinus", "praelector");
    // état de la fenêtre
    settings.beginGroup("fenetre");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
    settings.beginGroup("interface");
    QFont font;
    font.setPointSize(settings.value("zoom").toInt());
    textBrowser->setFont(font);
    settings.endGroup();
}

void MainWindow::parle(QString m)
{
    texte = m;
    if (relect)
    {
        surligne();
    }
    if (clavier)
    {
        ajTouches();
	    textBrowser->setHtml(texteT);
    }
    else textBrowser->setHtml(texte);
}

void MainWindow::setFTrace(QString nf)
{
    QString nft;
    QTextStream(&nft) << qApp->applicationDirPath()
        << "/enr/"
        << QFileInfo(nf).baseName()
        <<".prae";
    fTrace.setFileName(nft);
}

void MainWindow::surligne()
{
    int from = texte.indexOf("<a href=", 0);
    while (from > -1)
    {
        from += 9;
        int to = texte.indexOf("\"", from);
        QString url = texte.mid(from, to-from);
        if (url.startsWith("m.e.")
            && trace.at(ienr).startsWith("m.e.")
            && trace.at(ienr).count(".") > 2)
        {
            texte.replace(from, url.length(), trace.at(ienr));
            url = trace.at(ienr);
            to = texte.indexOf("\"", from);
        }
        if (url == trace.at(ienr))
        {
            // TODO : modifier l'url dans la variable texte
            texte.insert(to+2, "<span style=\"background-color:red;\">");
            to = texte.indexOf("</a>", to);
            texte.insert(to, "</span>");
        }
        from = texte.indexOf("<a href=", to);
    }
}

void MainWindow::traceMf(QString t)
{
    QString ultc = trace.at(trace.count()-1);
    // remplacer le 3ème champ par la saisie
    if (ultc.at(ultc.count()-1).isDigit()) ultc.append("."+t);
    else ultc.replace(QRegExp("\\.[^.]*$"), "."+t);
    trace.removeLast();
    trace.append(ultc);
}
