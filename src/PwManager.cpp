/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
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

#include "global.h"
#include <iostream>
#include <time.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qobject.h>
#include <qdatetime.h>
#include "crypto/sha256.h"
#include "crypto/rijndael.h"
#include "crypto/twoclass.h"
#include "lib/random.h"
using namespace std;
#include "PwManager.h"

QString PwDatabase::getError(){
if(Errors.size()){
QString r=Errors.front();
Errors.pop_front();
return r;
}
else return QString(trUtf8("unbekannter Fehler"));
}

QString PwDatabase::getErrors(){
QString r;
for(int i=0; i<Errors.size(); i++){
r+=Errors[i];
r+='\n';
}
Errors.clear();
return r;
}

bool PwDatabase::loadDatabase(QString _filename, QString& err){
unsigned long total_size,crypto_size;
Q_UINT32 Signature1,Signature2,Version,NumGroups,NumEntries,Flags;
Q_UINT8 TrafoRandomSeed[32];
Q_UINT8 FinalRandomSeed[16];
Q_UINT8 ContentsHash[32];
Q_UINT8 EncryptionIV[16];

filename=_filename;
QFile file(filename);
file.open(QIODevice::ReadOnly);
total_size=file.size();
char* buffer = new char[total_size];
file.readBlock(buffer,total_size);
file.close();

if(total_size < DB_HEADER_SIZE){
err=trUtf8("Unerwartete Dateigröße (Dateigröße < DB_HEADER_SIZE)");
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
memcpy(TrafoRandomSeed,buffer+88,32);
memcpyFromLEnd32(&KeyEncRounds,buffer+120);

if((Signature1!=PWM_DBSIG_1) || (Signature2!=PWM_DBSIG_2)){
err=trUtf8("Falsche Signatur");
return false;}


if((Version & 0xFFFFFF00) != (PWM_DBVER_DW & 0xFFFFFF00)){
	err=trUtf8("Nicht unterstüzte Dateiversion");
	return false;}

if(Flags & PWM_FLAG_RIJNDAEL) CryptoAlgorithmus = ALGO_AES;
else if(Flags & PWM_FLAG_TWOFISH) CryptoAlgorithmus = ALGO_TWOFISH;
else {
 err=trUtf8("Unbekannter Verschlüsselungsalgorithmus");
 return false;
 }

transformKey(MasterKey,TransformedMasterKey,TrafoRandomSeed,KeyEncRounds);
Q_UINT8 FinalKey[32];
sha256_context sha32;
sha256_starts(&sha32);
sha256_update(&sha32,FinalRandomSeed, 16);
sha256_update(&sha32,TransformedMasterKey, 32);
sha256_finish(&sha32,FinalKey);

if(CryptoAlgorithmus == ALGO_AES)
	{
		Rijndael aes;
		// Initialize Rijndael algorithm
		if(aes.init(Rijndael::CBC, Rijndael::Decrypt, FinalKey,
			Rijndael::Key32Bytes, EncryptionIV) != RIJNDAEL_SUCCESS)
			{err=trUtf8("AES-Initialisierung fehlgeschlagen");
			 return false;}
		// Decrypt! The first bytes aren't encrypted (that's the header)
		crypto_size = (unsigned long)aes.padDecrypt((Q_UINT8 *)buffer + DB_HEADER_SIZE,
			total_size  - DB_HEADER_SIZE, (Q_UINT8 *)buffer + DB_HEADER_SIZE);
	}
else if(CryptoAlgorithmus == ALGO_TWOFISH)
	{
		CTwofish twofish;
		if(twofish.init(FinalKey, 32, EncryptionIV) != true){return false;}
		crypto_size = (unsigned long)twofish.padDecrypt((Q_UINT8 *)buffer + DB_HEADER_SIZE,
			total_size - DB_HEADER_SIZE, (Q_UINT8 *)buffer + DB_HEADER_SIZE);
	}

if((crypto_size > 2147483446) || (crypto_size == 0)){err=trUtf8("Entschlüsselung nicht möglich - der Schlüssel ist falsch oder die Datei beschädigt."); return false;}

sha256_starts(&sha32);
sha256_update(&sha32,(unsigned char *)buffer + DB_HEADER_SIZE,crypto_size);
sha256_finish(&sha32,(unsigned char *)FinalKey);

if(memcmp(ContentsHash, FinalKey, 32) != 0)
{
err=trUtf8("Hash-Test fehlgeschlage: der Schlüssl ist falsch oder die Datei ist beschädigt.");
return false;}


Groups.resize(NumGroups);
Entries.resize(NumEntries);

unsigned long tmp_id=0;
unsigned long pos = DB_HEADER_SIZE;
Q_UINT16 FieldType;
Q_UINT32 FieldSize;
char* pField;
bool bRet;


	for(unsigned long CurGroup = 0; CurGroup < NumGroups; )
	{
		pField = buffer+pos;

		memcpyFromLEnd16(&FieldType, pField);
		pField += 2; pos += 2;
		if(pos >= total_size) {
		 return false; }

		memcpyFromLEnd32(&FieldSize, pField);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)) {
		return false;}

	 	bRet = Groups[CurGroup].ReadGroupField(FieldType, FieldSize, (Q_UINT8 *)pField);
		if((FieldType == 0xFFFF) && (bRet == true)){
			CurGroup++;} // Now and ONLY now the counter gets increased

		pField += FieldSize;
		pos += FieldSize;
		if(pos >= total_size) { return false;}
	}


	for(unsigned long CurEntry = 0; CurEntry < NumEntries;)
	{
		pField = buffer+pos;

		memcpyFromLEnd16(&FieldType, pField);
		pField += 2; pos += 2;
		if(pos >= total_size){
		 return false;}

		memcpyFromLEnd32(&FieldSize, pField);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)) {
		return false; }

		bRet = Entries[CurEntry].ReadEntryField(FieldType,FieldSize,(Q_UINT8*)pField);
		if((FieldType == 0xFFFF) && (bRet == true)){
			Entries[CurEntry].sID=tmp_id++;
			CurEntry++;} // Now and ONLY now the counter gets increased

		pField += FieldSize;
		pos += FieldSize;
		if(pos >= total_size) {
		return false; }
	}

