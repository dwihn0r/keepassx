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

#include <iostream.h>
#include <time.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qobject.h>
#include <qdatetime.h>
#include "crypto/sha256.h"
#include "crypto/rijndael.h"
#include "crypto/twoclass.h"
#include "lib/random.h"

#include "PwManager.h"

bool PwDatabase::loadDatabase(QString _filename, QString& err){
unsigned long total_size,crypto_size;
UINT32 Signature1,Signature2,Version,NumGroups,NumEntries,Flags;
UINT8 TrafoRandomSeed[32];
UINT8 FinalRandomSeed[16];
UINT8 ContentsHash[32];
UINT8 EncryptionIV[16];

filename=_filename;
QFile file(filename);
file.open(IO_ReadOnly);
total_size=file.size();
char* buffer = new char[total_size];
file.readBlock(buffer,total_size);
file.close();

if(total_size < DB_HEADER_SIZE){
err=trUtf8("Unerwartete Dateigröße (Dateigröße < DB_HEADER_SIZE)");
return false; }

memcpy(&Signature1,buffer,4);
memcpy(&Signature2,buffer+4,4);
memcpy(&Flags,buffer+8,4);
memcpy(&Version,buffer+12,4);
memcpy(FinalRandomSeed,buffer+16,16);
memcpy(EncryptionIV,buffer+32,16);
memcpy(&NumGroups,buffer+48,4);
memcpy(&NumEntries,buffer+52,4);
memcpy(ContentsHash,buffer+56,32);
memcpy(TrafoRandomSeed,buffer+88,32);
memcpy(&KeyEncRounds,buffer+120,4);

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
UINT8 FinalKey[32];
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
		crypto_size = (unsigned long)aes.padDecrypt((UINT8 *)buffer + DB_HEADER_SIZE,
			total_size  - DB_HEADER_SIZE, (UINT8 *)buffer + DB_HEADER_SIZE);
	}
else if(CryptoAlgorithmus == ALGO_TWOFISH)
	{
		CTwofish twofish;
		if(twofish.init(FinalKey, 32, EncryptionIV) != true){return false;}
		crypto_size = (unsigned long)twofish.padDecrypt((UINT8 *)buffer + DB_HEADER_SIZE,
			total_size - DB_HEADER_SIZE, (UINT8 *)buffer + DB_HEADER_SIZE);
	}

