/***************************************************************************
 *   Copyright (C) 2005-2008 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <QBuffer>
#include <QTextCodec>
#include "Kdb3Database.h"


#define UNEXP_ERROR error=QString("Unexpected error in: %1, Line:%2").arg(__FILE__).arg(__LINE__);

const QDateTime Date_Never(QDate(2999,12,28),QTime(23,59,59));


bool EntryHandleLessThan(const IEntryHandle* This,const IEntryHandle* Other){
	if(!This->isValid() && Other->isValid())return true;
	if(This->isValid() && !Other->isValid())return false;
	if(!This->isValid() && !Other->isValid())return false;
	return This->visualIndex()<Other->visualIndex();

}

bool StdEntryLessThan(const Kdb3Database::StdEntry& This,const Kdb3Database::StdEntry& Other){
	return This.Index<Other.Index;
}


QString Kdb3Database::getError(){
	return error;
}

void Kdb3Database::addIcon(const QPixmap& icon){
	CustomIcons << icon;
	emit iconsModified();
}

QPixmap& Kdb3Database::icon(int i){
	if(i>=builtinIcons()+CustomIcons.size())
		return EntryIcons[0];
	if(i<builtinIcons())
		return EntryIcons[i];
	return CustomIcons[i-builtinIcons()];
}

void Kdb3Database::removeIcon(int id){
	id-=builtinIcons();
	if(id < 0 ) return;
	if(id >= CustomIcons.size()) return;
	CustomIcons.removeAt(id); // .isNull()==true
	for(int i=0;i<Entries.size();i++){
		if(Entries[i].Image == id+builtinIcons())
			Entries[i].Image=Entries[i].OldImage;
		if(Entries[i].Image>id+builtinIcons())
			Entries[i].Image--;
	}
	for(int i=0;i<Groups.size();i++){
		if(Groups[i].Image == id+builtinIcons())
			Groups[i].Image=Groups[i].OldImage;
		if(Groups[i].Image>id+builtinIcons())
			Groups[i].Image--;
	}
	emit iconsModified();
}

void Kdb3Database::replaceIcon(int id,const QPixmap& icon){
	if(id<builtinIcons())return;
		CustomIcons[id-builtinIcons()]=icon;
	emit iconsModified();
}

int Kdb3Database::numIcons(){
	return builtinIcons()+CustomIcons.size();
}

bool Kdb3Database::parseMetaStream(const StdEntry& entry){

	qDebug("Found Metastream: %s",entry.Comment.toUtf8().data());

	if(entry.Comment=="KPX_GROUP_TREE_STATE"){
		parseGroupTreeStateMetaStream(entry.Binary);
		return true;}

	if(entry.Comment=="KPX_CUSTOM_ICONS_3"){
		parseCustomIconsMetaStream(entry.Binary);
		return true;}

	if(entry.Comment=="KPX_CUSTOM_ICONS_2")
		return parseCustomIconsMetaStreamV2(entry.Binary);

	if(entry.Comment=="KPX_CUSTOM_ICONS")
		return parseCustomIconsMetaStreamV1(entry.Binary);

	return false; //unknown MetaStream
}

bool Kdb3Database::isMetaStream(StdEntry& p){
	if(p.Binary.isNull()) return false;
	if(p.Comment == "") return false;
	if(p.BinaryDesc != "bin-stream") return false;
	if(p.Title != "Meta-Info") return false;
	if(p.Username != "SYSTEM") return false;
	if(p.Url != "$") return false;
	if(p.Image != 0) return false;
	return true;
}


bool Kdb3Database::parseCustomIconsMetaStreamV1(const QByteArray& dta){
	Q_UNUSED(dta)
	qDebug("Removed old CuIcMeSt v1");
	return true;
}

bool Kdb3Database::parseCustomIconsMetaStreamV2(const QByteArray& dta){
	Q_UNUSED(dta)
	qDebug("Removed old CuIcMeSt v2");
	return true;
}

void Kdb3Database::parseCustomIconsMetaStream(const QByteArray& dta){
	//Rev 3
	quint32 NumIcons,NumEntries,NumGroups,offset;
	memcpyFromLEnd32(&NumIcons,dta.data());
	memcpyFromLEnd32(&NumEntries,dta.data()+4);
	memcpyFromLEnd32(&NumGroups,dta.data()+8);
	offset=12;
	CustomIcons.clear();
	for(int i=0;i<NumIcons;i++){
		CustomIcons << QPixmap();
		quint32 Size;
		memcpyFromLEnd32(&Size,dta.data()+offset);
		if(offset+Size > dta.size()){
			CustomIcons.clear();
			qWarning("Discarded metastream KPX_CUSTOM_ICONS_3 because of a parsing error.");
			return;}
		offset+=4;
		if(!CustomIcons.back().loadFromData((const unsigned char*)dta.data()+offset,Size,"PNG")){
			CustomIcons.clear();
			qWarning("Discarded metastream KPX_CUSTOM_ICONS_3 because of a parsing error.");
			return;}
		offset+=Size;
		if(offset > dta.size()){
			CustomIcons.clear();
			qWarning("Discarded metastream KPX_CUSTOM_ICONS_3 because of a parsing error.");
			return;}
	}
	for(int i=0;i<NumEntries;i++){
		quint32 Icon;
		KpxUuid EntryUuid;
		EntryUuid.fromRaw(dta.data()+offset);
		offset+=16;
		memcpyFromLEnd32(&Icon,dta.data()+offset);
		offset+=4;
		StdEntry* entry=getEntry(EntryUuid);
		if(entry){
			entry->OldImage=entry->Image;
			entry->Image=Icon;
		}
	}
	for(int i=0;i<NumGroups;i++){
		quint32 GroupId,Icon;
		memcpyFromLEnd32(&GroupId,dta.data()+offset);
		offset+=4;
		memcpyFromLEnd32(&Icon,dta.data()+offset);
		offset+=4;
		StdGroup* Group=getGroup(GroupId);
		if(Group){
			Group->OldImage=Group->Image;
			Group->Image=Icon;
		}
	}
	return;
}

void Kdb3Database::parseGroupTreeStateMetaStream(const QByteArray& dta){
	if(dta.size()<4){qWarning("Discarded metastream KPX_GROUP_TREE_STATE because of a parsing error."); return;}
	quint32 Num;
	memcpyFromLEnd32(&Num,dta.data());
	if(Num*5!=dta.size()-4){qWarning("Discarded metastream KPX_GROUP_TREE_STATE because of a parsing error."); return;}
	TreeStateMetaStream.clear();
	for(int i=0;i<Num;i++){
		quint32 GroupID;
		quint8 IsExpanded;
		memcpyFromLEnd32(&GroupID,dta.data()+4+5*i);
		memcpy(&IsExpanded,dta.data()+8+5*i,1);
		TreeStateMetaStream.insert(GroupID,(bool)IsExpanded);
	}
	return;
}

void Kdb3Database::createGroupTreeStateMetaStream(StdEntry* e){
	e->BinaryDesc="bin-stream";
	e->Title="Meta-Info";
	e->Username="SYSTEM";
	e->Comment="KPX_GROUP_TREE_STATE";
	e->Url="$";
	e->OldImage=0;
	e->Image=0;
	if(Groups.size())e->GroupId=Groups[0].Id;
	QByteArray bin;
	quint32 Num=Groups.size();
	bin.resize(Num*5+4);
	memcpyToLEnd32(bin.data(),&Num);
	for(int i=0;i<Num;i++){
		memcpyToLEnd32(bin.data()+4+5*i,&Groups[i].Id);
		if(Groups[i].IsExpanded)
			bin.data()[8+5*i]=1;
		else
			bin.data()[8+5*i]=0;
	}
	e->Binary=bin;
}

Kdb3Database::StdEntry* Kdb3Database::getEntry(const KpxUuid& uuid){
	for(int i=0; i<Entries.size();i++)
		if(Entries[i].Uuid==uuid)return &Entries[i];
	return NULL;
}

Kdb3Database::StdGroup* Kdb3Database::getGroup(quint32 Id){
	for(int i=0; i<Groups.size();i++)
		if(Groups[i].Id==Id)return &Groups[i];
	return NULL;
}


//! Extracts one entry from raw decrypted data.
bool Kdb3Database::readEntryField(StdEntry* entry, quint16 FieldType, quint32 FieldSize, quint8 *pData){
switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		entry->Uuid=KpxUuid(pData);
		break;
	case 0x0002:
		memcpyFromLEnd32(&entry->GroupId, (char*)pData);
		break;
	case 0x0003:
		memcpyFromLEnd32(&entry->Image, (char*)pData);
		entry->OldImage=entry->Image;
		break;
	case 0x0004:
		entry->Title=QString::fromUtf8((char*)pData);
		break;
	case 0x0005:
		entry->Url=QString::fromUtf8((char*)pData);
		break;
	case 0x0006:
		entry->Username=QString::fromUtf8((char*)pData);
		break;
	case 0x0007:{
		QString s=QString::fromUtf8((char*)pData);
		entry->Password.setString(s,true);
		break;}
	case 0x0008:
		entry->Comment=QString::fromUtf8((char*)pData);
		break;
	case 0x0009:
		entry->Creation=dateFromPackedStruct5(pData);
		break;
	case 0x000A:
		entry->LastMod=dateFromPackedStruct5(pData);
		break;
	case 0x000B:
		entry->LastAccess=dateFromPackedStruct5(pData);
		break;
	case 0x000C:
		entry->Expire=dateFromPackedStruct5(pData);
		break;
	case 0x000D:
		entry->BinaryDesc=QString::fromUtf8((char*)pData);
		break;
	case 0x000E:
		if(FieldSize != 0)
			entry->Binary=QByteArray((char*)pData,FieldSize);
		else
			entry->Binary=QByteArray();
		break;
	case 0xFFFF:
		break;
	default:
		return false;
	}
	return true;
}

//! Extracts one group from raw decrypted data.
bool Kdb3Database::readGroupField(StdGroup* group,QList<quint32>& Levels,quint16 FieldType, quint32 FieldSize, quint8 *pData)
{
	switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpyFromLEnd32(&group->Id, (char*)pData);
		break;
	case 0x0002:
		group->Title=QString::fromUtf8((char*)pData);
		break;
	case 0x0003: //not longer used by KeePassX but part of the KDB format
		break;
	case 0x0004: //not longer used by KeePassX but part of the KDB format
		break;
	case 0x0005: //not longer used by KeePassX but part of the KDB format
		break;
	case 0x0006: //not longer used by KeePassX but part of the KDB format
		break;
	case 0x0007:
		memcpyFromLEnd32(&group->Image, (char*)pData);
		group->OldImage=group->Image;
		break;
	case 0x0008:
		quint16 Level;
		memcpyFromLEnd16(&Level, (char*)pData);
		Levels.append(Level);
		break;
	case 0x0009:
		 //not used by KeePassX but part of the KDB format
		 //memcpyFromLEnd32(&Flags, (char*)pData);
		break;
	case 0xFFFF:
		break;
	default:
		return false; // Field unsupported
	}

	return true; // Field supported
}

bool Kdb3Database::createGroupTree(QList<quint32>& Levels){
	if(Levels[0]!=0) return false;
	//find the parent for every group
	for(int i=0;i<Groups.size();i++){
		if(Levels[i]==0){
			Groups[i].Parent=&RootGroup;
			Groups[i].Index=RootGroup.Childs.size();
			RootGroup.Childs.append(&Groups[i]);
			continue;}
		int j;
		//the first item with a lower level is the parent
		for(j=i-1;j>=0;j--){
			if(Levels[j]<Levels[i]){
				if(Levels[i]-Levels[j]!=1)return false;
				break;
			}
			if(j==0)return false; //No parent found
		}
		Groups[i].Parent=&Groups[j];
		Groups[i].Index=Groups[j].Childs.size();
		Groups[i].Parent->Childs.append(&Groups[i]);
	}

	QList<int> EntryIndexCounter;
	for(int i=0;i<Groups.size();i++)EntryIndexCounter << 0;

	for(int e=0;e<Entries.size();e++){
		for(int g=0;g<Groups.size();g++){
			if(Entries[e].GroupId==Groups[g].Id){
				Groups[g].Entries.append(&Entries[e]);
				Entries[e].Group=&Groups[g];
				Entries[e].Index=EntryIndexCounter[g];
				EntryIndexCounter[g]++;
			}
		}
	}

	return true;
}

void Kdb3Database::createHandles(){
	for(int i=0;i<Groups.size();i++){
		GroupHandles.append(GroupHandle(this));
		Groups[i].Handle=&GroupHandles.back();
		GroupHandles.back().Group=&Groups[i];
	}
	for(int i=0;i<Entries.size();i++){
		EntryHandles.append(EntryHandle(this));
		Entries[i].Handle=&EntryHandles.back();
		EntryHandles.back().Entry=&Entries[i];
	}
}

void Kdb3Database::restoreGroupTreeState(){
	switch (config->groupTreeState()){
		case KpxConfig::RestoreLast:
			for(int i=0;i<Groups.size();i++){
				if(TreeStateMetaStream.contains(Groups[i].Id))
					Groups[i].IsExpanded=TreeStateMetaStream.value(Groups[i].Id);
			}
			break;

		case KpxConfig::ExpandAll:
			for(int i=0;i<Groups.size();i++)
				Groups[i].IsExpanded=true;
			break;
	}
}

bool Kdb3Database::load(QString filename){
unsigned long total_size,crypto_size;
quint32 Signature1,Signature2,Version,NumGroups,NumEntries,Flags;
quint8 TransfRandomSeed[32];
quint8 FinalRandomSeed[16];
quint8 ContentsHash[32];
quint8 EncryptionIV[16];

File=new QFile(filename);
if(!File->open(QIODevice::ReadWrite)){
	if(!File->open(QIODevice::ReadOnly)){
		error=tr("Could not open file.");
		delete File;
		File=NULL;
		return false;
	}
}
total_size=File->size();
char* buffer = new char[total_size];
DECRYPT:
File->read(buffer,total_size);

if(total_size < DB_HEADER_SIZE){
error=tr("Unexpected file size (DB_TOTAL_SIZE < DB_HEADER_SIZE)");
return false; }

memcpyFromLEnd32(&Signature1,buffer);
memcpyFromLEnd32(&Signature2,buffer+4);
memcpyFromLEnd32(&Flags,buffer+8);
memcpyFromLEnd32(&Version,buffer+12);
memcpy(FinalRandomSeed,buffer+16,16);
memcpy(EncryptionIV,buffer+32,16);
memcpyFromLEnd32(&NumGroups,buffer+48);
memcpyFromLEnd32(&NumEntries,buffer+52);
memcpy(ContentsHash,buffer+56,32);
memcpy(TransfRandomSeed,buffer+88,32);
memcpyFromLEnd32(&KeyTransfRounds,buffer+120);

if((Signature1!=PWM_DBSIG_1) || (Signature2!=PWM_DBSIG_2)){
error=tr("Wrong Signature");
return false;}

if((Version & 0xFFFFFF00) != (PWM_DBVER_DW & 0xFFFFFF00)){
	error=tr("Unsupported File Version.");
	return false;}

if(Flags & PWM_FLAG_RIJNDAEL) Algorithm = Rijndael_Cipher;
else if(Flags & PWM_FLAG_TWOFISH) Algorithm = Twofish_Cipher;
		else{error=tr("Unknown Encryption Algorithm.");
			 return false;}


if(!transformKey(RawMasterKey,MasterKey,TransfRandomSeed,KeyTransfRounds))return false;
quint8 FinalKey[32];

SHA256 sha;
sha.update(FinalRandomSeed,16);
sha.update(MasterKey,32);
sha.finish(FinalKey);

if(Algorithm == Rijndael_Cipher){
	AESdecrypt aes;
	aes.key256(FinalKey);
	aes.cbc_decrypt((unsigned char*)buffer+DB_HEADER_SIZE,(unsigned char*)buffer+DB_HEADER_SIZE,total_size-DB_HEADER_SIZE,(unsigned char*)EncryptionIV);
	crypto_size=total_size-((quint8*)buffer)[total_size-1]-DB_HEADER_SIZE;
}
else if(Algorithm == Twofish_Cipher){
	CTwofish twofish;
	if(twofish.init(FinalKey, 32, EncryptionIV) != true){return false;}
	crypto_size = (unsigned long)twofish.padDecrypt((quint8 *)buffer + DB_HEADER_SIZE,
	total_size - DB_HEADER_SIZE, (quint8 *)buffer + DB_HEADER_SIZE);
}

if((crypto_size > 2147483446) || (!crypto_size && NumGroups)){error=tr("Decryption failed.\nThe key is wrong or the file is damaged."); return false;}
SHA256::hashBuffer(buffer+DB_HEADER_SIZE,FinalKey,crypto_size);

if(memcmp(ContentsHash, FinalKey, 32) != 0){
	delete buffer;
	if(PotentialEncodingIssue){
		// KeePassX used Latin-1 encoding for passwords until version 0.3.1
		// but KeePass/Win32 uses Windows Codepage 1252.
		// Too stay compatible with databases created with KeePassX <= 0.3.1
		// the loading function gives both encodings a try.
		memcpy(RawMasterKey,RawMasterKey_Latin1,32);
		PotentialEncodingIssue=false;
		qDebug("Decryption failed. Retrying with Latin-1.");
		return load(filename); // second try
	}
	error=tr("Hash test failed.\nThe key is wrong or the file is damaged.");
	KeyError=true;
	return false;
}

unsigned long pos = DB_HEADER_SIZE;
quint16 FieldType;
quint32 FieldSize;
char* pField;
bool bRet;
StdGroup group;
QList<quint32> Levels;
RootGroup.Title="$ROOT$";
RootGroup.Parent=NULL;
RootGroup.Handle=NULL;

	for(unsigned long CurGroup = 0; CurGroup < NumGroups; )
	{
		pField = buffer+pos;

		memcpyFromLEnd16(&FieldType, pField);
		pField += 2; pos += 2;
		if(pos >= total_size) {
		 error=tr("Unexpected error: Offset is out of range.").append(" [G1]");
		 return false; }

		memcpyFromLEnd32(&FieldSize, pField);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)){
		 error=tr("Unexpected error: Offset is out of range.").append(" [G2]");
		return false;}

		bRet = readGroupField(&group,Levels, FieldType, FieldSize, (quint8 *)pField);
		if((FieldType == 0xFFFF) && (bRet == true)){
			Groups << group;
			CurGroup++;} // Now and ONLY now the counter gets increased

		pField += FieldSize;
		pos += FieldSize;
		if(pos >= total_size) {
		 error=tr("Unexpected error: Offset is out of range.").append(" [G1]");
		 return false;}
	}

StdEntry entry;

	for(unsigned long CurEntry = 0; CurEntry < NumEntries;)
	{
		pField = buffer+pos;

		memcpyFromLEnd16(&FieldType, pField);
		pField += 2; pos += 2;
		if(pos >= total_size){
		 error=tr("Unexpected error: Offset is out of range.").append(" [E1]");
		 return false;}

		memcpyFromLEnd32(&FieldSize, pField);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)) {
		 error=tr("Unexpected error: Offset is out of range.").append(" [E2]");
		 return false; }

		bRet = readEntryField(&entry,FieldType,FieldSize,(quint8*)pField);

		if((FieldType == 0xFFFF) && (bRet == true)){
			Entries << entry;
			if(!entry.GroupId)qDebug("NULL: %i, '%s'",(int)CurEntry,(char*)entry.Title.toUtf8().data());
			CurEntry++;}

		pField += FieldSize;
		pos += FieldSize;
		if(pos >= total_size) {
		 error=tr("Unexpected error: Offset is out of range.").append(" [E3]");
		 return false; }
	}

if(!createGroupTree(Levels)){
	error=tr("Invalid group tree.");
	return false;
}

delete [] buffer;

//Remove the metastreams from the entry list
for(int i=0;i<Entries.size();i++){
	if(isMetaStream(Entries[i])==true){
		if(!parseMetaStream(Entries[i]))
			UnknownMetaStreams << Entries[i];
		Entries.removeAt(i);
		i--;}

}

int* EntryIndices=new int[Groups.size()];
for(int i=0;i<Groups.size();i++)EntryIndices[i]=0;

for(int g=0;g<Groups.size();g++){
	for(int e=0;e<Entries.size();e++){
		if(Entries[e].GroupId==Groups[g].Id){
			Entries[e].Index=EntryIndices[g];
			EntryIndices[g]++;
		}
	}
}
delete [] EntryIndices;
createHandles();
restoreGroupTreeState();

return true;
}

QDateTime Kdb3Database::dateFromPackedStruct5(const unsigned char* pBytes){
quint32 dw1, dw2, dw3, dw4, dw5;
dw1 = (quint32)pBytes[0]; dw2 = (quint32)pBytes[1]; dw3 = (quint32)pBytes[2];
dw4 = (quint32)pBytes[3]; dw5 = (quint32)pBytes[4];
int y = (dw1 << 6) | (dw2 >> 2);
int mon = ((dw2 & 0x00000003) << 2) | (dw3 >> 6);
int d = (dw3 >> 1) & 0x0000001F;
int h = ((dw3 & 0x00000001) << 4) | (dw4 >> 4);
int min = ((dw4 & 0x0000000F) << 2) | (dw5 >> 6);
int s = dw5 & 0x0000003F;
return QDateTime(QDate(y,mon,d),QTime(h,min,s));
}


void Kdb3Database::dateToPackedStruct5(const QDateTime& d,unsigned char* pBytes){
pBytes[0] = (quint8)(((quint32)d.date().year() >> 6) & 0x0000003F);
pBytes[1] = (quint8)((((quint32)d.date().year() & 0x0000003F) << 2) | (((quint32)d.date().month() >> 2) & 0x00000003));
pBytes[2] = (quint8)((((quint32)d.date().month() & 0x00000003) << 6) | (((quint32)d.date().day() & 0x0000001F) << 1) | (((quint32)d.time().hour() >> 4) & 0x00000001));
pBytes[3] = (quint8)((((quint32)d.time().hour() & 0x0000000F) << 4) | (((quint32)d.time().minute() >> 2) & 0x0000000F));
pBytes[4] = (quint8)((((quint32)d.time().minute() & 0x00000003) << 6) | ((quint32)d.time().second() & 0x0000003F));
}


bool Kdb3Database::transformKey(quint8* src,quint8* dst,quint8* KeySeed,int rounds){
	quint8 tmp[32];
	AESencrypt aes;
	aes.key256(KeySeed);
	memcpy(tmp,src,32);
	for(int i=0;i<rounds;i++){
		aes.ecb_encrypt(tmp,tmp,32);
	}
	SHA256::hashBuffer(tmp,dst,32);
	return true;
}


int Kdb3Database::numGroups(){
	return Groups.size();
}

int Kdb3Database::numEntries(){
	return Entries.size();
}



void Kdb3Database::deleteGroup(StdGroup* group){

	while(group->Childs.size())
		deleteGroup(group->Childs.front());

	QList<IEntryHandle*> GroupEntries;
	GroupEntries=entries(group->Handle);
	deleteEntries(GroupEntries);

	Q_ASSERT(group==group->Parent->Childs[group->Index]);
	group->Parent->Childs.removeAt(group->Index);
	for(int i=group->Index;i<group->Parent->Childs.size();i++){
		group->Parent->Childs[i]->Index--;
	}
	group->Handle->invalidate();

	for(int i=0;i<Groups.size();i++){
		if(&Groups[i]==group){
			Groups.removeAt(i);
			break;
		}
	}

}


void Kdb3Database::deleteGroup(IGroupHandle* group){
	deleteGroup(((GroupHandle*)group)->Group);
}

void Kdb3Database::GroupHandle::setIndex(int index){
	/*
	quint32 ParentId=((GroupHandle*)parent())->Id;
	int Pos=pDB->getGroupListIndex(this);
	int NewPos=0;
	// Move the group to the new position in the list
	if(ParentId)
		NewPos=pDB->getGroupListIndex((GroupHandle*)parent());
	if(!index){
		if(ParentId)
			pDB->Groups.move(Pos,NewPos+1);
		else
			pDB->Groups.move(Pos,NewPos);
	}
	else{
		for(NewPos;NewPos<pDB->Groups.size();NewPos++){
			if(pDB->Groups[NewPos].ParentId==ParentId && pDB->Groups[NewPos].Index+1==index)
				break;
		}
		//skip the childs of the found sibling
		for(NewPos;NewPos<Groups.size();NewPos++){
			if(Groups[NewPos]
			pDB->Groups.move(Pos,NewPos);
		}

	}
	// adjust the indices
	int NewIndex=0;
	for(int i=0;i<pDB->Groups.size();i++){
		if(pDB->Groups[i].ParentId==ParentId){
			pDB->Groups[i].Index=NewIndex;
			NewIndex++;
		}
	}
	*/
}


