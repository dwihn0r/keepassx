/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.               *
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
#include "ListViews.h"
#include "PwManager.h"
#include <qstring.h>
#include <q3listview.h>



GroupItem::GroupItem(CGroup* group, Q3ListView * parent ) : Q3ListViewItem(parent)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListViewItem * parent ): Q3ListViewItem(parent)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListView * parent, Q3ListViewItem * after ): Q3ListViewItem(parent,after)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListViewItem * parent, Q3ListViewItem * after ): Q3ListViewItem(parent,after)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListView* parent, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,l1,l2,l3,l4,l5,l6,l7,l8)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListViewItem* parent, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,l1,l2,l3,l4,l5,l6,l7,l8)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListView* parent,Q3ListViewItem* after, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,after,l1,l2,l3,l4,l5,l6,l7,l8)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::GroupItem(CGroup* group, Q3ListViewItem* parent,Q3ListViewItem* after, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,after,l1,l2,l3,l4,l5,l6,l7,l8)
{
pGroup=group;
setDropEnabled(true);
}

GroupItem::~GroupItem(){}


bool GroupItem::acceptDrop(const QMimeSource* mime){
qDebug("DropEvent\n");
return true;
}


///////////////////////////////

EntryItem::EntryItem(CEntry* entry, Q3ListView * parent ) : Q3ListViewItem(parent)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListViewItem * parent ): Q3ListViewItem(parent)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListView * parent, Q3ListViewItem * after ): Q3ListViewItem(parent,after)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListViewItem * parent, Q3ListViewItem * after ): Q3ListViewItem(parent,after)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListView* parent, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,l1,l2,l3,l4,l5,l6,l7,l8)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListViewItem* parent, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,l1,l2,l3,l4,l5,l6,l7,l8)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListView* parent,Q3ListViewItem* after, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,after,l1,l2,l3,l4,l5,l6,l7,l8)
{
pEntry=entry;
setDragEnabled(true);
}

EntryItem::EntryItem(CEntry* entry, Q3ListViewItem* parent,Q3ListViewItem* after, QString l1,QString l2,QString l3,QString l4,
					 QString l5,QString l6,QString l7,QString l8)
		    :Q3ListViewItem(parent,after,l1,l2,l3,l4,l5,l6,l7,l8)
{
pEntry=entry;
setDragEnabled(true);
}




CGroupView::CGroupView(QWidget * parent, const char * name, Qt::WFlags f):Q3ListView(parent,name,f){

};