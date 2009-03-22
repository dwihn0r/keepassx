/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek.saidi@arcor.de                                                  *
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

#ifndef _ENTRY_VIEW_H_
#define _ENTRY_VIEW_H_

#include "Kdb3Database.h"

#define NUM_COLUMNS 11

class EntryViewItem;
class GroupViewItem;
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
		QBitArray Columns;
		void setCurrentEntry(IEntryHandle* entry);
		inline IGroupHandle* getCurrentGroup() { return CurrentGroup; };
	private:
		//void setEntry(IEntryHandle* entry);
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
		IGroupHandle* CurrentGroup;
		enum EntryViewMode {Normal, ShowSearchResults};
		EntryViewMode ViewMode;
		QList<int> ColumnSizes;
		QList<int> ColumnOrder;
		float GroupColumnSize;

		void contextMenuEvent(QContextMenuEvent *event);
		void paintEvent(QPaintEvent* event);
		void resizeEvent(QResizeEvent* event);
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
	
	private slots:
		void OnColumnResized();
		void OnHeaderSectionClicked(int index);
		void OnGroupChanged(IGroupHandle* group);
		void OnShowSearchResults();
		void OnEntryActivated(QTreeWidgetItem*,int);
		void OnEntryDblClicked(QTreeWidgetItem*,int);
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
#ifdef AUTOTYPE
		void OnAutoType();
#endif
		void removeDragItems();
		void OnColumnMoved();
		void OnEditOpenUrl();
		void OnEditCopyUrl();
	
	signals:
		void fileModified();
		void selectionChanged(SelectionState);
		void requestCreateGroup(QString title, quint32 image, GroupViewItem* parent);
};


class EntryViewItem:public QTreeWidgetItem{
	public:
		EntryViewItem(QTreeWidget *parent);
		EntryViewItem(QTreeWidget *parent, QTreeWidgetItem * preceding);
		EntryViewItem(QTreeWidgetItem *parent);
		EntryViewItem(QTreeWidgetItem *parent, QTreeWidgetItem * preceding);
		IEntryHandle* EntryHandle;
		bool operator<(const QTreeWidgetItem& other) const;
	private:
		int compare(const QTreeWidgetItem& other, int col, int index) const;
};


#endif
