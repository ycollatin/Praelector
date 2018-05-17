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

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;

	public:
    	explicit MainWindow(QWidget *parent = 0);
    	~MainWindow();

	private:
        int const    clz   = 0
        int const    clfl  = 1;
        int const    clreq = 2;
        int          etat;
        virtual void keyPressEvent(QKeyEvent *ev);
        QString      saisie;
		Phrase*      phrase;
		QString      choixPhr (QString cmd);

	private slots:
		void calcul (QUrl url);
        void closeEvent(QCloseEvent* event);
		void parle (QString m);
};

#endif // MAINWINDOW_H
