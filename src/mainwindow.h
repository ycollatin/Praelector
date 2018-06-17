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
//#include <QTextBrowser>
#include <QWebView>
#include <QVBoxLayout>
#include <QWidget>

#include <QUrl>
#include <phrase.h>

class Editeur : public QWebView
{
    Q_OBJECT

    public:
        Editeur(QWidget *parent = Q_NULLPTR);
        void emet(QUrl url);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    //QTextBrowser *textBrowser;
    Editeur* textBrowser;

	public:
    	explicit MainWindow(QWidget *parent = 0);
    	~MainWindow();

	private:
        QString            alphabet;
        QStringList        clesL;
        bool               clavier;
        QFile              fTrace;
        QStringList        lenr;
        QStringList        lurl;
		Phrase*            phrase;
        QString            prefixe;
        QString            texte;
        QString            texteT; // texte avec reccourcis
        QStringList        trace;
        QMap<QString,QUrl> urls;
        QString            wxyz;

        void               ajTouches();
        void               ajTrace(QString cmd);
        QString            catalogue(QString rep);
		QString            choixPhr(QString cmd);
        void               enr();
        bool               relect;
        void               setFTrace(QString nf);

    protected:
        void keyPressEvent(QKeyEvent *ev);

	private slots:
		void calcul (QUrl url);
        void closeEvent(QCloseEvent* event);
		void parle (QString m);
};

#endif // MAINWINDOW_H
