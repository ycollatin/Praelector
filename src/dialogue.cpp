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

#include <QFont>
#include <dialogue.h>
#include <QDebug>


Dialogue::Dialogue ()
{
    resize(400, 300);
    QFont font;
    font.setPointSize(14);
    setFont(font);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);

    label = new QLabel(this);
    verticalLayout->addWidget(label);

    plainTextEdit = new QPlainTextEdit(this);
	plainTextEdit->setTabChangesFocus(true);
	plainTextEdit->installEventFilter (this);
    verticalLayout->addWidget(plainTextEdit);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);

	setModal (true);

	connect (buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
	connect (buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}

bool Dialogue::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == plainTextEdit)
	{
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
			{
				accept();
            	return true;
			}
        } else {
            return false;
        }
    }
	return QDialog::eventFilter(obj, event);
}

QString Dialogue::getText ()
{
	return plainTextEdit->toPlainText ();
}

void Dialogue::setLabel (QString l)
{
	label->setText (l);
}

void Dialogue::setText (QString t)
{
	plainTextEdit->setPlainText (t);
	plainTextEdit->selectAll ();
}

