/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QLabel>
#include <QTime>
#include <qapplication.h>
#include <QShowEvent>
#include <QTranslator>
#include <QDropEvent>
#include <QListWidget>
#include <QTreeWidget>
#include <QClipboard>
#include <QTimer>
#include <QToolButton>
#include <QSystemTrayIcon>

#include "StandardDatabase.h"
#include "PwmConfig.h"
#include "lib/EntryView.h"
#include "lib/GroupView.h"

#include "ui_MainWindow.h"

class KeepassMainWindow : public QMainWindow, public Ui_MainWindow{
	Q_OBJECT
	public:
		KeepassMainWindow (const QString& ArgFile,QWidget *parent=0, Qt::WFlags flags=0);
		IDatabase* db;
	bool Start;
	
	signals:
		void entryChanged();
	
	private slots:
		void OnFileNewKdb();
		void OnFileNewKxdb();
		void OnFileOpen();
		void OnFileClose();
		bool OnFileSave();
		bool OnFileSaveAs();
		void OnFileSettings();
		void OnFileChangeKey();
		void OnFileExit();
		void OnImportFromPwm();
		void OnImportFromKWalletXml();
		void OnExportToTxt();
		void OnEditOpenUrl();
		void OnSearch();
		void OnGroupSearch();
		void OnViewShowToolbar(bool);
		void OnViewShowEntryDetails(bool);
		void OnViewToolbarIconSize16(bool);
		void OnViewToolbarIconSize22(bool);
		void OnViewToolbarIconSize28(bool);
		void OnGroupSelectionChanged(IGroupHandle*);
		void OnQuickSearch();
		void OnColumnVisibilityChanged(bool show);
		void OnUsernPasswVisibilityChanged(bool hide);
		void OnFileModified();
		void OnExtrasSettings();
		void OnExtrasPasswordGen();
		void OnHelpAbout();
		void OnHelpHandbook();
		void OnItemExpanded(QTreeWidgetItem*);
		void OnItemCollaped(QTreeWidgetItem*);
		void OnShowSearchResults();
		void OnEntryChanged(SelectionState);
		void OnSysTrayActivated(QSystemTrayIcon::ActivationReason);
	
	private:
		void closeEvent(QCloseEvent* event);
		SelectionState GroupSelection, EntrySelection;
		bool FileOpen;
		bool ModFlag;
		QList<quint32> SearchResults;
		inline void setupToolbar();
		inline void setupIcons();
		inline void setupConnections();
		inline void setupMenus();
		void setStateFileOpen(bool);
		void setStateFileModified(bool);
		void setStateGroupSelected(SelectionState s);
		void setStateEntrySelected(SelectionState s);
		void openDatabase(QString filename,bool IsAuto=false);
		void setupDatabaseConnections(IDatabase* DB);
		bool closeDatabase();
		void search(IGroupHandle* Group);
		void removeFromSearchResults(int sID);
		void updateDetailView();
		QLineEdit* QuickSearchEdit;
		QLabel* StatusBarGeneral;
		QLabel* StatusBarSelection;
		QToolBar* toolBar;
		QSystemTrayIcon* SysTray;
		QAction* ViewShowToolbarAction;
		QMenu* SysTrayMenu;
		bool ShutingDown;
};

#endif
