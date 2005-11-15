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
#include "PwmTime.h"


void CPwmTime::Set(unsigned char* pBytes){
Q_UINT32 dw1, dw2, dw3, dw4, dw5;
dw1 = (Q_UINT32)pBytes[0]; dw2 = (Q_UINT32)pBytes[1]; dw3 = (Q_UINT32)pBytes[2];
dw4 = (Q_UINT32)pBytes[3]; dw5 = (Q_UINT32)pBytes[4];
// Unpack 5 byte structure to date and time
///@FIXME nicht Endian-sicher
Year = (dw1 << 6) | (dw2 >> 2);
Month = ((dw2 & 0x00000003) << 2) | (dw3 >> 6);
Day = (dw3 >> 1) & 0x0000001F;
Hour = ((dw3 & 0x00000001) << 4) | (dw4 >> 4);
Minute = ((dw4 & 0x0000000F) << 2) | (dw5 >> 6);
Second = dw5 & 0x0000003F;
}

void CPwmTime::GetPackedTime(unsigned char* pBytes){
///@FIXME nicht Endian-sicher
pBytes[0] = (Q_UINT8)(((Q_UINT32)Year >> 6) & 0x0000003F);
pBytes[1] = (Q_UINT8)((((Q_UINT32)Year & 0x0000003F) << 2) | (((Q_UINT32)Month >> 2) & 0x00000003));
pBytes[2] = (Q_UINT8)((((Q_UINT32)Month & 0x00000003) << 6) | (((Q_UINT32)Day & 0x0000001F) << 1) | (((Q_UINT32)Hour >> 4) & 0x00000001));
pBytes[3] = (Q_UINT8)((((Q_UINT32)Hour & 0x0000000F) << 4) | (((Q_UINT32)Minute >> 2) & 0x0000000F));
pBytes[4] = (Q_UINT8)((((Q_UINT32)Minute & 0x00000003) << 6) | ((Q_UINT32)Second & 0x0000003F));
}

QString CPwmTime::GetString(Q_UINT16 format){
QString str;
switch(format){
	case 0:
	//DD.MM.YYYY HH:MM:SS
	str=(QString)"%1.%2.%3 %4:%5:%6";
	break;

	case 1:
	//MM/DD/YYYY HH:MM:SS
	str=(QString)"%2/%1/%3 %4:%5:%6";
	break;
};


if(Day<10){
str=str.arg(((QString)"0"+QString::number(Day)));
}else{
str=str.arg(Day);}

if(Month<10){
str=str.arg(((QString)"0"+QString::number(Month)));
}else{
str=str.arg(Month);}

str=str.arg(Year);

if(Hour<10){
str=str.arg(((QString)"0"+QString::number(Hour)));
}else{
str=str.arg(Hour);}

if(Minute<10){
str=str.arg(((QString)"0"+QString::number(Minute)));
}else{
str=str.arg(Minute);}

if(Second<10){
str=str.arg(((QString)"0"+QString::number(Second)));
}else{
str=str.arg(Second);}

return str;
}

void CPwmTime::Set(Q_UINT8 iDay,Q_UINT8 iMonth,Q_UINT16 iYear,Q_UINT8 iHour,Q_UINT8 iMinute,Q_UINT8 iSecond){
Day=iDay;
Month=iMonth;
Year=iYear;
Hour=iHour;
Minute=iMinute;
Second=iSecond;
}

bool CPwmTime::IsValidDate(QString& s){

if(s.length()>10)return false;

int count=0;
for(int i=0;i<s.length();i++){
if(s[i]=='.')count++;
}
if(count!=2)return false;
bool conv_error[3];
int day=s.section(".",0,0).toInt(&conv_error[0]);
int month=s.section(".",1,1).toInt(&conv_error[1]);
int year=s.section(".",2,2).toInt(&conv_error[2]);
if(conv_error[0]==false || conv_error[1]==false || conv_error[2]==false)return false;
if(day>31 || day<1 || month>12 || month<1 || year<0 || year>2999)return false;
return true;
}

bool CPwmTime::IsValidTime(QString& s){

if(s.length()>8)return false;

int count=0;
for(int i=0;i<s.length();i++){
if(s[i]==':')count++;
}
if(count!=2)return false;
bool conv_error[3];
int hour=s.section(":",0,0).toInt(&conv_error[0]);
int minute=s.section(":",1,1).toInt(&conv_error[1]);
int second=s.section(":",2,2).toInt(&conv_error[2]);
if(conv_error[0]==false || conv_error[1]==false || conv_error[2]==false)return false;
if(hour>23 || hour<0 || minute>59 || minute<0 || second>59 || second<0)return false;
return true;
}

bool CPwmTime::SetDate(QString s){
if(IsValidDate(s)==false)return false;
Day=s.section(".",0,0).toInt();
Month=s.section(".",1,1).toInt();
Year=s.section(".",2,2).toInt();
return true;
}

bool CPwmTime::SetTime(QString s){
if(IsValidTime(s)==false)return false;
Hour=s.section(":",0,0).toInt();
Minute=s.section(":",1,1).toInt();
Second=s.section(":",2,2).toInt();
return true;
}

void CPwmTime::SetToNow(){
time_t curTime=time(NULL);
tm* current=localtime(&curTime);
Year=current->tm_year+1900;
Month=current->tm_mon;
Day=current->tm_mday;
Hour=current->tm_hour;
Minute=current->tm_min;
Second=current->tm_sec;
}

bool CPwmTime::operator==(const CPwmTime& t){
if(   Year==t.Year
   && Month==t.Month
   && Day==t.Day
   && Hour==t.Hour
   && Minute==t.Minute
   && Second==t.Second) return true;
else return false;
}

bool CPwmTime::operator!=(const CPwmTime& t){
return !(*this==t);
}
