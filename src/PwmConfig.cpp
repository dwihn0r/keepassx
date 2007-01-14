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

#define CSTR(x)((const char*)x.toUtf8())
#define QSTR(x)(QString::fromUtf8((x).c_str()))

bool CConfig::loadFromIni(QString filename){
	QString defaultSearchOptions = "001101111";
	QString defaultPwGenOptions = "1111100001";
	ini.SetPath((const char*)filename.toUtf8());
	ini.ReadFile();
	ClipboardTimeOut=ini.GetValueI("Options","ClipboardTimeOut",20);
	Toolbar=ini.GetValueB("UI","ShowToolbar",true);
	EntryDetails=ini.GetValueB("UI","ShowEntryDetails",true);
	OpenLast=ini.GetValueB("Options","RememberLastFile",true);
	LastFile=QSTR(ini.GetValue("Options","LastFile",""));
	ParseColumnString(QSTR(ini.GetValue("UI","Columns","1111100000")),Columns);
	BannerColor1=ParseColorString(QSTR(ini.GetValue("Options","BannerColor1","0,85,127")));
	BannerColor2=ParseColorString(QSTR(ini.GetValue("Options","BannerColor2","0,117,175")));
	BannerTextColor=ParseColorString(QSTR(ini.GetValue("Options","BannerTextColor","222,222,222")));
	ShowPasswords=ini.GetValueB("Options","ShowPasswords",false);
	ShowPasswordsPasswordDlg=ini.GetValueB("Options","ShowPasswordsPasswordDlg",false);
	OpenUrlCommand=QSTR(ini.GetValue("Options","UrlCmd","kfmclient openURL %1"));
	Language=QSTR(ini.GetValue("Options","LangFile",""));
	ParseBoolString(QSTR(ini.GetValue("Options","SearchOptions",(const char*)defaultSearchOptions.toUtf8())),defaultSearchOptions,SearchOptions,9);
	ListView_HidePasswords=ini.GetValueB("UI","HidePasswords",true);
	ListView_HideUsernames=ini.GetValueB("UI","HideUsernames",false);
	ParseBoolString(QSTR(ini.GetValue("Options","PwGenOptions",(const char*)defaultPwGenOptions.toUtf8())),defaultPwGenOptions,PwGenOptions,10);
	PwGenLength=ini.GetValueI("Options","PwGenLength",25);
	PwGenCharList=QSTR(ini.GetValue("Options","PwGenCharList",""));
	ExpandGroupTree=ini.GetValueB("Options","ExpandGroupTree",true);
	MainWinHeight=ini.GetValueI("UI","MainWinHeight",550);
	MainWinWidth=ini.GetValueI("UI","MainWinWidth",900);
	MainWinSplit1=ini.GetValueI("UI","MainWinSplit1",100);
	MainWinSplit2=ini.GetValueI("UI","MainWinSplit2",300);
	ParseIntString(QSTR(ini.GetValue("UI","ColumnSizes","15,10,10,10,10,10,10,10,10,10")),ColumnSizes,10);
	ShowStatusbar=ini.GetValueB("UI","ShowStatusbar",true);
	AlternatingRowColors=ini.GetValueB("Options","AlternatingRowColors",true);
	MountDir=QSTR(ini.GetValue("Options","MountDir",DEFAULT_MOUNT_DIR));
	RememberLastKey=ini.GetValueB("Options","RememberLastKey",true);
	LastKeyLocation=QSTR(ini.GetValue("Options","LastKeyLocation",""));
	LastKeyType=(tKeyType)ini.GetValueI("Options","LastKeyType",(int)PASSWORD);	
	if(!OpenLast)RememberLastKey=false;
	ToolbarIconSize=ini.GetValueI("UI","ToolbarIconSize",16);
	ShowSysTrayIcon=ini.GetValueB("Options","ShowSysTrayIcon",false);
	MinimizeToTray=ini.GetValueB("Options","MinimizeToTray",false);
	SaveFileDlgHistory=ini.GetValueB("Options","SaveFileDlgHistory",true);
	EnableBookmarkMenu=ini.GetValueB("Options","EnableBookmarkMenu",true);
	GroupTreeRestore=ini.GetValueI("Options","GroupTreeRestore",1);
	QString IntegrPluginVal=QSTR(ini.GetValue("Options","IntegrPlugin","None"));
	if(IntegrPluginVal=="None")	
		IntegrPlugin=NONE;
	else if(IntegrPluginVal=="Gnome")
		IntegrPlugin=GNOME;
	else if(IntegrPluginVal=="KDE")
		IntegrPlugin=KDE;
	else
		IntegrPlugin=NONE;
		
	return true;
}

