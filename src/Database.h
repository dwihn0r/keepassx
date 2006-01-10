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
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <vector.h>
#include <QDateTime>
#include "lib/SecString.h"
using namespace std;

class CEntry{
public:
CEntry();
~CEntry();
Q_UINT8 ID[16];
Q_UINT32 sID;
Q_UINT32 GroupID;
Q_UINT32 ImageID;
QString Title;
QString URL;
QString UserName;
SecString Password;
QString Additional;
QString BinaryDesc;
QDateTime Creation;
QDateTime LastMod;
QDateTime LastAccess;
QDateTime Expire;
Q_UINT8 *pBinaryData;
Q_UINT32 BinaryDataLength;
Q_UINT32 PasswordLength;
bool ReadEntryField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData);
};


class CGroup{
public:
CGroup();
~CGroup();
Q_UINT32 ID;
Q_UINT32 ImageID;
Q_UINT32 NumEntries;
QString Name;
QDateTime Creation;
QDateTime LastMod;
QDateTime LastAccess;
QDateTime Expire;
Q_UINT16 Level;
Q_UINT32 Flags;
bool ReadGroupField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData);

bool 	  UI_ItemIsExpanded;
static bool UI_ExpandByDefault;

};


typedef vector<CEntry>::iterator EntryItr;
typedef vector<CGroup>::iterator GroupItr;


class Database{
public:
 Q_UINT32 CryptoAlgorithmus;
 Q_UINT32 KeyEncRounds;
 QString filename;
 bool modflag;
 int SearchGroupID;
 vector<CGroup>Groups;
 vector<CEntry>Entries;

protected:
 Q_UINT8 MasterKey[32];
 Q_UINT8 TransformedMasterKey[32];
};

#endif
