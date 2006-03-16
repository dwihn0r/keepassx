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
#include "../PwManager.h"

#define NUM_COLUMNS 10

class EntryViewItem;


class KeepassEntryView:public QTreeWidget{
Q_OBJECT
public:
 KeepassEntryView(QWidget* parent=0);
 ~KeepassEntryView();
 void updateItems(unsigned int group);
 void updateColumns();
 void refreshItems();
 void showSearchResults(QList<Q_UINT32>& results);
 PwDatabase* db;
 vector<EntryViewItem*>Items;
 QMenu *ContextMenu;
private:
 void setEntry(CEntry* entry);
 int CurrentGroup;
 QList<float>ColumnSizes;
 void resizeColumns();
 bool AutoResizeColumns;
 QPoint DragStartPos;
 QList<QTreeWidgetItem*> DragItems;
 QPixmap DragPixmap;
 bool IsSearchGroup;
protected:
 virtual void contextMenuEvent(QContextMenuEvent *event);
 virtual void paintEvent(QPaintEvent* event);
 virtual void resizeEvent(QResizeEvent* event);
 virtual void mousePressEvent(QMouseEvent *event);
 virtual void mouseMoveEvent(QMouseEvent *event);
public slots:
 void OnColumnResized(int index,int OldSize, int NewSize);
 void updateItems();
};


class EntryViewItem:public QTreeWidgetItem{
public:
EntryViewItem(QTreeWidget *parent);
EntryViewItem(QTreeWidget *parent, QTreeWidgetItem * preceding);
EntryViewItem(QTreeWidgetItem *parent);
EntryViewItem(QTreeWidgetItem *parent, QTreeWidgetItem * preceding);
CEntry* pEntry;
};


#endif