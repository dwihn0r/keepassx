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

#include "SecString.h"
#include <iostream>
#include "crypto/arcfour.h"
#include "crypto/yarrow.h"

using namespace std;
CArcFour SecString::RC4;

SecString::operator QString(){
return string();
}

SecString::SecString(){
locked=true;
}

int SecString::length(){
return crypt.size();
}

SecString::~SecString(){
lock();
}

void SecString::lock(){
locked=true;
overwrite(plain);
plain=QString();
}

void SecString::unlock(){
locked=false;
plain=QString();
if(!crypt.length()){return;}
const unsigned char* buffer=new unsigned char[crypt.length()];
SecString::RC4.decrypt((byte*)crypt.data(),(unsigned char*)buffer,crypt.length());
plain=QString::fromUtf8((const char*)buffer,crypt.size());
overwrite((unsigned char*)buffer,crypt.size());
delete [] buffer;
}


const QString& SecString::string(){
if(locked){
 printf("Error in function SecString::string(): string is locked\n");
 return QString(">SEC_STRING_ERROR<");
}
return plain;
}


void SecString::setString(QString& str,bool DeleteSource){
QByteArray StrData=str.toUtf8();
int len=StrData.size();
unsigned char* buffer=new unsigned char[len];
SecString::RC4.encrypt((const unsigned char*)StrData.data(),buffer,len);
crypt=QByteArray((const char*)buffer,len);
overwrite(buffer,len);
overwrite((unsigned char*)StrData.data(),len);
delete [] buffer;
if(DeleteSource){
  overwrite(str);
  str=QString();}
lock();
}

void SecString::overwrite(unsigned char* str,int strlen){
if(strlen==0 || str==NULL)return;
for(int i=0; i<strlen; i++){
	str[i]=0;
}
}

void SecString::overwrite(QString &str){
if(str.length()==0)return;
for(int i=0; i<str.length(); i++){
	((char*)str.data())[i]=0;
}
}


void SecString::generateSessionKey(){
CArcFour arc;
unsigned char* sessionkey=new unsigned char[32];
randomize(sessionkey,32);
RC4.setKey(sessionkey,32);
delete [] sessionkey;
}