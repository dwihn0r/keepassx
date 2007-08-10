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
#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H
#include "ui_PasswordDlg.h"
#include "main.h"
#include "lib/UrlLabel.h"
#include "Database.h"


class CPasswordDialog : public QDialog, public Ui_PasswordDlg
{
 	Q_OBJECT

	private:
		bool Mode_Set; //true = Set, false = Get
		IDatabase* db;
		void setStatePasswordOnly();
		void setStateKeyFileOnly();
		void setStateBoth();
		bool doAuth();
	
	public:
		QString keyfile;
		QString password;
		tKeyType KeyType;
		bool OverwriteKeyFile;	
		CPasswordDialog(QWidget* parent,IDatabase* DB,bool ShowExitButton = false, bool KeyMode_Set=false);
	
	public slots:
	    void OnOK();
		void OnOK_Set();
	    void OnCancel();
	    void OnButtonBrowse();
		void OnButtonBrowse_Set();
	    void OnButtonExit();
	    void OnPasswordChanged(const QString &txt);
	    void OnCheckBox_BothChanged(int state);
	    void ChangeEchoModeDatabaseKey();
	    void OnComboTextChanged(const QString&);
};

#endif