bool CConfig::saveToIni(QString filename){
	ini.SetValueI("Options","ClipboardTimeOut",ClipboardTimeOut);
	ini.SetValueB("UI","ShowToolbar",Toolbar);
	ini.SetValueB("UI","ShowEntryDetails",EntryDetails);
	ini.SetValueB("Options","RememberLastFile",OpenLast);
	 if(OpenLast)ini.SetValue("Options","LastFile",(const char*)LastFile.toUtf8());
	 else	     ini.SetValue("Options","LastFile","");
	ini.SetValue("UI","Columns",(const char*)CreateColumnString().toUtf8(),true);
	ini.SetValue("Options","BannerColor1",(const char*)CreateColorString(BannerColor1).toUtf8(),true);
	ini.SetValue("Options","BannerColor2",(const char*)CreateColorString(BannerColor2).toUtf8(),true);
	ini.SetValue("Options","BannerTextColor",(const char*)CreateColorString(BannerTextColor).toUtf8(),true);
	ini.SetValueB("Options","ShowPasswords",ShowPasswords,true);
	ini.SetValueB("Options","ShowPasswordsPasswordDlg",ShowPasswordsPasswordDlg,true);
	ini.SetValue("Options","UrlCmd",(const char*)OpenUrlCommand.toUtf8(),true);
	ini.SetValue("Options","LangFile",(const char*)Language.toUtf8(),true);
	ini.SetValue("Options","SearchOptions",(const char*)CreateBoolString(SearchOptions,9).toUtf8(),true);
	ini.SetValueB("UI","HidePasswords",ListView_HidePasswords);
	ini.SetValueB("UI","HideUsernames",ListView_HideUsernames);
	ini.SetValue("Options","PwGenOptions",(const char*)CreateBoolString(PwGenOptions,10).toUtf8(),true);
	ini.SetValueI("Options","PwGenLength",PwGenLength,true);
	ini.SetValue("Options","PwGenCharList",(const char*)PwGenCharList.toUtf8(),true);
	ini.SetValueB("Options","ExpandGroupTree",ExpandGroupTree,true);
	ini.SetValueI("UI","MainWinHeight",MainWinHeight);
	ini.SetValueI("UI","MainWinWidth",MainWinWidth);
	ini.SetValueI("UI","MainWinSplit1",MainWinSplit1);
	ini.SetValueI("UI","MainWinSplit2",MainWinSplit2);
	ini.SetValue("UI","ColumnSizes",(const char*)CreateIntString(ColumnSizes,10).toUtf8(),true);
	ini.SetValueB("UI","ShowStatusbar",ShowStatusbar);
	ini.SetValueB("Options","AlternatingRowColors",AlternatingRowColors);
	ini.SetValue("Options","MountDir",(const char*)MountDir.toUtf8());
	ini.SetValueB("Options","RememberLastKey",RememberLastKey);
	ini.SetValueB("Options","ShowSysTrayIcon",ShowSysTrayIcon);
	ini.SetValueB("Options","MinimizeToTray",MinimizeToTray);
	ini.SetValueB("Options","SaveFileDlgHistory",SaveFileDlgHistory);
	ini.SetValueB("Options","EnableBookmarkMenu",EnableBookmarkMenu);
	ini.SetValueI("Options","GroupTreeRestore",GroupTreeRestore);	
	
	if(RememberLastKey){
		ini.SetValue("Options","LastKeyLocation",(const char*)LastKeyLocation.toUtf8());
		ini.SetValueI("Options","LastKeyType",LastKeyType);}
	else{
		ini.SetValue("Options","LastKeyLocation","");
		ini.SetValueI("Options","LastKeyType",0);}
	ini.SetValueI("UI","ToolbarIconSize",ToolbarIconSize,true);
	
	if(IntegrPlugin==NONE)
		ini.SetValue("Options","IntegrPlugin","None");
	if(IntegrPlugin==GNOME)
		ini.SetValue("Options","IntegrPlugin","Gnome");
	if(IntegrPlugin==KDE)
		ini.SetValue("Options","IntegrPlugin","KDE");
	
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
QStringList lst=str.split(',');
if(lst.size()!=3){
  qWarning((QObject::tr("Warning:")+" CConfig::ParseColorString(QString):"+QObject::tr("Invalid RGB color value.\n")).toUtf8());
  return QColor(0,0,0);}
bool err[3];
int r=lst[0].toUInt(err);
int g=lst[1].toUInt(err+1);
int b=lst[2].toUInt(err+2);
if(!err[0] || !err[1] || !err[2]){
  qWarning((QObject::tr("Warning:")+" CConfig::ParseColorString(QString):"+QObject::tr("Invalid RGB color value.\n")).toUtf8());
  return QColor(0,0,0);}
return QColor(r,g,b);
}

QString CConfig::CreateColorString(QColor c){
return (QString::number(c.red())+","+QString::number(c.green())+","+QString::number(c.blue()));
}

void CConfig::ParseBoolString(const QString &str,const QString &defaults,bool* dst, int count){
Q_ASSERT(defaults.count() == count);
for(int i=0; i<count; i++){
 QChar setting;
 if(i < str.count()) setting = str[i];
 else setting = defaults[i];

 if(setting==QChar('0'))
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