bool Kdb3Database::convHexToBinaryKey(char* HexKey, char* dst){
	QString hex=QString::fromAscii(HexKey,64);
	for(int i=0; i<64; i+=2){
		bool err;
		quint8 bin;
		bin=hex.mid(i,2).toUInt(&err,16);
		if(!err)return false;
		memcpy(dst+(i/2),&bin,1);
	}
	return true;
}

bool Kdb3Database::setKey(const QString& password,const QString& keyfile){
	if(!password.isEmpty() && !keyfile.isEmpty())
		return setCompositeKey(password,keyfile);
	if(!password.isEmpty())
		return setPasswordKey(password);
	if(!keyfile.isEmpty())
		return setFileKey(keyfile);
	assert(false);	
}

bool Kdb3Database::setPasswordKey(const QString& Password){
	assert(Password.size());
	QTextCodec* codec=QTextCodec::codecForName("Windows-1252");
	QByteArray Password_CP1252 = codec->fromUnicode(Password);
	SHA256::hashBuffer(Password_CP1252.data(),RawMasterKey,Password_CP1252.size());
	QByteArray Password_Latin1 = Password.toLatin1();
	if(Password_Latin1 != Password_CP1252){
		// KeePassX used Latin-1 encoding for passwords until version 0.3.1
		// but KeePass/Win32 uses Windows Codepage 1252.
		// Too stay compatible with databases created with KeePassX <= 0.3.1
		// the loading function gives both encodings a try.
		PotentialEncodingIssue = true;
		SHA256::hashBuffer(Password_Latin1.data(),RawMasterKey_Latin1,Password_Latin1.size());
	}
	else {
		// If the password does not contain problematic characters we don't need
		// to try both encodings.
		PotentialEncodingIssue = false;
	}
	return true;
}

