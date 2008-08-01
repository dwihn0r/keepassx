/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *

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
 
#ifndef GENPWDIALOG_H
#define GENPWDIALOG_H

#include "ui_PasswordGenDlg.h"
#include "EditEntryDlg.h"

class CGenPwDialog : public QDialog, public Ui_GenPwDlg
{
	Q_OBJECT
	public:
		CGenPwDialog(QWidget* parent, bool StandAloneMode, Qt::WFlags fl = 0);
		~CGenPwDialog();
	
	private:
		int AddToAssoctable(char* table,int start,int end,int pos);
		bool trim(unsigned char &value,int range);
		void paintEvent(QPaintEvent* event);
		void generatePasswordInternal(char* buffer, int length);
		QPixmap BannerPixmap;
		static bool EntropyCollected;
		QPushButton* AcceptButton;
	
	private slots:
		void OnGeneratePw();
		void OnRadio2StateChanged(bool);
		void OnRadio1StateChanged(bool);
		void OnCancel();
		void OnAccept();
		void estimateQuality();
		void OnCollectEntropyChanged(int);
		void OnCharsChanged(const QString& str);
		void SwapEchoMode();
		void setGenerateEnabled();
		void setAcceptEnabled(const QString& str);
};

#endif

