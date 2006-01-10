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

#include <qobject.h>
#include <qfile.h>
#include <iostream>
#include <qdom.h>
#include "crypto/blowfish.h"
#include "crypto/sha1.h"
#include "Import_PwManager.h"
using namespace std;

bool Import_PwManager::importFile(QString filename, QString password, PwDatabase* db, QString& err){
database=db;
QFile file(filename);
char* buffer=NULL;
int offset=0;
int len=0;
if(!file.exists()){err+=QObject::trUtf8("Die angegebene Datei existiert nicht."); return false;}
if(!file.open(QIODevice::ReadOnly)){err+=QObject::trUtf8("Datei konnte nicht geöffnet werden."); return false;}
if(len=file.size()) buffer=new char[len];
else {err+=QObject::trUtf8("Datei ist leer"); return false;}
file.readBlock(buffer,len);
file.close();
if(QString::fromAscii(buffer,17)!="PWM_PASSWORD_FILE")
  {err+=QObject::trUtf8("Keine gültige PwManager-Datei"); return false;}
offset+=17;
if(buffer[offset]!=0x05)
 {err+=QObject::trUtf8("Nicht unterstützte Version"); return false;}
offset++;
if(buffer[offset]!=0x01)
 {err+=QObject::trUtf8("Nicht unterstützter Hash-Algorithmus"); return false;}
offset++;
if(buffer[offset]!=0x01)
 {err+=QObject::trUtf8("Nicht unterstützter Hash-Algorithmus"); return false;}
offset++;
if(buffer[offset]!=0x01)
 {err+=QObject::trUtf8("Nicht unterstützter Verschlüsselungs-Algorithmus"); return false;}
offset++;
if(buffer[offset]==0x00)Compression=0;
if(buffer[offset]==0x01)Compression=1;
if(buffer[offset]==0x02)Compression=2;
  ///@TODO Compression
  if(buffer[offset])
  {err+=QObject::trUtf8("Komprimierte PwManager-Dateien werden nicht unterstützt"); return false;}
offset++;
if(buffer[offset]==0x00)KeyFlag=true;
else KeyFlag=false;
offset++;
//Reserved Bytes (64)
offset+=64;
memcpy(KeyHash,buffer+offset,20);
offset+=20;
memcpy(DataHash,buffer+offset,20);
offset+=20;

Blowfish blowfish;
int pwlen=password.length();
byte* Key=new byte[pwlen];
byte* xml=new byte[len-offset+1];
xml[len-offset]=0;
memcpy(Key,password.ascii(),pwlen);
  char* key_hash=new char[20];
  CSHA1 sha;
  sha.Update(Key,pwlen);
  sha.Final();
  sha.GetHash((unsigned char*)key_hash);
  if(memcmp(key_hash,KeyHash,20)){
	delete[] Key; delete [] key_hash; delete [] buffer;
	err+=QObject::trUtf8("Falsches Passwort");
	return false;
}
delete [] key_hash;
blowfish.bf_setkey(Key,password.length());
blowfish.bf_decrypt(xml,(byte*)buffer+offset,len-offset);
delete [] Key;
delete [] buffer;
  char* content_hash=new char[20];
  sha.Reset();
  sha.Update(xml,strlen((char*)xml)-1);
  sha.Final();
  sha.GetHash((unsigned char*)content_hash);
  if(memcmp(content_hash,DataHash,20)){
	delete [] content_hash; delete [] xml;
	err+=QObject::trUtf8("Dateiinhalt ungültig (Hash-Test fehlgeschlagen)");
	return false;
}
delete[] content_hash;

if(!parseXmlContent((char*)xml)){
	delete [] xml;
	err+=QObject::trUtf8("Ungültiger XML-Inhalt"); return false;}
return true;
}

bool Import_PwManager::parseXmlContent(char* content){
QDomDocument db;
QString err;
int col,line;
if(!db.setContent(QString::fromUtf8(content,strlen(content)-1),false,&err,&line,&col)){
	cout << "Import_PwManager::parseXmlContent():" << endl;
	cout << (err+" (Line:%1 Column:%2)").arg(line).arg(col).ascii() << endl;
	return false;}
QDomElement root=db.documentElement();
if(root.tagName()!="P")return false;
//Kommentar und Kategorie haben das selbe Tag "c"
if(!root.elementsByTagName("c").item(0).isElement())return false;
QDomElement groups=root.elementsByTagName("c").item(0).toElement();

int i=0;
while(1){
 QDomElement CurrGroup;
 if(!groups.elementsByTagName("c"+QString::number(i)).length())break;
 if(groups.elementsByTagName("c"+QString::number(i)).length()>1)return false;
 if(!groups.elementsByTagName("c"+QString::number(i)).item(0).isElement())return false;
 CurrGroup=groups.elementsByTagName("c"+QString::number(i)).item(0).toElement();
 if(!CurrGroup.hasAttribute("n"))return false;
 CGroup* NewGroup=database->addGroup(NULL);
 NewGroup->Name=CurrGroup.attribute("n");
 int j=0;
	while(1){
	  QDomElement CurrEntry;
	  if(!CurrGroup.elementsByTagName("e"+QString::number(j)).length())break;
 	  if(CurrGroup.elementsByTagName("e"+QString::number(j)).length()>1)return false;
 	  if(!CurrGroup.elementsByTagName("e"+QString::number(j)).item(0).isElement())return false;
	  CurrEntry=CurrGroup.elementsByTagName("e"+QString::number(j)).item(0).toElement();
	  if(!xml_parseEntryAttributes(&CurrEntry,NewGroup))return false;
	  j++;
	}
 i++;
}

return true;
}

bool Import_PwManager::xml_parseEntryAttributes(QDomElement* EntryElement,CGroup* NewGroup){
CEntry* e=database->addEntry();
e->Title=EntryElement->elementsByTagName("d").item(0).toElement().text();
e->UserName=EntryElement->elementsByTagName("n").item(0).toElement().text();
QString pw=EntryElement->elementsByTagName("p").item(0).toElement().text();
e->Password.setString(pw,true);
e->Additional=EntryElement->elementsByTagName("c").item(0).toElement().text();
e->Additional=e->Additional.replace("$>--endl--<$","\n");
e->URL=EntryElement->elementsByTagName("u").item(0).toElement().text();
e->GroupID=NewGroup->ID;
return true;
}

