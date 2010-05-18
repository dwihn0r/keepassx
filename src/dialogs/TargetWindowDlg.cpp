/***************************************************************************
 *   Copyright (C) 2008 by Felix Geyer                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "TargetWindowDlg.h"
#include "AutoType.h"

TargetWindowDlg::TargetWindowDlg(QWidget* parent) : QDialog(parent){
	setupUi(this);
	QStringList windowTitles = autoType->getAllWindowTitles();
#ifndef Q_WS_MAC
	// on MacX, titles are in top to bottom order which is better than alpha order so no sort
	windowTitles.sort();
#endif
	for (QStringList::const_iterator i = windowTitles.constBegin(); i != windowTitles.constEnd(); ++i)
		comboWindow->addItem(*i);
	
	connect(buttonBox, SIGNAL(accepted()), SLOT(OnAccept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

void TargetWindowDlg::OnAccept(){
	pWindowTitle = comboWindow->currentText();
	accept();
}