unsigned long CurGID, g, e, z, num;
delete [] buffer;

for(vector<CEntry>::iterator i=Entries.begin();i!=Entries.end();i++){
if(IsMetaStream(*i)==true){
	///@TODO Parse Metastreams
	deleteEntry(i);
	i--;
}
}
return true;
}


void PwDatabase::transformKey(Q_UINT8* src,Q_UINT8* dst,Q_UINT8* KeySeed,int rounds){
Q_UINT8* tmp=new Q_UINT8[32];
Rijndael rijndael;
sha256_context sha2;
if(rijndael.init(Rijndael::ECB, Rijndael::Encrypt, (const Q_UINT8 *)KeySeed,
                 Rijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS){
  cout << QString("unexpected error in %1, line %2").arg(__FILE__).arg(__LINE__).ascii() << endl;
  exit(1);}

memcpy(tmp,src,32);
for(int i=0;i<rounds;i++){
 rijndael.blockEncrypt((const Q_UINT8 *)tmp, 256, (Q_UINT8 *)tmp);
}

sha256_starts(&sha2);
sha256_update(&sha2,tmp,32);
sha256_finish(&sha2,tmp);

memcpy(dst,tmp,32);
delete [] tmp;
}

bool PwDatabase::CalcMasterKeyByPassword(QString& Password){

unsigned long KeyLen, FileSize, Read;
sha256_context sha32;
char *paKey = NULL;

if(Password == QString::null) return false;

paKey = new char[Password.length() + 1];
if(paKey == NULL) return false;
strcpy(paKey, Password);



if(paKey == NULL) return false;

KeyLen = strlen(paKey);

if(KeyLen == 0) {
SecString::overwrite(paKey,Password.length() + 1);
delete [] paKey;
return false; }

sha256_starts(&sha32);
sha256_update(&sha32,(unsigned char*) paKey, KeyLen);
sha256_finish(&sha32,MasterKey);
SecString::overwrite(paKey,Password.length() + 1);
delete [] paKey;
return true;
 }


bool PwDatabase::CalcMasterKeyByFile(QString filename){

QFile file(filename);
if(file.open(QIODevice::ReadOnly) == false) return false;
unsigned long FileSize=file.size();

if(FileSize == 32){
	if(file.readBlock((char*)MasterKey,32) != 32){
	  file.close();
	  return false;}
}
else if(FileSize == 64){
	char hex[64];
	if(file.readBlock(hex,64) != 64){
	  file.close();
	  return false;}
	file.close();
	if(!convHexToBinaryKey(hex,(char*)MasterKey)) return false;

}
else
{
sha256_context sha32;
sha256_starts(&sha32);
unsigned char* buffer = new unsigned char[2048];
while(1)
 {
 unsigned long read=file.readBlock((char*)buffer,2048);
 if(read == 0) break;
 sha256_update(&sha32,buffer,read);
 if(read != 2048) break;
}
sha256_finish(&sha32,MasterKey);
delete [] buffer;
}

file.close();
return true;
}


CEntry* PwDatabase::addEntry(){
CEntry NewEntry;
if(Entries.size()==0){
 NewEntry.sID=0;
 getRandomBytes(&NewEntry.ID,16,1,false);
 }
 else {
 NewEntry.sID=(*(Entries.end()-1)).sID+1;
 while(1){
 bool used=false;
 getRandomBytes(&NewEntry.ID,16,1,false);
  for(int j=0;j<Entries.size();j++){
	int k;
	for(k=0;k<16;k++){if(Entries[j].ID[k]!=NewEntry.ID[k])k=0;break;}
	if(k==15)used=true;}
 if(used==false)break;
 }}
Entries.push_back(NewEntry);
return &Entries.back();
}


bool PwDatabase::CalcMasterKeyByFileAndPw(QString filename, QString& Password){
Q_UINT8* FileKey;
Q_UINT8* PasswordKey;
PasswordKey=new Q_UINT8[32];
FileKey=new Q_UINT8[32];
sha256_context sha32;
/////////////////////////
QFile file(filename);
if(file.open(QIODevice::ReadOnly) == false) return false;
unsigned long FileSize=file.size();
if(FileSize == 32){
	if(file.readBlock((char*)FileKey,32) != 32){
	   file.close();
	   return false;}
}
else{
sha256_starts(&sha32);
unsigned char* buffer = new unsigned char[2048];
while(1)
 {
 unsigned long read=file.readBlock((char*)buffer,2048);
 if(read == 0) break;
 sha256_update(&sha32,buffer,read);
 if(read != 2048) break;
}
sha256_finish(&sha32,FileKey);
delete [] buffer;
}
file.close();

//////////////////////

unsigned long KeyLen;
char *paKey = NULL;
if(Password == QString::null) return false;
paKey = new char[Password.length() + 1];
if(paKey == NULL) return false;
strcpy(paKey, Password);
if(paKey == NULL) return false;
KeyLen = strlen(paKey);
if(KeyLen == 0) {
delete [] paKey;
return false; }
sha256_starts(&sha32);
sha256_update(&sha32,(unsigned char*) paKey, KeyLen);
sha256_finish(&sha32,PasswordKey);
delete [] paKey;
////////////////////
sha256_starts(&sha32);
sha256_update(&sha32,(unsigned char*)PasswordKey,32);
sha256_update(&sha32,(unsigned char*)FileKey,32);
sha256_finish(&sha32,MasterKey);
delete[] PasswordKey;
delete[] FileKey;
}


vector<CEntry>::iterator PwDatabase::deleteEntry(vector<CEntry>::iterator iterator){
return Entries.erase(iterator);
}

vector<CEntry>::iterator PwDatabase::deleteEntry(CEntry* entry){
return deleteEntry(getEntryIterator(entry));
}

bool PwDatabase::IsMetaStream(CEntry& p){

if(p.pBinaryData == NULL) return false;
if(p.Additional == "") return false;
if(p.BinaryDesc == "") return false;
if(p.BinaryDesc != "bin-stream") return false;
if(p.Title == "") return false;
if(p.Title != "Meta-Info") return false;
if(p.UserName == "") return false;
if(p.UserName != "SYSTEM") return false;
if(p.URL == "") return false;
if(p.URL != "$") return false;
if(p.ImageID != 0) return false;
return true;
}



void PwDatabase::moveEntry(CEntry* entry,CGroup* dst){
entry->GroupID=dst->ID;
}



bool CGroup::ReadGroupField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData)
{

	switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpyFromLEnd32(&ID, (char*)pData);
		break;
	case 0x0002:
		//Name.fromUtf8((char*)pData);
		Name=QString::fromUtf8((char*)pData);
		break;
	case 0x0003:
		Creation=dateFromPackedStruct5(pData);
		break;
	case 0x0004:
		LastMod=dateFromPackedStruct5(pData);
		break;
	case 0x0005:
		LastAccess=dateFromPackedStruct5(pData);
		break;
	case 0x0006:
		Expire=dateFromPackedStruct5(pData);
		break;
	case 0x0007:
		memcpyFromLEnd32(&ImageID, (char*)pData);
		break;
	case 0x0008:
		memcpyFromLEnd16(&Level, (char*)pData);
		break;
	case 0x0009:
		memcpyFromLEnd32(&Flags, (char*)pData);
		break;
	case 0xFFFF:
		break;
	default:
		return false; // Field unsupported
	}

	return true; // Field supported
}

