/*          mainwindow.h     */

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include <QUrl>
#include <phrase.h>



class Editeur : public QTextBrowser
{
    Q_OBJECT

    public:
        Editeur(QWidget *parent = Q_NULLPTR);
        void emet(QUrl url);
};

class Raccourcis : public QObject
{
    Q_OBJECT

    private:
        QString alpha;
        QString alpha2;
        QString wxyz;
        QString a;
        QString a2;
        QString z;
    public:
        Raccourcis();
        void    reinit();
        QString racc(QString url);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    private:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    Editeur* textBrowser;

	public:
    	explicit MainWindow(QWidget *parent = 0);
    	~MainWindow();

	private:
        //QString            alphabet;
        //QString            alpha2;
        //QStringList        clesL;
        bool               clavier;
        QFont              font;
        QString           _fCorpus;
        QFile              fTrace;
        int                ienr;
        QStringList        lenr;
        QStringList        lurl;
        int               _nbphr;
		Phrase*            phrase;
        QStringList       _phrases;
        QString            prefixe;
        QString            texte;
        QString            texteT; // texte avec reccourcis
        QStringList        trace;
        QMap<QString,QUrl> urls;
        QString            wxyz;
        Raccourcis*        raccourcis;

        void               ajTouches();
        void               ajTrace(QString cmd);
        QString            catalogue(QString rep);
        void               clav();
		QString            choixPhr(QString cmd);
        void               enr();
        void               lisSettings();
        bool               relect;
        void               setFTrace(QString nf);
        void               surligne();

    protected:
        void keyPressEvent(QKeyEvent *ev);

	private slots:
		void calcul (QUrl url);
        void closeEvent(QCloseEvent* event);
		void parle (QString m);
        void traceMf(QString t);
};

#endif // MAINWINDOW_H
