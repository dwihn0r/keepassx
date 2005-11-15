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

#include <q3listview.h>
#include <qmime.h>
#include "PwManager.h"

class GroupItem: public Q3ListViewItem{
public:
CGroup* pGroup;
virtual bool acceptDrop(const QMimeSource * mime);
///////////////////////////////////////////////////////////////////////
    GroupItem(CGroup*, Q3ListView * parent );
    GroupItem(CGroup*, Q3ListViewItem * parent );
    GroupItem(CGroup*, Q3ListView * parent, Q3ListViewItem * after );
    GroupItem(CGroup*, Q3ListViewItem * parent, Q3ListViewItem * after );

    GroupItem(CGroup*, Q3ListView * parent,
		   QString,    QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   GroupItem(CGroup*, Q3ListViewItem * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   GroupItem(CGroup*, Q3ListView * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

    GroupItem(CGroup*, Q3ListViewItem * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
   ~GroupItem();
/////////////////////////////////////////////////////////////////////////
};


class EntryItem: public Q3ListViewItem{
public:
CEntry* pEntry;
///////////////////////////////////////////////////////////////////////
    EntryItem(CEntry*, Q3ListView * parent );
    EntryItem(CEntry*, Q3ListViewItem * parent );
    EntryItem(CEntry*, Q3ListView * parent, Q3ListViewItem * after );
    EntryItem(CEntry*, Q3ListViewItem * parent, Q3ListViewItem * after );

    EntryItem(CEntry*, Q3ListView * parent,
		   QString,    QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   EntryItem(CEntry*, Q3ListViewItem * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

   EntryItem(CEntry*, Q3ListView * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

    EntryItem(CEntry*, Q3ListViewItem * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
/////////////////////////////////////////////////////////////////////////
};


class CGroupView: public Q3ListView{
public:
  CGroupView(QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0 );


};

#endif