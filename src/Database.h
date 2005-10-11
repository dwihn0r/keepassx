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
#include "lib/PwmTime.h"
#include "lib/SecString.h"

class CEntry{
public:
CEntry();
~CEntry();
UINT8 ID[16];
UINT32 sID;
UINT32 GroupID;
UINT32 ImageID;
QString Title;
QString URL;
QString UserName;
SecString Password;
QString Additional;
QString BinaryDesc;
CPwmTime Creation;
CPwmTime LastMod;
CPwmTime LastAccess;
CPwmTime Expire;
UINT8 *pBinaryData;
UINT32 BinaryDataLength;
UINT32 PasswordLength;
bool ReadEntryField(UINT16 FieldType, UINT32 FieldSize, UINT8 *pData);
};


class CGroup{
public:
CGroup();
~CGroup();
UINT32 ID;
UINT32 ImageID;
UINT32 NumEntries;
QString Name;
CPwmTime Creation;
CPwmTime LastMod;
CPwmTime LastAccess;
CPwmTime Expire;
UINT16 Level;
UINT32 Flags;
bool ReadGroupField(UINT16 FieldType, UINT32 FieldSize, UINT8 *pData);

bool 	  UI_ItemIsExpanded;
static bool UI_ExpandByDefault;

};


typedef vector<CEntry>::iterator EntryItr;
typedef vector<CGroup>::iterator GroupItr;


class AbstractDatabase{
public:
 vector<CGroup>Groups;
 vector<CEntry>Entries;
};



class Database:public AbstractDatabase{
public:
 int CryptoAlgorithmus;
 int KeyEncRounds;
 QString filename;
 bool modflag;
 int SearchGroupID;

protected:
 UINT8 MasterKey[32];
 UINT8 TransformedMasterKey[32];
};

#endif