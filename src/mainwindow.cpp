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

#include <QDebug>
#include <QDir>
#include <QWebPage>
#include <QtWebKitWidgets>

#include "mainwindow.h"
#include "phrase.h"

Editeur::Editeur(QWidget *parent) : QWebView(parent)
{
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

void Editeur::emet(QUrl url)
{
    emit linkClicked(url);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(834, 734);
    centralWidget = new QWidget();
    QFont font;
    font.setPointSize(14);
	centralWidget->setFont (font);

    verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);

    textBrowser = new Editeur(centralWidget);
    verticalLayout->addWidget(textBrowser);

    setCentralWidget(centralWidget);

	phrase = new Phrase();
	connect(phrase, SIGNAL(repondu(QString)),this,SLOT(parle(QString)));
	connect(phrase, SIGNAL(editTr(QString)),this,SLOT(traceMf(QString)));
	connect(textBrowser, SIGNAL(linkClicked(QUrl)),this, SLOT(calcul(QUrl)));
    clavier = false;
    relect = false;
    lurl = QStringList()
    << "-prec"
    << "-suiv"
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
    alphabet = "ces/aqr<>bdfghijklmnoptuv";
    /*
       c choisir une phrase
       e " enregistrée
       s saisir une phrase
       / clavier
       a annuler
       q quitter
       r réinitialiser
       < mot précédent
       < mot suivant
     */
    wxyz = "wxyz";
    for (int i=0;i<alphabet.count();++i)
        clesL.append(alphabet.at(i));
    for (int i=0;i<4;++i)
    {
        QString cle(wxyz.at(i));
        for (int j=0;j<alphabet.count();++j)
        {
            clesL.append(cle+alphabet.at(j));
        }
        for (int j=0;j<wxyz.count();++j)
        {
            clesL.append(cle+wxyz.at(j));
        }
    }
	phrase->ecoute("-init");
}

MainWindow::~MainWindow()
{
}

void MainWindow::ajTouches()
{
    if (texte.isEmpty() || !texteT.isEmpty()) return;
    QString lien = "<span style=\"color:red;font-size:small;vertical-align:bottom;\">%1</a></span>";
    int i = 0;
    QString t = texte;
    QTextStream fl(&texteT);
    while (!t.isEmpty())
    {
        if (t.startsWith("<a href"))
        {
            QString url;
            t.remove(0,9);
            while (t.at(0) != '"')
            {
                url.append(t.at(0));
                t.remove(0,1);
            }
            if (i < clesL.count())
            {
                QString cle = clesL.at(i);
                fl << lien.arg(cle);
                urls.insert(cle, url);
                ++i;
            }
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

/*
 * MainWindow::calcul (Qurl url) traite l'hyperlien cliqué.
 * Il le renvoie si nécessaire à la méhtode homonyme de Phrase.
 *
 */
void MainWindow::calcul (QUrl url)
{
    texte.clear();
    texteT.clear();
    QString cmd = url.fromPercentEncoding(url.toString().toUtf8());
    // lecture d'une traduction éditée
    if (relect && cmd == trace.at(ienr)) ++ienr;
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
        cmd.remove(0,1);
		// charger la liste des fichiers du rép.
        QString page = catalogue(cmd);
        parle(page);
	}
	else if (cmd.startsWith("@"))
	{
        lenr.clear();
        parle(choixPhr(cmd));
        QString nf = cmd.remove(0,1);
        setFTrace(nf);
	}
    else if (cmd.startsWith("-trace"))
    {
        enr();
    }
	else 
	{
        // nouvelle phrase : réinitaliser la trace 
        if (cmd.startsWith("-phr"))
        {
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
        rep = QString("%1/%2").arg(qApp->applicationDirPath()).arg(rep);
        QDir repertoire = QDir(rep);
    	QStringList ls = repertoire.entryList(QDir::Files);
		for (int i=0;i<ls.count();++i)
		{
			QString nf = ls.at(i);
			QString lin;
            QTextStream(&lin) << "<a href=\"@"<<rep<<"/"<<nf <<"\">"<<nf<<"</a>";
			ls[i] = lin;
		}
        QString page;
        QTextStream(&page)
            << Chaines::titrePraelector
            << Chaines::menu
            << "<hr/>"
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
	c.remove (0,1);
	QFile f(c);
	f.open (QIODevice::ReadOnly|QIODevice::Text);
	QTextStream fluxD (&f);
	QStringList lp;
	int i=1;
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
            //url.replace(":","%3A");
            QTextStream fl(&p);
            fl << "<a href=\"-phr"<<i<<"_"<<url<<"\">"<<i<<"-</a> "<< aff;
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    //phrase->traceReq();
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    QString t = ev->text();
    if (t.isEmpty())
    {
        QMainWindow::keyPressEvent(ev);
        //prefixe.clear();
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
    else if (wxyz.contains(t) && prefixe.isEmpty())
    {
        prefixe = t;
    }
    else if (clavier
             && (clesL.contains(t)
                 || (!prefixe.isEmpty() && wxyz.contains(t))))
    {
        t.prepend(prefixe);
        QUrl url = urls[t];
        textBrowser->emet(url); 
        prefixe.clear();
    }
    else QMainWindow::keyPressEvent(ev);
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
    ultc.append('.'+t);
    trace.removeLast();
    trace.append(ultc);
}