bool Kdb3Database::setFileKey(const QString& filename){
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly|QIODevice::Unbuffered)){
		error=decodeFileError(file.error());
		return false;
	}
	unsigned long FileSize=file.size();
	if(FileSize == 0){
		error=tr("Key file is empty.");
		return false;
	}
	if(FileSize == 32){
		if(file.read((char*)RawMasterKey,32) != 32){
			error=decodeFileError(file.error());
			return false;}
		return true;
	}
	if(FileSize == 64){
		char hex[64];
		if(file.read(hex,64) != 64){
			error=decodeFileError(file.error());
			return false;}
		if(convHexToBinaryKey(hex,(char*)RawMasterKey))return true;
	}
	SHA256 sha;
	unsigned char* buffer = new unsigned char[2048];
	while(1)
	{
		unsigned long read=file.read((char*)buffer,2048);
		if(read == 0) break;
		sha.update(buffer,read);
		if(read != 2048) break;
	}
	sha.finish(RawMasterKey);
	delete [] buffer;
	return true;
}

bool Kdb3Database::setCompositeKey(const QString& Password,const QString& filename){
	unsigned char PasswordKey[32];
	unsigned char FileKey[32];
	if(!setFileKey(filename))return false;
	memcpy(FileKey,RawMasterKey,32);
	setPasswordKey(Password);
	memcpy(PasswordKey,RawMasterKey,32);
	SHA256 sha;
	sha.update(PasswordKey,32);
	sha.update(FileKey,32);
	sha.finish(RawMasterKey);
	return true;
}