PwDatabase::PwDatabase(){
SearchGroupID=-1;
}

PwDatabase::~PwDatabase(){

}


bool CEntry::ReadEntryField(Q_UINT16 FieldType, Q_UINT32 FieldSize, Q_UINT8 *pData){


switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpy(ID, pData, 16);
		break;
	case 0x0002:
		memcpyFromLEnd32(&GroupID, (char*)pData);
		break;
	case 0x0003:
		memcpyFromLEnd32(&ImageID, (char*)pData);
		break;
	case 0x0004:
		//Title=(char*)pData;
		Title=QString::fromUtf8((char*)pData);
		break;
	case 0x0005:
		URL=QString::fromUtf8((char*)pData);
		break;
	case 0x0006:
		UserName=QString::fromUtf8((char*)pData);
		break;
	case 0x0007:{
		QString s=QString::fromUtf8((char*)pData);
		Password.setString(s,true);
		break;}
	case 0x0008:
		Additional=QString::fromUtf8((char*)pData);
		break;
	case 0x0009:
		Creation=dateFromPackedStruct5(pData);
		break;
	case 0x000A:
		LastMod=dateFromPackedStruct5(pData);
		break;
	case 0x000B:
		LastAccess=dateFromPackedStruct5(pData);
		break;
	case 0x000C:
		Expire=dateFromPackedStruct5(pData);
		break;
	case 0x000D:
		BinaryDesc=(char*)pData;
		break;
	case 0x000E:
		if(FieldSize != 0)
		{
			///@TODO: im Destruktor löschen
			///@TODO: im Konstruktor auf Null
			pBinaryData = new Q_UINT8[FieldSize];
			memcpy(pBinaryData, pData, FieldSize);
			BinaryDataLength = FieldSize;
		}
		else
		{pBinaryData=0;}
		break;
	case 0xFFFF:
		///@TODO: Alle Elemente geladen - Status setzen oder so was
		break;
	default:
		return false; // Field unsupported
	}

	return true; // Field processed
}

