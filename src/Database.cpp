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

#include "Database.h"


CEntry::CEntry(){
ImageID=0;
GroupID=0;
Creation=QDateTime::currentDateTime();
LastMod=QDateTime::currentDateTime();
LastAccess=QDateTime::currentDateTime();
Expire=QDateTime(QDate(2999,12,28),QTime(23,59,59));
BinaryDataLength=0;
pBinaryData=NULL;
}

bool CGroup::UI_ExpandByDefault=true;

CGroup::CGroup(){
Creation=QDateTime::currentDateTime();
LastAccess=QDateTime::currentDateTime();
LastMod=QDateTime::currentDateTime();
Expire=QDateTime(QDate(2999,12,28),QTime(23,59,59));
Level=0;
ImageID=0;
Name="<Group>";
UI_ItemIsExpanded=UI_ExpandByDefault;
}

CGroup::~CGroup(){
}

CEntry::~CEntry(){
if(pBinaryData) {
delete [] pBinaryData;
}
pBinaryData=NULL;

}
