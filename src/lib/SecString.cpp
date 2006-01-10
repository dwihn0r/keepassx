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
#include <qmessagebox.h>
#include <iostream>
#include "random.h"
using namespace std;
Q_UINT8 SecString::Key[32]={0};

SecString::SecString(){
data=NULL;
len=0;
cryptlen=0;
}


SecString::~SecString(){
//if(data)delete [] data; ///@FIXME zerschießt den Stack, aber warum???
overwrite(plaintext);
}

void SecString::getString(QString & str){
if(data){
Rijndael aes;
int r=aes.init(Rijndael::CBC, Rijndael::Decrypt,Key,Rijndael::Key32Bytes);
if(r){ cout << "AES error, code " << r << endl;
       exit(-1);}
char* out=new char[len];
r=aes.padDecrypt((unsigned char*)data,cryptlen,(unsigned char*)out);
if(r!=len){ cout << "AES error in SecString::getString(), r!=length, r=" << r << endl;
       	       exit(-1);}
str=QString::fromUtf8(out,len);
overwrite(out,len);
delete [] out;
}
}

QString& SecString::getString(){
getString(plaintext);
return plaintext;
}

void SecString::delRef(){
overwrite(plaintext);
}

void SecString::setString(QString& str,bool DelSrc){
Rijndael aes;
int r=aes.init(Rijndael::CBC, Rijndael::Encrypt,Key,Rijndael::Key32Bytes);
if(r){ cout << "AES error, code " << r << endl;
       exit(-1);}
int il=str.length();
char* input=new char[il];
char* output=new char[il+16];
memcpy(input,str.utf8(),il);
r=aes.padEncrypt((unsigned char*)input,il,(unsigned char*)output);
if(r<0){ cout << "AES error, code " << r << endl;
         exit(-1);}
cryptlen=r;
len=il;
if(data)delete [] data;
data=output;
overwrite(input,il);
delete [] input;
if(DelSrc)overwrite(str);
}

void SecString::overwrite(char* str,int strlen){
if(strlen==0 || str==NULL)return;
getRandomBytes(str,strlen,1,false);
}

void SecString::overwrite(QString &str){
if(str.length()==0)return;
char* tmp=new char[str.length()];
getRandomBytes(tmp,str.length(),1,false);
str=tmp;
delete [] tmp;
}

int SecString::length(){
return len;
}

void SecString::generateSessionKey(){
getRandomBytes(Key,32,1,false);
}