bool PwDatabase::closeDatabase(){
Groups.clear();
Entries.clear();
return true;
}

bool PwDatabase::saveDatabase(){
CGroup SearchGroup;
Q_UINT32 NumGroups,NumEntries,Signature1,Signature2,Flags,Version;
Q_UINT8 TrafoRandomSeed[32];
Q_UINT8 FinalRandomSeed[16];
Q_UINT8 ContentsHash[32];
Q_UINT8 EncryptionIV[16];

if(SearchGroupID!=-1){
 for(int i=0;i<Groups.size();i++){
  if(Groups[i].ID==SearchGroupID){
   SearchGroup=Groups[i];
   Groups.erase(getGroupIterator(&Groups[i]));}
 }
}
if(filename==QString::null)return false;
QFile file(filename);
unsigned int FileSize;

//->Add Metastreams
FileSize=DB_HEADER_SIZE;
// Get the size of all groups (94 Byte + length of the name string)
for(int i = 0; i < Groups.size(); i++){
  FileSize += 94 + Groups[i].Name.utf8().length()+1;
}
// Get the size of all entries
for(int i = 0; i < Entries.size(); i++){
  FileSize += 134
	   +Entries[i].Title.utf8().length()+1
	   +Entries[i].UserName.utf8().length()+1
	   +Entries[i].URL.utf8().length()+1
	   +Entries[i].Password.length()+1
	   +Entries[i].Additional.utf8().length()+1
	   +Entries[i].BinaryDesc.utf8().length()+1
	   +Entries[i].BinaryDataLength;}
// Round up filesize to 16-byte boundary for Rijndael/Twofish
FileSize = (FileSize + 16) - (FileSize % 16);
char* buffer=new char[FileSize+16];


Signature1 = PWM_DBSIG_1;
Signature2 = PWM_DBSIG_2;
Flags = PWM_FLAG_SHA2;
if(CryptoAlgorithmus == ALGO_AES) Flags |= PWM_FLAG_RIJNDAEL;
else if(CryptoAlgorithmus == ALGO_TWOFISH) Flags |= PWM_FLAG_TWOFISH;
Version = PWM_DBVER_DW;
NumGroups = Groups.size(); //-> (+MetaStreams)
NumEntries = Entries.size();

getRandomBytes(FinalRandomSeed,1,16,false);
getRandomBytes(TrafoRandomSeed,1,32,false);
getRandomBytes(EncryptionIV,1,16,false);

Q_UINT16 FieldType;
Q_UINT32 FieldSize;
int pos=DB_HEADER_SIZE; // Skip the header, it will be written later

for(int i=0; i < Groups.size(); i++){
		FieldType = 0x0001; FieldSize = 4;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Groups[i].ID, 4); pos += 4;

		FieldType = 0x0002; FieldSize = Groups[i].Name.utf8().length() + 1;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Groups[i].Name.utf8(),FieldSize); pos += FieldSize;

		FieldType = 0x0003; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Groups[i].Creation,(unsigned char*)buffer+pos); pos += 5;

		FieldType = 0x0004; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Groups[i].LastMod,(unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0005; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Groups[i].LastAccess,(unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0006; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Groups[i].Expire,(unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0007; FieldSize = 4;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Groups[i].ImageID, 4); pos += 4;

		FieldType = 0x0008; FieldSize = 2;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Groups[i].Level, 2); pos += 2;

		FieldType = 0x0009; FieldSize = 4;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Groups[i].Flags, 4); pos += 4;

		FieldType = 0xFFFF; FieldSize = 0;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
}