QList<IEntryHandle*> Kdb3Database::entries(){
	QList<IEntryHandle*> handles;
	for(int i=0; i<EntryHandles.size(); i++){
		if(EntryHandles[i].isValid())handles.append(&EntryHandles[i]);
	}
	return handles;
}

QList<IEntryHandle*> Kdb3Database::expiredEntries(){
	QList<IEntryHandle*> handles;
	for(int i=0; i<EntryHandles.size(); i++){
		if(EntryHandles[i].isValid() &&
		  (EntryHandles[i].expire()<=QDateTime::currentDateTime()) &&
		  (EntryHandles[i].expire()!=Date_Never))
			handles.append(&EntryHandles[i]);
	}
	return handles;
}

QList<IEntryHandle*> Kdb3Database::entries(IGroupHandle* group){
	QList<IEntryHandle*> handles;
	for(int i=0; i<EntryHandles.size(); i++){
		if(EntryHandles[i].isValid() && (EntryHandles[i].group()==group))
			handles.append(&EntryHandles[i]);
	}
	qSort(handles.begin(),handles.end(),EntryHandleLessThan);

	return handles;
}

void Kdb3Database::deleteEntry(IEntryHandle* entry){
	if(!entry)return;
	int j;
	for(j=0;j<Entries.size();j++){
		if(&Entries[j]==((EntryHandle*)entry)->Entry)
			break;
	}
	Entries[j].Handle->invalidate();
	Entries.removeAt(j);
}

void Kdb3Database::moveEntry(IEntryHandle* entry, IGroupHandle* group){
	((EntryHandle*)entry)->Entry->GroupId=((GroupHandle*)group)->Group->Id;
	((EntryHandle*)entry)->Entry->Group=((GroupHandle*)group)->Group;
}


void Kdb3Database::deleteEntries(QList<IEntryHandle*> entries){
	if(!entries.size())return;
	StdGroup* Group=((EntryHandle*)entries[0])->Entry->Group;
	for(int i=0;i<entries.size();i++){
		int j;
		for(j=0;j<Entries.size();j++){
			if(&Entries[j]==((EntryHandle*)entries[i])->Entry)
				break;
		}
		Group->Childs.removeAt(Entries[j].Index);
		Entries[j].Handle->invalidate();
		Entries.removeAt(j);
	}

	for(int i=0;i<Group->Childs.size();i++){
		Group->Childs[i]->Index=i;
	}
};

QList<IGroupHandle*> Kdb3Database::groups(){
	QList<IGroupHandle*> handles;
	for(int i=0; i<GroupHandles.size(); i++){
		if(GroupHandles[i].isValid())handles.append(&GroupHandles[i]);
	}
	return handles;
}

