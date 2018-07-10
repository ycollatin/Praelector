/* dialogue.h
 *
 *  This file is part of PRAELECTOR.
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

#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>

class Dialogue: public QDialog
{
	Q_OBJECT

	private:
    	QVBoxLayout *verticalLayout;
    	QLabel *label;
    	QPlainTextEdit *plainTextEdit;
    	QDialogButtonBox *buttonBox;
	protected:
		bool eventFilter(QObject *obj, QEvent *ev);
	public:
			Dialogue ();
			void    setLabel (QString l);
			void    setText (QString t);
			QString getText ();
};

#endif
