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
#ifndef _PW_MANAGER_H_
#define _PW_MANAGER_H_
 #define DB_HEADER_SIZE 			124
 #define PWM_DBSIG_1			0x9AA2D903
 #define PWM_DBSIG_2			0xB54BFB65
 #define PWM_DBVER_DW  	 		0x00030002
 #define PWM_FLAG_SHA2			1
 #define PWM_FLAG_RIJNDAEL		2
 #define PWM_FLAG_ARCFOUR		4
 #define PWM_FLAG_TWOFISH		8
 #define PWM_STD_KEYENCROUNDS 	6000
 #define ALGO_AES         		0
 #define ALGO_TWOFISH			1
 #define KEEPASS_VERSION			"0.1.3"

#include <qcolor.h>
#include <qobject.h>
#include "lib/SecString.h"
#include "lib/PwmTime.h"
#include "Database.h"


class PwDatabase:QObject,public Database{
 Q_OBJECT
public:
 PwDatabase();
 ~ PwDatabase();
 bool loadDatabase(QString filename, QString& err);
 bool SaveDataBase(QString filename);
 bool NewDataBase();
 bool CloseDataBase();
 bool CalcMasterKeyByPassword(QString& password);
 bool CalcMasterKeyByFile(QString filename);
 bool CalcMasterKeyByFileAndPw(QString filename, QString& password);

 CGroup*   addGroup(CGroup* parent);
 GroupItr  deleteGroup(CGroup* pGroup);
 GroupItr  deleteGroup(unsigned long ID);
 int       getGroupIndex(unsigned long ID);
 EntryItr  deleteEntry(CEntry* pEntry);
 void      moveEntry(CEntry* pEntry,CGroup* pDstGroup);
 CEntry*   addEntry();
 void 	 merge(PwDatabase* db2);



private:
 EntryItr deleteEntry(vector<CEntry>::iterator i);
 EntryItr getEntryIterator(CEntry* pEntry);
 GroupItr getGroupIterator(CGroup* pGroup);
 bool IsMetaStream(CEntry& Entry);
 void transformKey(Q_UINT8* src,Q_UINT8* dst,Q_UINT8* seed,int rounds);
 bool readHeader(char* raw);
 bool isGroupIdInUse(Q_UINT32 GroupID);
 bool isEntrySidInUse(Q_UINT32 sID);
 Q_UINT32 getNewGroupId();
 Q_UINT32 getNewEntrySid();
 bool convHexToBinaryKey(char* HexKey, char* dst);
};

 void memcpyFromLEnd32(Q_UINT32* dst,char* src);
 void memcpyFromLEnd16(Q_UINT16* dst,char* src);

#endif
