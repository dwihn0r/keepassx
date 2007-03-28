/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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

#ifndef _ENTRY_VIEW_H_
#define _ENTRY_VIEW_H_

#include <QMenu>
#include <QTreeWidget>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QTimer>
#include <QClipboard>
#include <QVarLengthArray>
#include "../StandardDatabase.h"

#define NUM_COLUMNS 11

class EntryViewItem;
enum SelectionState{NONE,SINGLE,MULTIPLE,SEARCHGROUP};

class KeepassEntryView:public QTreeWidget{
	Q_OBJECT
	public:
		KeepassEntryView(QWidget* parent=0);
		~KeepassEntryView();
		void showSearchResults();
		void showGroup(IGroupHandle* group);
		void updateColumns();
		void refreshItems();
		int columnListIndex(int LogicalIndex);
		IDatabase* db;
		QList<EntryViewItem*>Items;
		QList<IEntryHandle*> SearchResults;
		QMenu *ContextMenu;
		QVarLengthArray<bool>Columns;
		void setCurrentEntry(IEntryHandle* entry);	
	private:
		void setEntry(IEntryHandle* entry);
		void updateEntry(EntryViewItem*);
		void editEntry(EntryViewItem*);
		void createItems(QList<IEntryHandle*>& entries);
		int logicalColIndex(int ListIndex);
		
		QClipboard* Clipboard;
		QTimer ClipboardTimer;		
		void resizeColumns();
		bool AutoResizeColumns;
		QPoint DragStartPos;
		QList<QTreeWidgetItem*> DragItems;
		QPixmap DragPixmap;
		IGroupHandle* CurrentGroup;
		enum EntryViewMode {Normal, ShowSearchResults};
		EntryViewMode ViewMode;
		QVarLengthArray<float>ColumnSizes;
		QVarLengthArray<int>ColumnOrder;
		float GroupColumnSize;
		
		virtual void contextMenuEvent(QContextMenuEvent *event);
		virtual void paintEvent(QPaintEvent* event);
		virtual void resizeEvent(QResizeEvent* event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
	public slots:
		void OnColumnResized(int index,int OldSize, int NewSize);
		void OnHeaderSectionClicked(int index);
		void OnGroupChanged(IGroupHandle* group);
		void OnShowSearchResults();
		void OnEntryActivated(QTreeWidgetItem*,int);
		void OnNewEntry();
		void OnItemsChanged();
		void updateIcons();
		void OnUsernameToClipboard();
		void OnPasswordToClipboard();
		void OnEditEntry();
		void OnClipboardTimeOut();
		void OnCloneEntry();
		void OnDeleteEntry();
		void OnSaveAttachment();
		void OnAutoType();
		void removeDragItems();
		void OnColumnMoved(int LogIndex,int OldVisIndex,int NewVisIndex);
		void OnEditOpenUrl();
	signals:
		void fileModified();
		void selectionChanged(SelectionState);
};


class EntryViewItem:public QTreeWidgetItem{
	public:
		EntryViewItem(QTreeWidget *parent);
		EntryViewItem(QTreeWidget *parent, QTreeWidgetItem * preceding);
		EntryViewItem(QTreeWidgetItem *parent);
		EntryViewItem(QTreeWidgetItem *parent, QTreeWidgetItem * preceding);
		IEntryHandle* EntryHandle;
		virtual bool operator<(const QTreeWidgetItem& other)const;
};


#endif