quint32 Kdb3Database::getNewGroupId(){
	quint32 id;
	bool used;
	while(1){
		used=false;
		randomize(&id,4);
		if(!id)continue; //group IDs must not be 0
		for(int j=0;j<Groups.size();j++){
			if(Groups[j].Id==id)used=true;}
		if(used==false)break;
	}
	return id;
}

IGroupHandle* Kdb3Database::addGroup(const CGroup* group,IGroupHandle* ParentHandle){
	GroupHandles.append(GroupHandle(this));
	Groups.append(*group);
	Groups.back().Id=getNewGroupId();
	Groups.back().Handle=&GroupHandles.back();
	GroupHandles.back().Group=&Groups.back();
	if(ParentHandle){
		Groups.back().Parent=((GroupHandle*)ParentHandle)->Group;
		Groups.back().Index=Groups.back().Parent->Childs.size();
		Groups.back().Parent->Childs.append(&Groups.back());
	}
	else{
		Groups.back().Parent=&RootGroup;
		Groups.back().Index=RootGroup.Childs.size();
		Groups.back().Parent->Childs.append(&Groups.back());
	}
	return &GroupHandles.back();
}

Kdb3Database::StdGroup::StdGroup(const CGroup& other){
	OldImage=0;
	Index=0;
	Id=other.Id;
	Image=other.Image;
	Title=other.Title;
}

void Kdb3Database::EntryHandle::setTitle(const QString& Title){Entry->Title=Title; }
void Kdb3Database::EntryHandle::setUsername(const QString& Username){Entry->Username=Username;}
void Kdb3Database::EntryHandle::setUrl(const QString& Url){Entry->Url=Url;}
void Kdb3Database::EntryHandle::setPassword(const SecString& Password){Entry->Password=Password;}
void Kdb3Database::EntryHandle::setExpire(const KpxDateTime& s){Entry->Expire=s;}
void Kdb3Database::EntryHandle::setCreation(const KpxDateTime& s){Entry->Creation=s;}
void Kdb3Database::EntryHandle::setLastAccess(const KpxDateTime& s){Entry->LastAccess=s;}
void Kdb3Database::EntryHandle::setLastMod(const KpxDateTime& s){Entry->LastMod=s;}
void Kdb3Database::EntryHandle::setBinaryDesc(const QString& s){Entry->BinaryDesc=s;}
void Kdb3Database::EntryHandle::setComment(const QString& s){Entry->Comment=s;}
void Kdb3Database::EntryHandle::setBinary(const QByteArray& s){Entry->Binary=s;}
void Kdb3Database::EntryHandle::setImage(const quint32& s){Entry->Image=s;}
void Kdb3Database::EntryHandle::setOldImage(const quint32& s){Entry->OldImage=s;}
KpxUuid	Kdb3Database::EntryHandle::uuid(){return Entry->Uuid;}
IGroupHandle* Kdb3Database::EntryHandle::group(){return Entry->Group->Handle;}
quint32	Kdb3Database::EntryHandle::image(){return Entry->Image;}
quint32	Kdb3Database::EntryHandle::oldImage(){return Entry->OldImage;}
QString	Kdb3Database::EntryHandle::title(){return Entry->Title;}
QString	Kdb3Database::EntryHandle::url(){return Entry->Url;}
QString	Kdb3Database::EntryHandle::username(){return Entry->Username;}
SecString Kdb3Database::EntryHandle::password(){return Entry->Password;}
QString	Kdb3Database::EntryHandle::comment(){return Entry->Comment;}
QString	Kdb3Database::EntryHandle::binaryDesc(){return Entry->BinaryDesc;}
KpxDateTime	Kdb3Database::EntryHandle::creation(){return Entry->Creation;}
KpxDateTime	Kdb3Database::EntryHandle::lastMod(){return Entry->LastMod;}
KpxDateTime	Kdb3Database::EntryHandle::lastAccess(){return Entry->LastAccess;}
KpxDateTime	Kdb3Database::EntryHandle::expire(){return Entry->Expire;}
QByteArray Kdb3Database::EntryHandle::binary(){return Entry->Binary;}
quint32 Kdb3Database::EntryHandle::binarySize(){return Entry->Binary.size();}

QString Kdb3Database::EntryHandle::friendlySize()
{
    quint32 binsize = binarySize();
    QString unit;
    uint    faktor;
    int     prec;

    if (binsize < 1024)
    {
        unit = tr("Bytes");
        faktor = 1;
        prec = 0;
    }
    else
    {
        if (binsize < 1048576)
        {
            unit = tr("KiB");
            faktor = 1024;
        }
        else
            if (binsize < 1073741824)
            {
                unit = tr("MiB");
                faktor = 1048576;
            }
            else
            {
                unit = tr("GiB");
                faktor = 1073741824;
            }
        prec = 1;
    }
    return (QString::number((float)binsize / (float)faktor, 'f', prec) + " " + unit);
}

int Kdb3Database::EntryHandle::visualIndex()const{return Entry->Index;}
void Kdb3Database::EntryHandle::setVisualIndexDirectly(int i){Entry->Index=i;}
bool Kdb3Database::EntryHandle::isValid()const{return valid;}

void Kdb3Database::EntryHandle::setVisualIndex(int index){
	QList<IEntryHandle*>Entries=pDB->entries(Entry->Group->Handle);
	Entries.move(visualIndex(),index);
	for(int i=0;i<Entries.size();i++){
		dynamic_cast<Kdb3Database::EntryHandle*>(Entries[i])->Entry->Index=index;
	}
}

Kdb3Database::EntryHandle::EntryHandle(Kdb3Database* db){
	pDB=db;
	valid=true;
}


bool Kdb3Database::GroupHandle::isValid(){return valid;}
void Kdb3Database::GroupHandle::setOldImage(const quint32& s){Group->OldImage=s;}
QString Kdb3Database::GroupHandle::title(){return Group->Title;}
quint32	Kdb3Database::GroupHandle::oldImage(){return Group->OldImage;}
quint32	Kdb3Database::GroupHandle::image(){return Group->Image;}
int Kdb3Database::GroupHandle::index(){return Group->Index;}
void Kdb3Database::GroupHandle::setTitle(const QString& Title){Group->Title=Title;}
void Kdb3Database::GroupHandle::setExpanded(bool IsExpanded){Group->IsExpanded=IsExpanded;}
bool Kdb3Database::GroupHandle::expanded(){return Group->IsExpanded;}
void Kdb3Database::GroupHandle::setImage(const quint32& New)
{
	if(Group->Image < pDB->builtinIcons() && New >= pDB->builtinIcons())
		Group->OldImage=Group->Image;

	if(New < pDB->builtinIcons())
		Group->OldImage=New;

	Group->Image=New;
}


Kdb3Database::GroupHandle::GroupHandle(Kdb3Database* db){
	pDB=db;
	valid=true;
	Group=NULL;
}

IGroupHandle* Kdb3Database::GroupHandle::parent(){
	return (IGroupHandle*)Group->Parent->Handle;
}

int Kdb3Database::GroupHandle::level(){
	int i=0;
	StdGroup* group=Group;
	while(group->Parent){
		group=group->Parent;
		i++;
	}
	i--;
	return i;
}


QList<IGroupHandle*> Kdb3Database::GroupHandle::childs(){
	QList<IGroupHandle*> childs;
	for(int i=0;i<Group->Childs.size();i++){
		childs.append(Group->Childs[i]->Handle);
	}
	return childs;
}


void memcpyFromLEnd32(quint32* dst,const char* src){

if(QSysInfo::ByteOrder==QSysInfo::BigEndian){
  memcpy(((char*)dst)+3,src+0,1);
  memcpy(((char*)dst)+2,src+1,1);
  memcpy(((char*)dst)+1,src+2,1);
  memcpy(((char*)dst)+0,src+3,1);
}
else
  memcpy(dst,src,4);
}

