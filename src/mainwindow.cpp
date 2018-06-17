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

Editeur::Editeur(QWidget *parent) : QWebView(parent)
{
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

	phrase = new Phrase("");
	connect(phrase, SIGNAL(repondu(QString)),this,SLOT(parle(QString)));
	connect(textBrowser, SIGNAL(linkClicked(QUrl)),this, SLOT(calcul(QUrl)));
    clavier = false;
    alphabet = "abcdefghijklmnopqrstuv";
    wxyz = "wxyz";
    lurl = QStringList()
    << "-prec"
    << "-suiv"
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
	phrase->ecoute ("");
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
	QString cmd = url.toString ();
	if (cmd == "-quitter") 
	{
		// TODO : détruire MainWindow (et donc phrase), mais pê dans main.cpp.
		close ();
	}
	else if (cmd == "-aide")
	{
		parle(Chaines::documentation);
	}
	else if (cmd == "-corpus" || cmd == "-enr")
	{
        qDebug()<<"corpus, cmd"<<cmd;
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
            relect = cmd.contains("%3C!--,");
            if (relect)
            {
                // -phr1_1,id bene scio.%3C!--,-suiv,-suiv,l.v.28,l.v.26,m.i.3,m.i.3,je le sais bien--%3E
                //QRegularExpressionMatch rem = QRegularExpression("(-phr\\d+_),([]*)%3C!--,(.*)--%3E$").match(cmd);
                //cmd = rem.captured(1)+rem.captured(2);
                //trace = rem.captured(3).split(',');
                QRegExp re = QRegExp("(-phr)(\\d)+_\\d*,([^%]*)%3C!--,(.*)--%3E$");
                int pos = re.indexIn(cmd);
                if (pos > -1)
                {
                    cmd = re.cap(1)+"_"+re.cap(3);
                    trace = re.cap(4).split(',');
                }
            }
        }
        // éventuellement, enregistrer la commande dans trace
        else if (!relect) (ajTrace(cmd));
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
		QString page = ls.join ("<br/>");
        page.prepend(Chaines::menu);
        return page;
}

QString MainWindow::choixPhr(QString c)
{
	c.remove (0,1);
	QFile f(c);
	f.open (QIODevice::ReadOnly|QIODevice::Text);
	QTextStream fluxD (&f);
	QStringList lp;
	lp.append(Chaines::menu);
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
            // TODO vérifier lin vide
            QString aff;
            aff = lin;
            QTextStream fl(&p);
            fl << "<a href=\"-phr"<<i<<"_"<<aff<<"\">"<<i<<"-</a> "<< aff;
            ++i;
        }
		lp.append (p);
	}
	f.close ();
	lp.append (Chaines::menu);
	return lp.join ("<br/>\n");
}

void MainWindow::enr()
{
    trace.prepend(phrase->gr()+"<!--");
    trace.prepend(phrase->num());
    trace.append(phrase->tr()+"-->");
    // joindre les cmd
    QString vest = trace.join(',');
    while (vest.endsWith('\n')) vest.chop(1);
    // chercher le fichier trace
    if (!fTrace.exists())
    {
        fTrace.open(QIODevice::WriteOnly);
        QTextStream (&fTrace) << vest << "\n";;
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
        neotrace.append(lin);
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
        prefixe.clear();
        return;
    }
    if (ev->key() == Qt::Key_Slash)
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
    qDebug()<<"parle, texte"<<texte;
    qDebug()<<"clavier:"<<clavier;
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