for(int i = 0; i < Entries.size(); i++){
		FieldType = 0x0001; FieldSize = 16;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Entries[i].ID, 16); pos += 16;

		FieldType = 0x0002; FieldSize = 4;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Entries[i].GroupID, 4); pos += 4;

		FieldType = 0x0003; FieldSize = 4;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, &Entries[i].ImageID, 4); pos += 4;


		FieldType = 0x0004;
		FieldSize = Entries[i].Title.utf8().length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].Title.utf8(),FieldSize);  pos += FieldSize;

		FieldType = 0x0005;
		FieldSize = Entries[i].URL.utf8().length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].URL.utf8(),FieldSize);  pos += FieldSize;

		FieldType = 0x0006;
		FieldSize = Entries[i].UserName.utf8().length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].UserName.utf8(),FieldSize);  pos += FieldSize;

		FieldType = 0x0007;
		FieldSize = Entries[i].Password.length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].Password.getString(),FieldSize);  pos += FieldSize;
		Entries[i].Password.delRef();

		FieldType = 0x0008;
		FieldSize = Entries[i].Additional.utf8().length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].Additional.utf8(),FieldSize);  pos += FieldSize;

		FieldType = 0x0009; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Entries[i].Creation,(unsigned char*)buffer+pos); pos+=5;


		FieldType = 0x000A; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Entries[i].LastMod,(unsigned char*)buffer+pos); pos+=5;

		FieldType = 0x000B; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Entries[i].LastAccess,(unsigned char*)buffer+pos); pos+=5;

		FieldType = 0x000C; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		dateToPackedStruct5(Entries[i].Expire,(unsigned char*)buffer+pos); pos+=5;

		FieldType = 0x000D;
		FieldSize = Entries[i].BinaryDesc.utf8().length() + 1; // Add terminating NULL character space
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		memcpy(buffer+pos, Entries[i].BinaryDesc.utf8(),FieldSize);  pos += FieldSize;

		FieldType = 0x000E; FieldSize = Entries[i].BinaryDataLength;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		if((Entries[i].pBinaryData != NULL) && (FieldSize != 0))
			memcpy(buffer+pos, Entries[i].pBinaryData, FieldSize);
		pos += FieldSize;

		FieldType = 0xFFFF; FieldSize = 0;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
}
sha256_context context;
sha256_starts(&context);
sha256_update(&context,(unsigned char*)buffer+DB_HEADER_SIZE, pos - DB_HEADER_SIZE);
sha256_finish(&context,(unsigned char*)ContentsHash);
memcpy(buffer,&Signature1,4);
memcpy(buffer+4,&Signature2,4);
memcpy(buffer+8,&Flags,4);
memcpy(buffer+12,&Version,4);
memcpy(buffer+16,FinalRandomSeed,16);
memcpy(buffer+32,EncryptionIV,16);
memcpy(buffer+48,&NumGroups,4);
memcpy(buffer+52,&NumEntries,4);
memcpy(buffer+56,ContentsHash,32);
memcpy(buffer+88,TrafoRandomSeed,32);
memcpy(buffer+120,&KeyEncRounds,4);
transformKey(MasterKey,TransformedMasterKey,TrafoRandomSeed,KeyEncRounds);
Q_UINT8 FinalKey[32];
sha256_starts(&context);
sha256_update(&context,FinalRandomSeed, 16);
sha256_update(&context,TransformedMasterKey, 32);
sha256_finish(&context,FinalKey);

