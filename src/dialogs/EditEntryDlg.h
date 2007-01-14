/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#ifndef EDITENTRYDLG_H
#define EDITENTRYDLG_H
#include "ui_EditEntryDlg.h"
#include <QPixmap>
#include <QShowEvent>
#include "main.h"
#include "StandardDatabase.h"

class CEditEntryDlg : public QDialog, public Ui_EditEntryDialog
{
	Q_OBJECT
	public:
		CEditEntryDlg(IDatabase* _db, IEntryHandle* _entry,QWidget* parent = 0,  bool modal = FALSE, Qt::WFlags fl = 0);
		~CEditEntryDlg();
		void InitGroupComboBox();
		void InitIconComboBox();
		static void saveAttachment(IEntryHandle* pEntry, QWidget* ParentWidget=NULL);
		
		IEntryHandle* entry;
		IDatabase* db;
		bool ModFlag;
		int GroupIndex;
		QList<IGroupHandle*> groups;
		QPixmap BannerPixmap;	
	
	public slots:
		void OnPasswordwLostFocus();
		void OnPasswordwTextChanged(const QString&);
		void OnPasswordTextChanged(const QString&);
		void ChangeEchoMode();
		void OnButtonCancel();
		void OnNewAttachment();
		void OnDeleteAttachment();
		void OnSaveAttachment();
		void OnButtonGenPw();
		void OnCheckBoxExpiresNeverChanged(int state);
		void OnCustomIcons();
		void OnButtonOK();
		
	private:		
		virtual void showEvent(QShowEvent *);
		virtual void paintEvent(QPaintEvent*);
		// Added resizeEvent
		virtual void resizeEvent(QResizeEvent *);
	
	

};

#endif