void memcpyFromLEnd16(quint16* dst,const char* src){

if(QSysInfo::ByteOrder==QSysInfo::BigEndian){
  memcpy(((char*)dst)+1,src+0,1);
  memcpy(((char*)dst)+0,src+1,1);
}
else
  memcpy(dst,src,2);
}

void memcpyToLEnd32(char* dst,const quint32* src){

if(QSysInfo::ByteOrder==QSysInfo::BigEndian){
  memcpy(dst+0,((char*)src)+3,1);
  memcpy(dst+1,((char*)src)+2,1);
  memcpy(dst+2,((char*)src)+1,1);
  memcpy(dst+3,((char*)src)+0,1);
}
else
  memcpy(dst,src,4);
}

void memcpyToLEnd16(char* dst,const quint16* src){

if(QSysInfo::ByteOrder==QSysInfo::BigEndian){
  memcpy(dst+0,((char*)src)+1,1);
  memcpy(dst+1,((char*)src)+0,1);
}
else
  memcpy(dst,src,2);
}

bool Kdb3Database::save(){
	if(!Groups.size()){
		error=tr("The database must contain at least one group.");
		return false;
	}
	quint32 NumGroups,NumEntries,Signature1,Signature2,Flags,Version;
	quint8 TransfRandomSeed[32];
	quint8 FinalRandomSeed[16];
	quint8 ContentsHash[32];
	quint8 EncryptionIV[16];

	if(!(File->openMode() & QIODevice::WriteOnly)){
		File->close();
	}
	if(!File->isOpen()){
		if(!File->open(QIODevice::ReadWrite)){
			error= tr("Could not open file for writing.");
			return false;
		}
	}


/*	This is only a fix for a bug in the implementation of the metastream creation
	in KeePassX 0.2.1. to restore lost icons.
	It should be removed after a while.
	-----------------------------------------------------------------------------------*/
	for(int i=0;i<Groups.size();i++){
		if(Groups[i].Image<builtinIcons())
			Groups[i].OldImage=Groups[i].Image;}
	for(int i=0;i<Entries.size();i++){
		if(Entries[i].Image<builtinIcons())
				Entries[i].OldImage=Entries[i].Image;}
/*  ----------------------------------------------------------------------------------*/

	unsigned int FileSize;

	QList<StdEntry> MetaStreams;
	MetaStreams << StdEntry();
	createCustomIconsMetaStream(&MetaStreams.back());
	MetaStreams << StdEntry();
	createGroupTreeStateMetaStream(&MetaStreams.back());

	FileSize=DB_HEADER_SIZE;
	// Get the size of all groups (94 Byte + length of the name string)
	for(int i = 0; i < Groups.size(); i++){
		FileSize += 94 + Groups[i].Title.toUtf8().length()+1;
	}
	// Get the size of all entries
	for(int i = 0; i < Entries.size(); i++){
		FileSize
			+= 134
			+Entries[i].Title.toUtf8().length()+1
			+Entries[i].Username.toUtf8().length()+1
			+Entries[i].Url.toUtf8().length()+1
			+Entries[i].Password.length()+1
			+Entries[i].Comment.toUtf8().length()+1
			+Entries[i].BinaryDesc.toUtf8().length()+1
			+Entries[i].Binary.length();
	}

	for(int i=0; i < UnknownMetaStreams.size(); i++){
		FileSize
			+=164
			+UnknownMetaStreams[i].Comment.toUtf8().length()+1
			+UnknownMetaStreams[i].Binary.length();
	}

	for(int i=0; i < MetaStreams.size(); i++){
		FileSize
				+=164
				+MetaStreams[i].Comment.toUtf8().length()+1
				+MetaStreams[i].Binary.length();
	}


	// Round up filesize to 16-byte boundary for Rijndael/Twofish
	FileSize = (FileSize + 16) - (FileSize % 16);
	char* buffer=new char[FileSize+16];

	Signature1 = PWM_DBSIG_1;
	Signature2 = PWM_DBSIG_2;
	Flags = PWM_FLAG_SHA2;
	if(Algorithm == Rijndael_Cipher) Flags |= PWM_FLAG_RIJNDAEL;
	else if(Algorithm == Twofish_Cipher) Flags |= PWM_FLAG_TWOFISH;
	Version = PWM_DBVER_DW;
	NumGroups = Groups.size();
	NumEntries = Entries.size()+UnknownMetaStreams.size()+MetaStreams.size();

	QList<StdEntry> saveEntries = Entries;
	qSort(saveEntries.begin(),saveEntries.end(),StdEntryLessThan);

	randomize(FinalRandomSeed,16);
	randomize(TransfRandomSeed,32);
	randomize(EncryptionIV,16);

	unsigned int pos=DB_HEADER_SIZE; // Skip the header, it will be written later

	serializeGroups(Groups,buffer,pos);
	serializeEntries(saveEntries,buffer,pos);
	serializeEntries(UnknownMetaStreams,buffer,pos);
	serializeEntries(MetaStreams,buffer,pos);
	SHA256::hashBuffer(buffer+DB_HEADER_SIZE,ContentsHash,pos-DB_HEADER_SIZE);
	memcpyToLEnd32(buffer,&Signature1);
	memcpyToLEnd32(buffer+4,&Signature2);
	memcpyToLEnd32(buffer+8,&Flags);
	memcpyToLEnd32(buffer+12,&Version);
	memcpy(buffer+16,FinalRandomSeed,16);
	memcpy(buffer+32,EncryptionIV,16);
	memcpyToLEnd32(buffer+48,&NumGroups);
	memcpyToLEnd32(buffer+52,&NumEntries);
	memcpy(buffer+56,ContentsHash,32);
	memcpy(buffer+88,TransfRandomSeed,32);
	memcpyToLEnd32(buffer+120,&KeyTransfRounds);
	transformKey(RawMasterKey,MasterKey,TransfRandomSeed,KeyTransfRounds);
	quint8 FinalKey[32];

	SHA256 sha;
	sha.update(FinalRandomSeed,16);
	sha.update(MasterKey,32);
	sha.finish(FinalKey);

	unsigned long EncryptedPartSize;

	if(Algorithm == Rijndael_Cipher){
		EncryptedPartSize=((pos-DB_HEADER_SIZE)/16+1)*16;
		quint8 PadLen=EncryptedPartSize-(pos-DB_HEADER_SIZE);
		for(int i=0;i<PadLen;i++)
			((quint8*)buffer)[DB_HEADER_SIZE+EncryptedPartSize-1-i]=PadLen;
		AESencrypt aes;
		aes.key256(FinalKey);
		aes.cbc_encrypt((unsigned char*)buffer+DB_HEADER_SIZE,(unsigned char*)buffer+DB_HEADER_SIZE,EncryptedPartSize,(unsigned char*)EncryptionIV);
	}
	else{
		if(Algorithm == Twofish_Cipher){
			CTwofish twofish;
			if(twofish.init(FinalKey, 32, EncryptionIV) == false){
				UNEXP_ERROR
				delete [] buffer;
				return false;}
			EncryptedPartSize = (unsigned long)twofish.padEncrypt((quint8*)buffer+DB_HEADER_SIZE,
				pos - DB_HEADER_SIZE,(quint8*)buffer+DB_HEADER_SIZE);
		}
	}
	if((EncryptedPartSize > (0xFFFFFFE - 202)) || (!EncryptedPartSize && Groups.size())){
		UNEXP_ERROR
		delete [] buffer;
		return false;
	}

	if(!File->resize(EncryptedPartSize+DB_HEADER_SIZE)){
		delete [] buffer;
		error=decodeFileError(File->error());
		return false;	
	}
	File->seek(0);
	if(File->write(buffer,EncryptedPartSize+DB_HEADER_SIZE)!=EncryptedPartSize+DB_HEADER_SIZE){
		delete [] buffer;
		error=decodeFileError(File->error());
		return false;
	}
	File->flush();

	delete [] buffer;
	//if(SearchGroupID!=-1)Groups.push_back(SearchGroup);
	return true;
}