unsigned long EncryptedPartSize;

if(CryptoAlgorithmus == ALGO_AES){
Rijndael aes;
// Initialize Rijndael/AES
  if(aes.init(Rijndael::CBC, Rijndael::Encrypt, FinalKey,Rijndael::Key32Bytes, EncryptionIV) != RIJNDAEL_SUCCESS){
	//TODO:ERR_MSG
	delete [] buffer;
	return false;}
EncryptedPartSize = (unsigned long)aes.padEncrypt((Q_UINT8*)buffer+DB_HEADER_SIZE,
						  pos - DB_HEADER_SIZE,
						  (Q_UINT8*)buffer+DB_HEADER_SIZE);
}else if(CryptoAlgorithmus == ALGO_TWOFISH){
CTwofish twofish;
if(twofish.init(FinalKey, 32, EncryptionIV) == false){
//TODO:ERR_MSG
delete [] buffer;
return false;}
EncryptedPartSize = (unsigned long)twofish.padEncrypt((Q_UINT8*)buffer+DB_HEADER_SIZE,
						      pos - DB_HEADER_SIZE,
						      (Q_UINT8*)buffer+DB_HEADER_SIZE);
}

if((EncryptedPartSize > 2147483446) || (EncryptedPartSize == 0)){
//TODO:ERR_MSG
delete [] buffer;
return false;
}

