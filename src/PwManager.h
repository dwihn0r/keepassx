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
 #define DB_HEADER_SIZE 		124
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

#include <qcolor.h>
#include <qobject.h>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QStringList>
#include "lib/SecString.h"
#include "Database.h"


class PwDatabase:QObject,public Database{
 Q_OBJECT
public:
 PwDatabase();
 ~ PwDatabase();
 bool loadDatabase(QString filename, QString& err);
 bool saveDatabase();
 bool closeDatabase();
 bool CalcMasterKeyByPassword(QString& password);
 bool CalcMasterKeyByFile(QString filename);
 bool CalcMasterKeyByFileAndPw(QString filename, QString& password);

 CGroup*   addGroup(CGroup* parent);
 void      deleteGroup(CGroup* pGroup);
 void      deleteGroup(unsigned long ID);
 void	   moveGroup(CGroup* group, CGroup* DstGroup, int pos=-1);
 void	   moveGroupDirectly(CGroup* group, CGroup* DstGroup); //inserts group directly behind DstGroup on the same level
 int	   getGroupIndex(CGroup* group);
 int       getGroupIndex(unsigned long ID);
 int	   getNumberOfChilds(CGroup* pGroup);

 void      deleteEntry(CEntry* pEntry);
 void      moveEntry(CEntry* pEntry,CGroup* pDstGroup);
 CEntry*   addEntry();
 void 	   merge(PwDatabase* db2);
 bool	   isParentGroup(CGroup* Group,CGroup* PotenialParent);
 QString   getError();  //get first error
 QString   getErrors(); //get all errors in a \n seperated String



private:
 bool IsMetaStream(CEntry& Entry);
 void transformKey(Q_UINT8* src,Q_UINT8* dst,Q_UINT8* seed,int rounds);
 bool readHeader(char* raw);
 bool isGroupIdInUse(Q_UINT32 GroupID);
 bool isEntrySidInUse(Q_UINT32 sID);
 Q_UINT32 getNewGroupId();
 Q_UINT32 getNewEntrySid();
 bool convHexToBinaryKey(char* HexKey, char* dst);
 QStringList Errors;
};


 extern const QDateTime Date_Never;
 void memcpyFromLEnd32(Q_UINT32* dst,char* src);
 void memcpyFromLEnd16(Q_UINT16* dst,char* src);
 void memcpyToLEnd32(char* src,Q_UINT32* dst);
 void memcpyToLEnd16(char* src,Q_UINT16* dst);
 QDateTime dateFromPackedStruct5(const unsigned char* pBytes);
 void dateToPackedStruct5(const QDateTime& datetime, unsigned char* dst);

 bool testDatabase();

#endif
