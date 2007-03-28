/***************************************************************************
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
 *   keepassx@gmail.com                                                    *
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
 
#ifndef _STD_DATABASE_H_
#define _STD_DATABASE_H_

#define DB_HEADER_SIZE	124
#define PWM_DBSIG_1		0x9AA2D903
#define PWM_DBSIG_2 	0xB54BFB65
#define PWM_DBVER_DW	0x00030002
#define PWM_FLAG_SHA2			1
#define PWM_FLAG_RIJNDAEL		2
#define PWM_FLAG_ARCFOUR		4
#define PWM_FLAG_TWOFISH		8
#define PWM_STD_KEYENCROUNDS 	6000

#include <qcolor.h>
#include <qobject.h>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QStringList>
#include <QPixmap>
#include <QMap>
#include "lib/SecString.h"
#include "Database.h"


void memcpyFromLEnd32(quint32* dst,const char* src);
void memcpyFromLEnd16(quint16* dst,const char* src);
void memcpyToLEnd32(char* src,const quint32* dst);
void memcpyToLEnd16(char* src,const quint16* dst);

//! Implementation of the standard KeePassX database.
class Kdb3Database:public ICustomIcons,public IDatabase, public IFilePasswordAuth, public IKdbSettings{
Q_OBJECT
public:
	class StdGroup;
	class StdEntry;
	class EntryHandle:public IEntryHandle{
		friend class Kdb3Database;
		public:
			EntryHandle(Kdb3Database* db);
			virtual void setImage(const quint32& ImageID);
			void setOldImage(const quint32& OldImgID);
			virtual void setTitle(const QString& Title);
			virtual void setUrl(const QString& URL);
			virtual void setUsername(const QString& Username);
			virtual void setPassword(const SecString& Password);
			virtual void setComment(const QString& Comment);
			virtual void setBinaryDesc(const QString& BinaryDesc);
			virtual void setCreation(const KpxDateTime& Creation);
			virtual void setLastMod(const KpxDateTime& LastMod);
			virtual void setLastAccess(const KpxDateTime& LastAccess);
			virtual void setExpire(const KpxDateTime& Expire);
			virtual void setBinary(const QByteArray& BinaryData);
			virtual KpxUuid uuid();
			virtual IGroupHandle* group();
			virtual quint32 image();
			virtual int visualIndex() const;
			virtual void setVisualIndex(int i);
			virtual void setVisualIndexDirectly(int i);
			quint32 oldImage();
			virtual QString title();
			virtual QString url();
			virtual QString username();
			virtual SecString password();
			virtual QString comment();
			virtual QString binaryDesc();
			virtual KpxDateTime creation();
			virtual KpxDateTime lastMod();
			virtual KpxDateTime lastAccess();
			virtual KpxDateTime expire();
			virtual QByteArray binary();
			virtual quint32 binarySize();
			virtual bool isValid() const;
		private:
			void invalidate(){valid=false;}
			bool valid;
			unsigned int ListIndex;
			KpxUuid Uuid;
			Kdb3Database* pDB;
			StdEntry* Entry;
	};
	class GroupHandle:public IGroupHandle{
		friend class Kdb3Database;
		GroupHandle(Kdb3Database* db);
		public:
			virtual void setTitle(const QString& Title);
			virtual void setImage(const quint32& ImageId);
			void setOldImage(const quint32& ImageId);	
			virtual QString title();
			virtual quint32 image();
			quint32 oldImage();
			virtual bool isValid();
			virtual IGroupHandle* parent();
			virtual QList<IGroupHandle*> childs();
			virtual int index();
			virtual void setIndex(int index);
			virtual int level();
			virtual bool expanded();
			virtual void setExpanded(bool IsExpanded);
		private:
			void invalidate(){valid=false;}
			bool valid;
			StdGroup* Group;
			Kdb3Database* pDB;
	};
	friend class EntryHandle;
	friend class GroupHandle;
	class StdEntry:public CEntry{
		public:
				quint32 OldImage;
				quint16 Index;
				EntryHandle* Handle;
				StdGroup* Group;
	};
	class StdGroup:public CGroup{
		public:
			StdGroup():CGroup(){};
			StdGroup(const CGroup&);
			quint32 OldImage;
			quint16 Index;
			StdGroup* Parent;
			GroupHandle* Handle;
			QList<StdGroup*> Childs;
			QList<StdEntry*> Entries;
	};
	virtual ~Kdb3Database(){};
	virtual bool load(QString identifier);
	virtual bool save();
	virtual bool close();
	virtual void create();
	virtual int numEntries();
	virtual int numGroups();
	virtual QString getError();
	virtual bool isKeyError();
	virtual void cleanUpHandles();
	virtual QPixmap& icon(int index);
 	virtual int	numIcons();
	virtual void addIcon(const QPixmap& icon);
	virtual void removeIcon(int index);
	virtual void replaceIcon(int index,const QPixmap& icon);
	virtual int builtinIcons(){return 62;};
	virtual void authByPwd(QString& password);
	virtual bool authByFile(QString& filename);
	virtual bool authByFileAndPwd(QString& password, QString& filename);
	virtual bool createKeyFile(const QString& filename,int length=32, bool Hex=false);
	virtual QList<IEntryHandle*> search(IGroupHandle* Group,const QString& SearchString, bool CaseSensitve, bool RegExp,bool Recursive,bool* Fields);
	virtual QFile* file(){return File;}
	virtual bool changeFile(const QString& filename);
	virtual void setCryptAlgorithm(CryptAlgorithm algo){Algorithm=algo;}
	virtual CryptAlgorithm cryptAlgorithm(){return Algorithm;}
	virtual unsigned int keyTransfRounds(){return KeyTransfRounds;}
	virtual void setKeyTransfRounds(unsigned int rounds){KeyTransfRounds=rounds;}	
	
	virtual QList<IEntryHandle*> entries();
	virtual QList<IEntryHandle*> entries(IGroupHandle* Group);
	virtual QList<IEntryHandle*> expiredEntries();
		
	virtual IEntryHandle* cloneEntry(const IEntryHandle* entry);
	virtual void deleteEntry(IEntryHandle* entry);
	virtual void deleteEntries(QList<IEntryHandle*> entries);
	virtual IEntryHandle* newEntry(IGroupHandle* group);
	virtual IEntryHandle* addEntry(const CEntry* NewEntry, IGroupHandle* group);
	virtual void moveEntry(IEntryHandle* entry, IGroupHandle* group);
	virtual void deleteLastEntry();

	
	virtual QList<IGroupHandle*> groups();
	virtual QList<IGroupHandle*> sortedGroups();
	virtual void deleteGroup(IGroupHandle* group);
	virtual void moveGroup(IGroupHandle* Group,IGroupHandle* NewParent,int Position);
	virtual IGroupHandle* addGroup(const CGroup* Group,IGroupHandle* Parent);
	virtual bool isParent(IGroupHandle* parent, IGroupHandle* child);

	

	
private:
	QDateTime dateFromPackedStruct5(const unsigned char* pBytes);
	void dateToPackedStruct5(const QDateTime& datetime, unsigned char* dst);
	bool isMetaStream(StdEntry& Entry);
	bool parseMetaStream(const StdEntry& Entry);
	void parseCustomIconsMetaStream(const QByteArray& data);
 	bool parseCustomIconsMetaStreamV1(const QByteArray& data);
	bool parseCustomIconsMetaStreamV2(const QByteArray& data);
	void parseGroupTreeStateMetaStream(const QByteArray& data);
	void createCustomIconsMetaStream(StdEntry* e);
	void createGroupTreeStateMetaStream(StdEntry* e);
	bool readEntryField(StdEntry* entry, quint16 FieldType, quint32 FieldSize, quint8 *pData);
	bool readGroupField(StdGroup* group,QList<quint32>& Levels,quint16 FieldType, quint32 FieldSize, quint8 *pData);
	bool createGroupTree(QList<quint32>& Levels);
	void createHandles();
	bool transformKey(quint8* src,quint8* dst,quint8* KeySeed,int rounds);
	void invalidateHandle(StdEntry* entry);
	bool convHexToBinaryKey(char* HexKey, char* dst);
	quint32 getNewGroupId();
	void serializeEntries(QList<StdEntry>& EntryList,char* buffer,unsigned int& pos);
	void serializeGroups(QList<StdGroup>& GroupList,char* buffer,unsigned int& pos);
	void appendChildsToGroupList(QList<StdGroup*>& list,StdGroup& group);
	void appendChildsToGroupList(QList<IGroupHandle*>& list,StdGroup& group);
	bool searchStringContains(const QString& search, const QString& string,bool Cs, bool RegExp);
	void getEntriesRecursive(IGroupHandle* Group, QList<IEntryHandle*>& EntryList);
	void rebuildIndices(QList<StdGroup*>& list);
	void restoreGroupTreeState();

	StdEntry* getEntry(const KpxUuid& uuid);
	StdEntry* getEntry(EntryHandle* handle);
	int getEntryListIndex(EntryHandle* handle);
	EntryHandle* getHandle(StdEntry* entry);
	
	StdGroup* getGroup(quint32 Id);
	void deleteGroup(StdGroup* group);

	QList<EntryHandle> EntryHandles;
	QList<GroupHandle> GroupHandles;
	QList<StdEntry> Entries;
	QList<StdGroup> Groups;
	StdGroup RootGroup;
	QList<QPixmap>CustomIcons;
	QFile* File;
	QString error;
	bool KeyError;
	QList<StdEntry> UnknownMetaStreams;
	QMap<quint32,bool> TreeStateMetaStream;
	unsigned int KeyTransfRounds;
	CryptAlgorithm Algorithm;
	quint8 RawMasterKey[32];
	quint8 MasterKey[32];
};

#endif