if((crypto_size > 2147483446) || (crypto_size == 0)){err=trUtf8("Unerwarteter Wert für 'crypto_size'"); return false;}

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
UINT16 FieldType;
UINT32 FieldSize;
char* pField;
bool bRet;


	for(unsigned long CurGroup = 0; CurGroup < NumGroups; )
	{
		pField = buffer+pos;

		memcpy(&FieldType, pField, 2);
		pField += 2; pos += 2;
		if(pos >= total_size) {
		 return false; }

		memcpy(&FieldSize, pField, 4);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)) {
		return false;}

	 	bRet = Groups[CurGroup].ReadGroupField(FieldType, FieldSize, (UINT8 *)pField);
		if((FieldType == 0xFFFF) && (bRet == true)){
			CurGroup++;} // Now and ONLY now the counter gets increased

		pField += FieldSize;
		pos += FieldSize;
		if(pos >= total_size) { return false;}
	}


	for(unsigned long CurEntry = 0; CurEntry < NumEntries;)
	{
		pField = buffer+pos;

		memcpy(&FieldType, pField, 2);
		pField += 2; pos += 2;
		if(pos >= total_size){
		 return false;}

		memcpy(&FieldSize, pField, 4);
		pField += 4; pos += 4;
		if(pos >= (total_size + FieldSize)) {
		return false; }

		bRet = Entries[CurEntry].ReadEntryField(FieldType,FieldSize,(UINT8*)pField);
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


void PwDatabase::transformKey(UINT8* src,UINT8* dst,UINT8* KeySeed,int rounds){
UINT8* tmp=new UINT8[32];
Rijndael rijndael;
sha256_context sha2;
if(rijndael.init(Rijndael::ECB, Rijndael::Encrypt, (const UINT8 *)KeySeed,
                 Rijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS){
  cout << QString("unexpected error in %1, line %2").arg(__FILE__).arg(__LINE__) << endl;
  exit(1);}

memcpy(tmp,src,32);
for(int i=0;i<rounds;i++){
 rijndael.blockEncrypt((const UINT8 *)tmp, 256, (UINT8 *)tmp);
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
if(file.open(IO_ReadOnly) == false) return false;
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
UINT8* FileKey;
UINT8* PasswordKey;
PasswordKey=new UINT8[32];
FileKey=new UINT8[32];
sha256_context sha32;
/////////////////////////
QFile file(filename);
if(file.open(IO_ReadOnly) == false) return false;
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
if(p.Additional == NULL) return false;
if(p.BinaryDesc == NULL) return false;
if(p.BinaryDesc != "bin-stream") return false;
if(p.Title == NULL) return false;
if(p.Title != "Meta-Info") return false;
if(p.UserName == NULL) return false;
if(p.UserName != "SYSTEM") return false;
if(p.URL == NULL) return false;
if(p.URL != "$") return false;
if(p.ImageID != 0) return false;
return true;
}



void PwDatabase::moveEntry(CEntry* entry,CGroup* dst){
entry->GroupID=dst->ID;
}



bool CGroup::ReadGroupField(UINT16 FieldType, UINT32 FieldSize, UINT8 *pData)
{

	switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpy(&ID, pData, 4);
		break;
	case 0x0002:
		//Name.fromUtf8((char*)pData);
		Name=QString::fromUtf8((char*)pData);
		break;
	case 0x0003:
		Creation.Set(pData);
		break;
	case 0x0004:
		LastMod.Set(pData);
		break;
	case 0x0005:
		LastAccess.Set(pData);
		break;
	case 0x0006:
		Expire.Set(pData);
		break;
	case 0x0007:
		memcpy(&ImageID, pData, 4);
		break;
	case 0x0008:
		memcpy(&Level, pData, 2);
		break;
	case 0x0009:
		memcpy(&Flags, pData, 4);
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


bool CEntry::ReadEntryField(UINT16 FieldType, UINT32 FieldSize, UINT8 *pData){


switch(FieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpy(ID, pData, 16);
		break;
	case 0x0002:
		memcpy(&GroupID, pData, 4);
		break;
	case 0x0003:
		memcpy(&ImageID, pData, 4);
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
		Creation.Set(pData);
		break;
	case 0x000A:
		LastMod.Set(pData);
		break;
	case 0x000B:
		LastAccess.Set(pData);
		break;
	case 0x000C:
		Expire.Set(pData);
		break;
	case 0x000D:
		BinaryDesc=(char*)pData;
		break;
	case 0x000E:
		if(FieldSize != 0)
		{
			///@TODO: im Destruktor löschen
			///@TODO: im Konstruktor auf Null
			pBinaryData = new UINT8[FieldSize];
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

bool PwDatabase::CloseDataBase(){
Groups.clear();
Entries.clear();
return true;
}

bool PwDatabase::SaveDataBase(QString filename){
CGroup SearchGroup;
UINT32 NumGroups,NumEntries,Signature1,Signature2,Flags,Version;
UINT8 TrafoRandomSeed[32];
UINT8 FinalRandomSeed[16];
UINT8 ContentsHash[32];
UINT8 EncryptionIV[16];

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

UINT16 FieldType;
UINT32 FieldSize;
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
		Groups[i].Creation.GetPackedTime((unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0004; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Groups[i].LastMod.GetPackedTime((unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0005; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Groups[i].LastAccess.GetPackedTime((unsigned char*)buffer+pos);pos += 5;

		FieldType = 0x0006; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Groups[i].Expire.GetPackedTime((unsigned char*)buffer+pos);pos += 5;

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
		Entries[i].Creation.GetPackedTime((unsigned char*)buffer+pos); pos+=5;


		FieldType = 0x000A; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Entries[i].LastMod.GetPackedTime((unsigned char*)buffer+pos); pos+=5;

		FieldType = 0x000B; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Entries[i].LastAccess.GetPackedTime((unsigned char*)buffer+pos); pos+=5;

		FieldType = 0x000C; FieldSize = 5;
		memcpy(buffer+pos, &FieldType, 2); pos += 2;
		memcpy(buffer+pos, &FieldSize, 4); pos += 4;
		Entries[i].Expire.GetPackedTime((unsigned char*)buffer+pos); pos+=5;

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
UINT8 FinalKey[32];
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
EncryptedPartSize = (unsigned long)aes.padEncrypt((UINT8*)buffer+DB_HEADER_SIZE,
						  pos - DB_HEADER_SIZE,
						  (UINT8*)buffer+DB_HEADER_SIZE);
}else if(CryptoAlgorithmus == ALGO_TWOFISH){
CTwofish twofish;
if(twofish.init(FinalKey, 32, EncryptionIV) == false){
//TODO:ERR_MSG
delete [] buffer;
return false;}
EncryptedPartSize = (unsigned long)twofish.padEncrypt((UINT8*)buffer+DB_HEADER_SIZE,
						      pos - DB_HEADER_SIZE,
						      (UINT8*)buffer+DB_HEADER_SIZE);
}

if((EncryptedPartSize > 2147483446) || (EncryptedPartSize == 0)){
//TODO:ERR_MSG
delete [] buffer;
return false;
}

if(file.open(IO_ReadWrite | IO_Truncate)==false){
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

bool PwDatabase::NewDataBase(){
filename="";
SearchGroupID=-1;
CryptoAlgorithmus=ALGO_AES;
KeyEncRounds=6000;

CGroup g;
g.ID=1;
g.Name=QObject::trUtf8("Standardgruppe");
Groups.push_back(g);
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

bool PwDatabase::isGroupIdInUse(UINT32 id){
for(int i=0;i<Groups.size();i++)
 if(Groups[i].ID==id)return true;

return false;
}

bool PwDatabase::isEntrySidInUse(UINT32 sid){
for(int i=0;i<Entries.size();i++)
 if(Entries[i].sID==sid)return true;

return false;
}


UINT32 PwDatabase::getNewGroupId(){
UINT32 id;
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

UINT32 PwDatabase::getNewEntrySid(){
UINT32 sid;
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
	UINT8 bin;
	bin=hex.mid(i,2).toUInt(&err,16);
	if(!err){
		qWarning("Invalid Hex Key\n");
		return false;}
	memcpy(dst+(i/2),&bin,1);
}
}
