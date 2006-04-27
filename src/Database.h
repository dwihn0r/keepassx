/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
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

#include <QList>
#include <QDateTime>
#include <QFile>
#include <QPixmap>
#include "lib/SecString.h"
using namespace std;

extern const QDateTime Date_Never;
class KpxDateTime:public QDateTime{
public:
KpxDateTime(){};
KpxDateTime ( const QDate & date ):QDateTime(date){};
KpxDateTime ( const QDate & date, const QTime & time, Qt::TimeSpec spec = Qt::LocalTime):QDateTime(date,time,spec){};
KpxDateTime ( const QDateTime & other ):QDateTime(other){};
virtual QString toString(Qt::DateFormat format = Qt::TextDate ) const;
QString dateToString(Qt::DateFormat format = Qt::TextDate)const;
};

class CEntry{
public:
CEntry();
~CEntry();
Q_UINT8 ID[16];
Q_UINT32 sID;
Q_UINT32 GroupID;
Q_UINT32 ImageID;
quint32 OldImgID;
QString Title;
QString URL;
QString UserName;
SecString Password;
QString Additional;
QString BinaryDesc;
KpxDateTime Creation;
KpxDateTime LastMod;
KpxDateTime LastAccess;
KpxDateTime Expire;
QByteArray BinaryData;
/*Q_UINT32 PasswordLength;*/
bool ReadEntryField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData);
bool operator==(const CEntry&) const;
};


class CGroup{
public:
CGroup();
~CGroup();
Q_UINT32 ID;
Q_UINT32 ImageID;
quint32 OldImgID;
QString Name;
QDateTime Creation;
QDateTime LastMod;
QDateTime LastAccess;
QDateTime Expire;
Q_UINT16 Level;
Q_UINT32 Flags;
bool ReadGroupField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData);
bool operator==(const CGroup&) const;

bool 	  UI_ItemIsExpanded;
static bool UI_ExpandByDefault;

};

#define ALGO_AES         		0
#define ALGO_TWOFISH			1

class Database:public QObject{
public:
 Database();
 virtual ~Database(){};
 virtual bool openDatabase(QString filename, QString& err)=0;
 virtual bool saveDatabase()=0;
 virtual bool closeDatabase()=0;
 virtual void newDatabase()=0;

 virtual bool CalcMasterKeyByPassword(QString& password)=0;
 virtual bool CalcMasterKeyByFile(QString filename)=0;
 virtual bool CalcMasterKeyByFileAndPw(QString filename, QString& password)=0;
 virtual bool createKeyFile(const QString& filename)=0;

 virtual CGroup&   group(unsigned long index)=0;
 virtual void	   setGroup(unsigned long index,CGroup& group)=0;
 virtual int	   numGroups()=0;
 virtual CGroup*   addGroup(CGroup* parent)=0;
 virtual void      deleteGroup(CGroup* pGroup)=0;
 virtual void      deleteGroup(unsigned long ID)=0;
 virtual void	   moveGroup(CGroup* group, CGroup* DstGroup, int pos=-1)=0;
 virtual void	   moveGroupDirectly(CGroup* group, CGroup* DstGroup)=0; //inserts group directly behind DstGroup on the same level
 virtual int	   getGroupIndex(CGroup* group)=0;
 virtual int       getGroupIndex(unsigned long ID)=0;
 virtual int	   getNumberOfChilds(CGroup* pGroup)=0;
 virtual QList<int> getChildIds(CGroup* pGroup)=0;

 virtual CEntry&   entry(unsigned long index)=0;
 virtual void	   setEntry(unsigned long index,CEntry& Entry)=0;
 virtual int	   numEntries()=0;
 virtual CEntry*   cloneEntry(CEntry* pEntry)=0;
 virtual void      deleteEntry(CEntry* pEntry)=0;
 virtual void      moveEntry(CEntry* pEntry,CGroup* pDstGroup)=0;
 virtual CEntry*   addEntry()=0;
 virtual CEntry*   addEntry(CEntry* NewEntry)=0;
 virtual void 	   merge(Database* db2)=0;
 virtual bool	   isParentGroup(CGroup* Group,CGroup* PotenialParent)=0;
 virtual QString   getError()=0;  //get first error
 virtual QString   getErrors()=0; //get all errors in a \n seperated String
 virtual QPixmap&  icon(int index)=0;
 virtual int	   numIcons()=0;
 virtual void	   addIcon(const QPixmap& icon)=0;
 virtual void	   removeIcon(int Id)=0;

 Q_UINT32 CryptoAlgorithmus;
 Q_UINT32 KeyEncRounds;
 QFile* file;
 bool modflag;
 int SearchGroupID;

protected:
 Q_UINT8 MasterKey[32];
 Q_UINT8 TransformedMasterKey[32];

};

#endif