void Kdb3Database::createCustomIconsMetaStream(StdEntry* e){
	/* Rev 3 */
	e->BinaryDesc="bin-stream";
	e->Title="Meta-Info";
	e->Username="SYSTEM";
	e->Comment="KPX_CUSTOM_ICONS_3";
	e->Url="$";
	e->OldImage=0;
	if(Groups.size())e->GroupId=Groups[0].Id;
	int Size=12;
	quint32 NumEntries=Entries.size();
	quint32 NumGroups=Groups.size();
	Size+=8*NumGroups+20*NumEntries;
	Size+=CustomIcons.size()*1000; // 1KB
	e->Binary.reserve(Size);
	e->Binary.resize(12);
	quint32 NumIcons=CustomIcons.size();

	memcpyToLEnd32(e->Binary.data(),&NumIcons);
	memcpyToLEnd32(e->Binary.data()+4,&NumEntries);
	memcpyToLEnd32(e->Binary.data()+8,&NumGroups);
	for(int i=0;i<CustomIcons.size();i++){
		quint32 ImgSize;
		char ImgSizeBin[4];
		QByteArray png;
		png.reserve(1000);
		QBuffer buffer(&png);
		CustomIcons[i].save(&buffer,"PNG",0);
		ImgSize=png.size();
		memcpyToLEnd32(ImgSizeBin,&ImgSize);
		e->Binary.append(QByteArray::fromRawData(ImgSizeBin,4));
		e->Binary.append(png);
	}
	for(quint32 i=0;i<Entries.size();i++){
		char Bin[20];
		Entries[i].Uuid.toRaw(Bin);
		quint32 id=Entries[i].Image;
		memcpyToLEnd32(Bin+16,&id);
		e->Binary.append(QByteArray::fromRawData(Bin,20));
	}
	for(quint32 i=0;i<Groups.size();i++){
		char Bin[8];
		memcpyToLEnd32(Bin,&Groups[i].Id);
		quint32 id=Groups[i].Image;
		memcpyToLEnd32(Bin+4,&id);
		e->Binary.append(QByteArray::fromRawData(Bin,8));
	}
}

QList<IGroupHandle*> Kdb3Database::sortedGroups(){
	QList<IGroupHandle*> SortedGroups;
	appendChildsToGroupList(SortedGroups,RootGroup);
	return SortedGroups;
}


void Kdb3Database::appendChildsToGroupList(QList<IGroupHandle*>& list,StdGroup& group){
	for(int i=0;i<group.Childs.size();i++){
		list << group.Childs[i]->Handle;
		appendChildsToGroupList(list,*group.Childs[i]);
	}
}


void Kdb3Database::appendChildsToGroupList(QList<StdGroup*>& list,StdGroup& group){
	for(int i=0;i<group.Childs.size();i++){
		list << group.Childs[i];
		appendChildsToGroupList(list,*group.Childs[i]);
	}
}


void Kdb3Database::serializeGroups(QList<StdGroup>& GroupList,char* buffer,unsigned int& pos){
	quint16 FieldType;
	quint32 FieldSize;
	quint32 Flags=0; //unused
	QList<StdGroup*>SortedGroups;
	appendChildsToGroupList(SortedGroups,RootGroup);

	for(int i=0; i < SortedGroups.size(); i++){
		unsigned char Date[5];
		dateToPackedStruct5(Date_Never,Date);
		quint16 Level=0;
		StdGroup* group=SortedGroups[i];
		while(group->Parent){
			Level++;
			group=group->Parent;
		}
		Level--;

		FieldType = 0x0001; FieldSize = 4;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpyToLEnd32(buffer+pos, &SortedGroups[i]->Id); pos += 4;

		FieldType = 0x0002; FieldSize = SortedGroups[i]->Title.toUtf8().length() + 1;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpy(buffer+pos, SortedGroups[i]->Title.toUtf8(),FieldSize); pos += FieldSize;

		FieldType = 0x0003; FieldSize = 5; //Creation
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpy(buffer+pos, Date,5); pos+=5;

		FieldType = 0x0004; FieldSize = 5; //LastMod
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpy(buffer+pos, Date,5); pos+=5;

		FieldType = 0x0005; FieldSize = 5; //LastAccess
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpy(buffer+pos, Date,5); pos+=5;

		FieldType = 0x0006; FieldSize = 5; //Expire
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpy(buffer+pos, Date,5); pos+=5;

		FieldType = 0x0007; FieldSize = 4;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpyToLEnd32(buffer+pos, &SortedGroups[i]->OldImage); pos += 4;

		FieldType = 0x0008; FieldSize = 2;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpyToLEnd16(buffer+pos, &Level); pos += 2;

		FieldType = 0x0009; FieldSize = 4;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		memcpyToLEnd32(buffer+pos, &Flags); pos += 4;

		FieldType = 0xFFFF; FieldSize = 0;
		memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
	}

}


void Kdb3Database::serializeEntries(QList<StdEntry>& EntryList,char* buffer,unsigned int& pos){
	 quint16 FieldType;
	 quint32 FieldSize;
	 for(int i = 0; i < EntryList.size(); i++){
		 FieldType = 0x0001; FieldSize = 16;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 EntryList[i].Uuid.toRaw(buffer+pos);		pos += 16;

		 FieldType = 0x0002; FieldSize = 4;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpyToLEnd32(buffer+pos, &EntryList[i].GroupId); pos += 4;

		 FieldType = 0x0003; FieldSize = 4;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpyToLEnd32(buffer+pos,&EntryList[i].OldImage); pos += 4;


		 FieldType = 0x0004;
		 FieldSize = EntryList[i].Title.toUtf8().length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpy(buffer+pos, EntryList[i].Title.toUtf8(),FieldSize);  pos += FieldSize;

		 FieldType = 0x0005;
		 FieldSize = EntryList[i].Url.toUtf8().length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpy(buffer+pos, EntryList[i].Url.toUtf8(),FieldSize);  pos += FieldSize;

		 FieldType = 0x0006;
		 FieldSize = EntryList[i].Username.toUtf8().length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpy(buffer+pos, EntryList[i].Username.toUtf8(),FieldSize);  pos += FieldSize;

		 FieldType = 0x0007;
		 FieldSize = EntryList[i].Password.length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 EntryList[i].Password.unlock();
		 memcpy(buffer+pos, EntryList[i].Password.string().toUtf8(),FieldSize);  pos += FieldSize;
		 EntryList[i].Password.lock();

		 FieldType = 0x0008;
		 FieldSize = EntryList[i].Comment.toUtf8().length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpy(buffer+pos, EntryList[i].Comment.toUtf8(),FieldSize);  pos += FieldSize;

		 FieldType = 0x0009; FieldSize = 5;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 dateToPackedStruct5(EntryList[i].Creation,(unsigned char*)buffer+pos); pos+=5;


		 FieldType = 0x000A; FieldSize = 5;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 dateToPackedStruct5(EntryList[i].LastMod,(unsigned char*)buffer+pos); pos+=5;

		 FieldType = 0x000B; FieldSize = 5;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 dateToPackedStruct5(EntryList[i].LastAccess,(unsigned char*)buffer+pos); pos+=5;

		 FieldType = 0x000C; FieldSize = 5;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 dateToPackedStruct5(EntryList[i].Expire,(unsigned char*)buffer+pos); pos+=5;

		 FieldType = 0x000D;
		 FieldSize = EntryList[i].BinaryDesc.toUtf8().length() + 1; // Add terminating NULL character space
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 memcpy(buffer+pos, EntryList[i].BinaryDesc.toUtf8(),FieldSize);  pos += FieldSize;

		 FieldType = 0x000E; FieldSize = EntryList[i].Binary.length();
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
		 if((!EntryList[i].Binary.isNull()) && (FieldSize != 0))
			 memcpy(buffer+pos, EntryList[i].Binary.data(), FieldSize);
		 pos += FieldSize;

		 FieldType = 0xFFFF; FieldSize = 0;
		 memcpyToLEnd16(buffer+pos, &FieldType); pos += 2;
		 memcpyToLEnd32(buffer+pos, &FieldSize); pos += 4;
	 }
 }

