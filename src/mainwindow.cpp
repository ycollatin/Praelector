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

#include "mainwindow.h"
#include "phrase.h"

Editeur::Editeur(QWidget *parent) : QTextBrowser(parent)
{
}

void Editeur::emet(QUrl url)
{
    emit anchorClicked(url);
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

    //textBrowser = new QTextBrowser(centralWidget);
    textBrowser = new Editeur(centralWidget);
    verticalLayout->addWidget(textBrowser);

    setCentralWidget(centralWidget);
    //retranslateUi(MainWindow);

	phrase = new Phrase("");
	connect(phrase, SIGNAL(repondu(QString)),this,SLOT(parle(QString)));
	connect(textBrowser, SIGNAL(anchorClicked(QUrl)),this, SLOT(calcul(QUrl)));
	textBrowser->setOpenLinks(false);
    clavier = false;
    alphabet = "abcdefghijklmnopqrstuv";
    wxyz = "wxyz";
    // construire la liste des clés alpha
    for (int i=0;i<alphabet.count();++i)
        clesL.append(alphabet.at(i));
    for (int i=0;i<4;++i)
    {
        QChar prem = wxyz.at(i);
        for (int j=0;j<alphabet.count();++j)
        {
            QString cle;
            cle.append(prem);
            cle.append(alphabet.at(j));
            clesL.append(cle);
        }
    }
    qDebug()<<"clesL";
    qDebug()<<clesL.count();
	phrase->ecoute ("");
}

MainWindow::~MainWindow()
{
}

void MainWindow::ajTouches()
{
    if (texte.isEmpty() || !texteT.isEmpty()) return;
    QString lien = "<span style=\"color:red;\">%1</a></span> ";
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
                //urls[cle] = QUrl(url);
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

/*
 * MainWindow::calcul (Qurl url) traite l'hyperlien cliqué.
 * Il le renvoie si nécessaire à la méhtode homonyme de Phrase.
 *
 */
void MainWindow::calcul (QUrl url)
{
    texte.clear();
    texteT.clear();
	QString cmd = url.toString ();
	if (cmd == "-quitter") 
	{
		// TODO : détruire MainWindow (et donc phrase), mais pê dans main.cpp.
		close ();
	}
	else if (cmd == "-aide")
	{
		//textBrowser->setHtml (Chaines::documentation);
		parle(Chaines::documentation);
	}
	else if (cmd == "-corpus")
	{
		// charger la liste des fichiers du rép. ./corpus/*
    	QDir repertoire = QDir (qApp->applicationDirPath () +"/corpus", "*");
    	QStringList corpus = repertoire.entryList (QDir::Files);
		for (int i=0;i<corpus.count();++i)
		{
			QString nf = corpus.at (i);
			QString lin;
			QTextStream (&lin) << "<a href=\"@" <<nf <<"\">"<<nf<<"</a>";
			corpus[i] = lin;
		}
		QString page = corpus.join ("<br/>");
		page.prepend ("<br/><a href=\"-nouvPhr\">Saisir une phrase</a><br/>"
					 "<a href=\"-init\">annuler</a>&nbsp;"
					 "<a href=\"-quitter\">quitter</a><hr/>");
        parle(page);
	}
	else if (cmd.startsWith("@"))
	{
        parle(choixPhr(cmd));
		//textBrowser->setHtml(choixPhr(cmd));
	}
	else 
	{
		phrase->ecoute(cmd);
	}
}

QString MainWindow::choixPhr (QString c)
{
	const QString menu ( 
		 "<a href=\"-corpus\">choisir une phrase</a> "
		 "<a href=\"-nouvPhr\">Saisir une phrase</a> "
         "<a href=\"-init\">annuler</a><br/>");
	c.remove (0,1);
	QFile f (qApp->applicationDirPath ()+"/corpus/"+c);
	f.open (QIODevice::ReadOnly|QIODevice::Text);
	QTextStream fluxD (&f);
	QStringList lp;
	lp.append (menu);
	int i=1;
	while (!fluxD.atEnd ())
	{
		QString lin = fluxD.readLine().simplified ();
		QString p;
		// commentaires
		if (lin.startsWith("!"))
		{
			lin.remove (0, 1);
			QTextStream (&p) << lin;
		}
		else QTextStream (&p) << "<a href=\"-phr-"<<lin<<"\">"<<i++<<"-</a> "<< lin;
		lp.append (p);
	}
	f.close ();
	lp.append (menu);
	return lp.join ("<br/>");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    phrase->trace();
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    QString t = ev->text();
    if (t.isEmpty())
    {
        QMainWindow::keyPressEvent(ev);
        prefixe.clear();
        return;
    }
    if (ev->key() == Qt::Key_Slash)
    {
        clavier = !clavier;
        if (clavier)
        {
            if (texteT.isEmpty()) ajTouches();
            textBrowser->setText(texteT);
        }
        else 
        {
            textBrowser->setText(texte);
        }
        prefixe.clear();
    }
    else if (wxyz.contains(t))
    {
        prefixe = t;
    }
    else if (clavier && clesL.contains(t))
    {
        t.prepend(prefixe);
        QUrl url = urls[t];
        textBrowser->emet(url); 
        prefixe.clear();
    }
    QMainWindow::keyPressEvent(ev);
}

void MainWindow::parle(QString m)
{
    texte = m;
    if (clavier)
    {
        ajTouches();
	    textBrowser->setHtml(texteT);
    }
    else textBrowser->setHtml(texte);
}
