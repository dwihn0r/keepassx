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

#include "PwmConfig.h"
#include <qobject.h>
#include <qdir.h>
#include <iostream>
using namespace std;

#ifdef Q_WS_MAC
	#define DEFAULT_MOUNT_DIR "/Volumes/"
#endif
#ifdef Q_WS_X11
	#define DEFAULT_MOUNT_DIR "/media/"
#endif
#ifdef Q_WS_WIN
	#define DEFAULT_MOUNT_DIR "/"
#endif


bool CConfig::loadFromIni(QString filename){
ini.SetPath((const char*)filename);
ini.ReadFile();
ClipboardTimeOut=ini.GetValueI("Options","ClipboardTimeOut",20);
Toolbar=ini.GetValueB("UI","ShowToolbar",true);
EntryDetails=ini.GetValueB("UI","ShowEntryDetails",true);
OpenLast=ini.GetValueB("Options","RememberLastFile",true);
LastFile=ini.GetValue("Options","LastFile","").c_str();
ParseColumnString(ini.GetValue("UI","Columns","1111100000").c_str(),Columns);
BannerColor1=ParseColorString(ini.GetValue("Options","BannerColor1","0,104,176").c_str());
BannerColor2=ParseColorString(ini.GetValue("Options","BannerColor2","213,239,255").c_str());
BannerTextColor=ParseColorString(ini.GetValue("Options","BannerTextColor","4,0,80").c_str());
ShowPasswords=ini.GetValueB("Options","ShowPasswords",false);
OpenUrlCommand=ini.GetValue("Options","UrlCmd","kfmclient openURL %1").c_str();
Language=ini.GetValue("Options","LangFile","").c_str();
ParseBoolString(ini.GetValue("Options","SearchOptions","001101111").c_str(),SearchOptions,9);
ListView_HidePasswords=ini.GetValueB("UI","HidePasswords",true);
ListView_HideUsernames=ini.GetValueB("UI","HideUsernames",false);
ParseBoolString(ini.GetValue("Options","PwGenOptions","1111100001").c_str(),PwGenOptions,10);
PwGenLength=ini.GetValueI("Options","PwGenLength",25);
PwGenCharList=ini.GetValue("Options","PwGenCharList","").c_str();
ExpandGroupTree=ini.GetValueB("Options","ExpandGroupTree",true);
EnableKdePlugin=ini.GetValueB("KDE Plugin","Enabled",false);
MainWinHeight=ini.GetValueI("UI","MainWinHeight",550);
MainWinWidth=ini.GetValueI("UI","MainWinWidth",900);
MainWinSplit1=ini.GetValueI("UI","MainWinSplit1",100);
MainWinSplit2=ini.GetValueI("UI","MainWinSplit2",300);
ParseIntString(ini.GetValue("UI","ColumnSizes","15,10,10,10,10,10,10,10,10,10").c_str(),ColumnSizes,10);
ShowStatusbar=ini.GetValueB("UI","ShowStatusbar",true);
AlternatingRowColors=ini.GetValueB("Options","AlternatingRowColors",true);
MountDir=ini.GetValue("Options","MountDir",DEFAULT_MOUNT_DIR).c_str();
RememberLastKey=ini.GetValueB("Options","RememberLastKey",true);
LastKeyLocation=ini.GetValue("Options","LastKeyLocation","").c_str();
LastKeyType=(tKeyType)ini.GetValueI("Options","LastKeyType",(int)PASSWORD);
if(!OpenLast)RememberLastKey=false;
return true;
}

