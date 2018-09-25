/*  dialogue.cpp
 *
 *  This file is part of PRAELECTOR
 *
 *  PRAELECTOR is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  PRAELECTOR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PRAELECTOR; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <QFile>
#include <QFont>
#include <QtCore/QCoreApplication>

#include <aide.h>
#include <QDebug>


Aide::Aide ()
{
    resize(800, 600);
    QFont font;
    font.setPointSize(14);
    setFont(font);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);

    textBrowser = new QTextBrowser(this);
	textBrowser->setTabChangesFocus(true);
	textBrowser->installEventFilter (this);
    verticalLayout->addWidget(textBrowser);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);

    QFile f(qApp->applicationDirPath() + "/doc/auxilium.html");
    f.open(QFile::ReadOnly);
    QTextStream flux(&f);
    flux.setCodec("UTF-8");
    textBrowser->setHtml(flux.readAll());
    f.close();

	setModal (false);
	connect (buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
}