if(file.open(QIODevice::ReadWrite | QIODevice::Truncate)==false){
//TODO:ERR_MSG
delete [] buffer;
return false;
}

if(file.writeBlock(buffer,EncryptedPartSize+DB_HEADER_SIZE)!=EncryptedPartSize+DB_HEADER_SIZE){
//TODO:ERR_MSG
file.close();
delete [] buffer;
return false;
}

file.close();
delete [] buffer;
if(SearchGroupID!=-1)Groups.push_back(SearchGroup);
return true;
}

GroupItr PwDatabase::getGroupIterator(CGroup* pGroup){
//for(vector<CGroup>::iterator i=Groups.begin();i!=Groups.end();i++){
//if((*i).ID==pGroup->ID)return i;}
return Groups.begin()+(pGroup-&Groups[0]);
}


EntryItr PwDatabase::getEntryIterator(CEntry* pEntry){
return Entries.begin()+(pEntry-&Entries[0]);
}

CGroup* PwDatabase::addGroup(CGroup* parent){
CGroup group;
if(parent)group.Level=parent->Level+1;
group.ImageID=0;
group.ID=getNewGroupId();

if(!Groups.size() || !parent){
	Groups.push_back(group);
	return &Groups.back();
}
else {  GroupItr groupIt;
	groupIt=Groups.insert(getGroupIterator(parent)+1,group);
	return &(*groupIt);}
}


GroupItr PwDatabase::deleteGroup(unsigned long id){
for(int i=0;i<Groups.size();i++){
	if(Groups[i].ID==id) return deleteGroup(&Groups[i]);
}
}

GroupItr PwDatabase::deleteGroup(CGroup* group){
GroupItr first=getGroupIterator(group);
GroupItr last=Groups.end();
for(GroupItr i=getGroupIterator(group)+1; i!=Groups.end();i++){
 if((*i).Level<=group->Level){
	last=i;
	break;}
}

for(GroupItr g=first;g!=last;g++){
   for(EntryItr e=Entries.begin();e!=Entries.end();){
	if((*g).ID==(*e).GroupID)e=deleteEntry(e);
	else e++;
   }
}

return Groups.erase(first,last);
}




int PwDatabase::getGroupIndex(unsigned long ID){
int i=0;
for(i;i<Groups.size();i++){
if(Groups[i].ID==ID)break;
}
return i;
}


void PwDatabase::merge(PwDatabase* db){
for(int i=0;i<db->Groups.size();i++){
	int NewGroupID;
	if(isGroupIdInUse(db->Groups[i].ID)==true) NewGroupID=getNewGroupId();
	else NewGroupID=db->Groups[i].ID;
	for(int j=0;j<db->Entries.size();j++){
		if(db->Entries[j].GroupID==db->Groups[i].ID){
			Entries.push_back(db->Entries[j]);
			Entries.back().GroupID=NewGroupID;
			Entries.back().sID=getNewEntrySid();}
	}
	Groups.push_back(db->Groups[i]);
	Groups.back().ID=NewGroupID;
}
}

bool PwDatabase::isGroupIdInUse(Q_UINT32 id){
for(int i=0;i<Groups.size();i++)
 if(Groups[i].ID==id)return true;

return false;
}

bool PwDatabase::isEntrySidInUse(Q_UINT32 sid){
for(int i=0;i<Entries.size();i++)
 if(Entries[i].sID==sid)return true;

return false;
}


