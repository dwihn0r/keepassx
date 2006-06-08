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

#include "Database.h"
#include "lib/random.h"

KpxUuid::KpxUuid(){
	generate();
}


void KpxUuid::generate(){
	char uuid[16];
	getRandomBytes(uuid,16);
	quint32 Secs=QDateTime::currentDateTime().toTime_t();
	quint16 mSecs=QTime::currentTime().msec();
	mSecs=(mSecs & 0x3FF) | (*((quint16*)(uuid+4)) & 0xFC00); //msec has only 10 Bits, filling the rest with random data
	memcpy((void*)uuid,&Secs,4);
	memcpy((void*)(uuid+4),&mSecs,2);
	Data=QByteArray(uuid,16);
}

QString KpxUuid::toString()const{
	QString hex;
	Q_ASSERT(Data.length()==16);
	for(int i=0;i<16;i++){
		QString HexByte;
		HexByte.setNum((unsigned char)*(Data.data()+i),16);
		if(HexByte.length()<2)HexByte="0"+HexByte;
		hex+=HexByte;
	}
	return QString("{%1-%2-%3-%4-%5}")
			.arg(hex.mid(0,8))
			.arg(hex.mid(8,4))
			.arg(hex.mid(12,4))	
			.arg(hex.mid(16,4))											
			.arg(hex.mid(20,12));
}

void KpxUuid::toRaw(void* dst){
	memcpy(dst,Data.data(),16);
}

void KpxUuid::fromRaw(void* src){
	Data=QByteArray((char*)src,16);
}

bool KpxUuid::operator==(const KpxUuid& other)const{
	return other.Data==Data;
}

bool KpxUuid::operator!=(const KpxUuid& other)const{
	return other.Data!=Data;
}



QString KpxDateTime::toString(Qt::DateFormat format) const{
if(*this==Date_Never)return QObject::tr("Never");
else return QDateTime::toString(format);
}

QString KpxDateTime::dateToString(Qt::DateFormat format) const{
if(*this==Date_Never)return QObject::tr("Never");
else return date().toString(format);
}


CEntry::CEntry(){
ImageID=0;
OldImgID=0;
GroupID=0;
Creation=QDateTime::currentDateTime();
LastMod=QDateTime::currentDateTime();
LastAccess=QDateTime::currentDateTime();
Expire=QDateTime(QDate(2999,12,28),QTime(23,59,59)); //Never
BinaryData=QByteArray();
}

bool CGroup::UI_ExpandByDefault=true;

bool CEntry::operator==(const CEntry& e)const{
if(sID==e.sID)return true;
else	      return false;
}

bool CGroup::operator==(const CGroup& g)const{
if(ID==g.ID)return true;
else	      return false;
}

CGroup::CGroup(){
Creation=QDateTime::currentDateTime();
LastAccess=QDateTime::currentDateTime();
LastMod=QDateTime::currentDateTime();
Expire=QDateTime(QDate(2999,12,28),QTime(23,59,59));
Level=0;
ImageID=0;
OldImgID=0;
Name="<Group>";
UI_ItemIsExpanded=UI_ExpandByDefault;
}

CGroup::~CGroup(){
}

CEntry::~CEntry(){

}

Database::Database(){
file=NULL;
KeyEncRounds=6000;
CryptoAlgorithmus=ALGO_AES;
}