bool Kdb3Database::close(){
	return true;
}

void Kdb3Database::create(){
	File=NULL;
	RootGroup.Title="$ROOT$";
	RootGroup.Parent=NULL;
	RootGroup.Handle=NULL;
	Algorithm=Rijndael_Cipher;
	KeyTransfRounds=6000;
	KeyError=false;
}

bool Kdb3Database::isKeyError(){
	if(KeyError){
		KeyError=false;
		return true;
	}
	else
		return false;
}

IEntryHandle* Kdb3Database::cloneEntry(const IEntryHandle* entry){
	StdEntry dolly;
	dolly=*((EntryHandle*)entry)->Entry;
	dolly.Uuid.generate();
	Entries.append(dolly);
	EntryHandles.append(EntryHandle(this));
	EntryHandles.back().Entry=&Entries.back();
	Entries.back().Handle=&EntryHandles.back();
	return &EntryHandles.back();
}

IEntryHandle* Kdb3Database::newEntry(IGroupHandle* group){
	StdEntry Entry;
	Entry.Uuid.generate();
	Entry.Group=((GroupHandle*)group)->Group;
	Entry.GroupId=Entry.Group->Id;
	Entries.append(Entry);
	EntryHandles.append(EntryHandle(this));
	EntryHandles.back().Entry=&Entries.back();
	Entries.back().Handle=&EntryHandles.back();
	return &EntryHandles.back();
}

IEntryHandle* Kdb3Database::addEntry(const CEntry* NewEntry, IGroupHandle* Group){
	StdEntry Entry(*((StdEntry*)NewEntry));
	Entry.Uuid.generate();
	Entry.Group=((GroupHandle*)Group)->Group;
	Entry.GroupId=Entry.Group->Id;
	Entries.append(Entry);
	EntryHandles.append(EntryHandle(this));
	EntryHandles.back().Entry=&Entries.back();
	Entries.back().Handle=&EntryHandles.back();
	return &EntryHandles.back();
}

void Kdb3Database::deleteLastEntry(){
	Entries.removeAt(Entries.size()-1);
	EntryHandles.back().invalidate();
}

bool Kdb3Database::isParent(IGroupHandle* parent, IGroupHandle* child){
	StdGroup* group=((GroupHandle*)child)->Group;
	while(group->Parent!=&RootGroup){
		if(group->Parent==((GroupHandle*)parent)->Group)return true;
		group=group->Parent;
	}
	return false;
}



void Kdb3Database::cleanUpHandles(){}

bool Kdb3Database::searchStringContains(const QString& search, const QString& string,bool Cs, bool RegExp){
	if(RegExp){
		QRegExp exp(search,Cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
		if(string.contains(exp)==0)return false;}
		else
			if(string.contains(search,Cs ? Qt::CaseSensitive : Qt::CaseInsensitive)==0)return false;

		return true;
}

void Kdb3Database::getEntriesRecursive(IGroupHandle* Group, QList<IEntryHandle*>& EntryList){
	EntryList<<entries(Group);
	for(int i=0;i<((GroupHandle*)Group)->Group->Childs.size();	i++){
		getEntriesRecursive(((GroupHandle*)Group)->Group->Childs[i]->Handle,EntryList);
	}
}

QList<IEntryHandle*> Kdb3Database::search(IGroupHandle* Group,const QString& search, bool CaseSensitive, bool RegExp, bool Recursive,bool* Fields){
	bool fields[6]={true,true,true,false,true,true};
	if(!Fields)
		Fields=fields;
	QList<IEntryHandle*> SearchEntries;
	if(search==QString())return Group ? entries(Group) : entries();
	if(Group){
		if(Recursive)
			getEntriesRecursive(Group,SearchEntries);
		else
			SearchEntries=entries(Group);
	}
	else
		SearchEntries=entries();

	for(int i=0;i<SearchEntries.size();i++){
		bool match=false;
		if(Fields[0])match=match||searchStringContains(search,SearchEntries[i]->title(),CaseSensitive,RegExp);
		if(Fields[1])match=match||searchStringContains(search,SearchEntries[i]->username(),CaseSensitive,RegExp);
		if(Fields[2])match=match||searchStringContains(search,SearchEntries[i]->url(),CaseSensitive,RegExp);
		SecString Password=SearchEntries[i]->password(); Password.unlock();
		if(Fields[3])match=match||searchStringContains(search,Password.string(),CaseSensitive,RegExp);
		Password.lock();
		if(Fields[4])match=match||searchStringContains(search,SearchEntries[i]->comment(),CaseSensitive,RegExp);
		if(Fields[5])match=match||searchStringContains(search,SearchEntries[i]->binaryDesc(),CaseSensitive,RegExp);
		if(!match){
			SearchEntries.removeAt(i);
			i--;
		}
	}

	return SearchEntries;
}

void Kdb3Database::rebuildIndices(QList<StdGroup*>& list){
	for(int i=0;i<list.size();i++){
		list[i]->Index=i;
	}
}


void Kdb3Database::moveGroup(IGroupHandle* groupHandle,IGroupHandle* NewParent,int Pos){
	StdGroup* Parent;
	StdGroup* Group=((GroupHandle*)groupHandle)->Group;
	if(NewParent)
		Parent=((GroupHandle*)NewParent)->Group;
	else
		Parent=&RootGroup;
	Group->Parent->Childs.removeAt(Group->Index);
	rebuildIndices(Group->Parent->Childs);
	Group->Parent=Parent;
	if(Pos==-1){
		Parent->Childs.append(Group);
	}
	else
	{
		Q_ASSERT(Parent->Childs.size()>=Pos);
		Parent->Childs.insert(Pos,Group);
	}
	rebuildIndices(Parent->Childs);
}

bool Kdb3Database::changeFile(const QString& filename){
	if(File)
		delete File;
	if(filename==QString()){
		File=NULL;
		return true;
	}
	File=new QFile(filename);
	if(!File->open(QIODevice::ReadWrite)){
		if(!File->open(QIODevice::ReadOnly)){
			error=decodeFileError(File->error());
			return false;
		}
	}
	return true;
}

void Kdb3Database::moveToTrash(IEntryHandle* entry){
	TrashEntry trash=*((TrashEntry*)dynamic_cast<EntryHandle*>(entry)->Entry);
	IGroupHandle* CurGroup=entry->group();
	while(CurGroup){
		trash.GroupPath << CurGroup->title();
		CurGroup=CurGroup->parent();
	}
	deleteEntry(entry);
	trash.Group=NULL;
	TrashEntries.append(trash);
	TrashHandles.append(EntryHandle(this));
	TrashHandles.back().Entry=&TrashEntries.back();
	TrashEntries.back().Handle=&TrashHandles.back();
}

void Kdb3Database::emptyTrash(){
	TrashEntries.clear();
	TrashHandles.clear();
}

QList<IEntryHandle*> Kdb3Database::trashEntries(){
	QList<IEntryHandle*> handles;
	for(int i=0; i<TrashHandles.size();i++)
		if(TrashHandles[i].isValid())
			handles << &TrashHandles[i];
	return handles;
}