Q_UINT32 PwDatabase::getNewGroupId(){
Q_UINT32 id;
bool used;
while(1){
used=false;
getRandomBytes(&id,4,1,false);
if(!id)continue; //darf nicht 0 sein (KDB-Format v3)
  for(int j=0;j<Groups.size();j++){
	if(Groups[j].ID==id)used=true;}
if(used==false)break;}
return id;
}

Q_UINT32 PwDatabase::getNewEntrySid(){
Q_UINT32 sid;
while(1){
getRandomBytes(&sid,4,1,false);
if(!sid)continue;
if(!isEntrySidInUse(sid))break;
}
return sid;
}

bool PwDatabase::convHexToBinaryKey(char* HexKey, char* dst){
QString hex=QString::fromAscii(HexKey,64);
for(int i=0; i<64; i+=2){
	bool err;
	Q_UINT8 bin;
	bin=hex.mid(i,2).toUInt(&err,16);
	if(!err){
		qWarning("Invalid Hex Key\n");
		return false;}
	memcpy(dst+(i/2),&bin,1);
}
}

void memcpyFromLEnd32(Q_UINT32* dst,char* src){
#ifdef KEEPASS_LITTLE_ENDIAN
  memcpy(dst,src,4);
#endif
#ifdef KEEPASS_BIG_ENDIAN
  memcpy(dst+3,src+0,1);
  memcpy(dst+2,src+1,1);
  memcpy(dst+1,src+2,1);
  memcpy(dst+0,src+3,1);
#endif
}

void memcpyFromLEnd16(Q_UINT16* dst,char* src){
#ifdef KEEPASS_LITTLE_ENDIAN
  memcpy(dst,src,2);
#endif
#ifdef KEEPASS_BIG_ENDIAN
  memcpy(dst+1,src+0,1);
  memcpy(dst+0,src+1,1);
#endif
}

const QDateTime Date_Never(QDate(2999,12,28),QTime(23,59,59));

QDateTime dateFromPackedStruct5(const unsigned char* pBytes){
Q_UINT32 dw1, dw2, dw3, dw4, dw5;
dw1 = (Q_UINT32)pBytes[0]; dw2 = (Q_UINT32)pBytes[1]; dw3 = (Q_UINT32)pBytes[2];
dw4 = (Q_UINT32)pBytes[3]; dw5 = (Q_UINT32)pBytes[4];
int y = (dw1 << 6) | (dw2 >> 2);
int mon = ((dw2 & 0x00000003) << 2) | (dw3 >> 6);
int d = (dw3 >> 1) & 0x0000001F;
int h = ((dw3 & 0x00000001) << 4) | (dw4 >> 4);
int min = ((dw4 & 0x0000000F) << 2) | (dw5 >> 6);
int s = dw5 & 0x0000003F;
return QDateTime(QDate(y,mon,d),QTime(h,min));
}


void dateToPackedStruct5(const QDateTime& d,unsigned char* pBytes){
pBytes[0] = (Q_UINT8)(((Q_UINT32)d.date().year() >> 6) & 0x0000003F);
pBytes[1] = (Q_UINT8)((((Q_UINT32)d.date().year() & 0x0000003F) << 2) | (((Q_UINT32)d.date().month() >> 2) & 0x00000003));
pBytes[2] = (Q_UINT8)((((Q_UINT32)d.date().month() & 0x00000003) << 6) | (((Q_UINT32)d.date().day() & 0x0000001F) << 1) | (((Q_UINT32)d.time().hour() >> 4) & 0x00000001));
pBytes[3] = (Q_UINT8)((((Q_UINT32)d.time().hour() & 0x0000000F) << 4) | (((Q_UINT32)d.time().minute() >> 2) & 0x0000000F));
pBytes[4] = (Q_UINT8)((((Q_UINT32)d.time().minute() & 0x00000003) << 6) | ((Q_UINT32)d.time().second() & 0x0000003F));
}