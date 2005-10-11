/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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
#ifndef _LISTVIEWS_H_
#define _LISTVIEWS_H_

#include <qlistview.h>
#include <qmime.h>
#include "PwManager.h"

class GroupItem: public QListViewItem{
public:
CGroup* pGroup;
virtual bool acceptDrop(const QMimeSource * mime);
///////////////////////////////////////////////////////////////////////
    GroupItem(CGroup*, QListView * parent );
    GroupItem(CGroup*, QListViewItem * parent );
    GroupItem(CGroup*, QListView * parent, QListViewItem * after );
    GroupItem(CGroup*, QListViewItem * parent, QListViewItem * after );

    GroupItem(CGroup*, QListView * parent,
		   QString,    QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   GroupItem(CGroup*, QListViewItem * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   GroupItem(CGroup*, QListView * parent, QListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

    GroupItem(CGroup*, QListViewItem * parent, QListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
   ~GroupItem();
/////////////////////////////////////////////////////////////////////////
};


class EntryItem: public QListViewItem{
public:
CEntry* pEntry;
///////////////////////////////////////////////////////////////////////
    EntryItem(CEntry*, QListView * parent );
    EntryItem(CEntry*, QListViewItem * parent );
    EntryItem(CEntry*, QListView * parent, QListViewItem * after );
    EntryItem(CEntry*, QListViewItem * parent, QListViewItem * after );

    EntryItem(CEntry*, QListView * parent,
		   QString,    QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   EntryItem(CEntry*, QListViewItem * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   EntryItem(CEntry*, QListView * parent, QListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

    EntryItem(CEntry*, QListViewItem * parent, QListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
/////////////////////////////////////////////////////////////////////////
};


class CGroupView: public QListView{
public:
  CGroupView(QWidget * parent = 0, const char * name = 0, WFlags f = 0 );


};

#endif