bool CConfig::saveToIni(QString filename){
ini.SetValueI("Options","ClipboardTimeOut",ClipboardTimeOut);
ini.SetValueB("UI","ShowToolbar",Toolbar);
ini.SetValueB("UI","ShowEntryDetails",EntryDetails);
ini.SetValueB("Options","RememberLastFile",OpenLast);
 if(OpenLast)ini.SetValue("Options","LastFile",(const char*)LastFile);
 else	     ini.SetValue("Options","LastFile","");
ini.SetValue("UI","Columns",(const char*)CreateColumnString(),true);
ini.SetValue("Options","BannerColor1",(const char*)CreateColorString(BannerColor1),true);
ini.SetValue("Options","BannerColor2",(const char*)CreateColorString(BannerColor2),true);
ini.SetValue("Options","BannerTextColor",(const char*)CreateColorString(BannerTextColor),true);
ini.SetValueB("Options","ShowPasswords",ShowPasswords,true);
ini.SetValue("Options","UrlCmd",(const char*)OpenUrlCommand,true);
ini.SetValue("Options","LangFile",(const char*)Language,true);
ini.SetValue("Options","SearchOptions",(const char*)CreateBoolString(SearchOptions,9),true);
ini.SetValueB("UI","HidePasswords",ListView_HidePasswords);
ini.SetValueB("UI","HideUsernames",ListView_HideUsernames);
ini.SetValue("Options","PwGenOptions",(const char*)CreateBoolString(PwGenOptions,10),true);
ini.SetValueI("Options","PwGenLength",PwGenLength,true);
ini.SetValue("Options","PwGenCharList",(const char*)PwGenCharList,true);
ini.SetValueB("Options","ExpandGroupTree",ExpandGroupTree,true);
ini.SetValueB("KDE Plugin","Enabled",EnableKdePlugin,true);
ini.SetValueI("UI","MainWinHeight",MainWinHeight);
ini.SetValueI("UI","MainWinWidth",MainWinWidth);
ini.SetValueI("UI","MainWinSplit1",MainWinSplit1);
ini.SetValueI("UI","MainWinSplit2",MainWinSplit2);
ini.SetValue("UI","ColumnSizes",(const char*)CreateIntString(ColumnSizes,10),true);
ini.SetValueB("UI","ShowStatusbar",ShowStatusbar);
ini.SetValueB("Options","AlternatingRowColors",AlternatingRowColors);
ini.SetValue("Options","MountDir",(const char*)MountDir);
ini.SetValueB("Options","RememberLastKey",RememberLastKey);
if(RememberLastKey){
	ini.SetValue("Options","LastKeyLocation",(const char*)LastKeyLocation);
	ini.SetValueI("Options","LastKeyType",LastKeyType);}
else{
	ini.SetValue("Options","LastKeyLocation","");
	ini.SetValueI("Options","LastKeyType",0);}
if(!ini.WriteFile())return false;
else return true;
}

void CConfig::ParseColumnString(QString str, bool* dst){
if(str.length()<10){
bool _default[]={true,true,true,true,true,false,false,false,false,false};
memcpy((char*)_default,(char*)dst,10*sizeof(bool));}
for(int i=0; i<10; i++){
if(str[i]==QChar('0'))
 *(dst+i)=false;
else
 *(dst+i)=true;

}

}

QString CConfig::CreateColumnString(){
QString str="";
for(int i=0;i<10;i++){
if(Columns[i])str+="1";
else	      str+="0";
}
return str;
}

QColor CConfig::ParseColorString(QString str){
QStringList lst=QStringList::split(',',str);
if(lst.size()!=3){
  qWarning(QObject::tr("Warning:")+" CConfig::ParseColorString(QString):"+QObject::tr("Invalid RGB color value.\n"));
  return QColor(0,0,0);}
bool err[3];
int r=lst[0].toUInt(err);
int g=lst[1].toUInt(err+1);
int b=lst[2].toUInt(err+2);
if(!err[0] || !err[1] || !err[2]){
  qWarning(QObject::tr("Warning:")+" CConfig::ParseColorString(QString):"+QObject::tr("Invalid RGB color value.\n"));
  return QColor(0,0,0);}
return QColor(r,g,b);
}

QString CConfig::CreateColorString(QColor c){
return (QString::number(c.red())+","+QString::number(c.green())+","+QString::number(c.blue()));
}

void CConfig::ParseBoolString(const QString &str,bool* dst, int count){
for(int i=0; i<count; i++){
if(str[i]==QChar('0'))
 *(dst+i)=false;
else
 *(dst+i)=true;
}
}

QString CConfig::CreateBoolString(bool* src, int count){
QString str="";
for(int i=0;i<count;i++){
if(src[i])str+="1";
else	  str+="0";
}
return str;
}

#define DEFAULT_INT_VAL 20

void CConfig::ParseIntString(const QString &str,int* dst, int count){
QStringList lst=str.split(",");
if(lst.size()!=count)
	qWarning("Warnig: CConfig::ParseIntString(): unexpected item count.\n");
int* values=new int[count];
bool err;
for(int i=0;i<lst.size() && i<count;i++){
	values[i]=lst[i].toUInt(&err);
	if(!err){
		qWarning("Warnig: CConfig::ParseIntString(): invalid int value.\n");
		values[i]=DEFAULT_INT_VAL;}
}
if(count > lst.size()){
	for(int i=lst.size(); i<count; i++)
		values[i]=DEFAULT_INT_VAL;}

memcpy(dst,values,count*sizeof(int));
delete [] values;
}

QString CConfig::CreateIntString(int* src, int count){
QString str;
for(int i=0;i<count;i++){
	str+=QString::number(src[i]);
	if(i<(count-1))str+=",";
}
return str;
}
