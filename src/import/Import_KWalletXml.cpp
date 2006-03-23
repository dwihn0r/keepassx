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

#include "Import_KWalletXml.h"
#include <iostream>
#include <qobject.h>
#include <qfile.h>
#include <qdom.h>
using namespace std;

bool Import_KWalletXml::importFile(QString FileName,Database* pwm,QString& err){
QFile file(FileName);
if(!file.exists()){
	err+=QObject::tr("File not found.");
	return false;}
if(!file.open(QIODevice::ReadOnly)){
	err+=QObject::tr("Could not open file.");
	return false;}
int len=file.size();
if(len==0){
	err+=QObject::tr("File is empty.");
	return false;}
Q_UINT8* buffer=new Q_UINT8[len];
file.readBlock((char*)buffer,len);
file.close();
QDomDocument doc;
QString xmlerr;
int col,line;
if(!doc.setContent(QString::fromUtf8((char*)buffer,len),false,&xmlerr,&line,&col)){
	cout << "Import_PwManager::parseXmlContent():" << endl;
	cout << (xmlerr+" (Line:%1 Column:%2)").arg(line).arg(col).ascii() << endl;
	err+=QObject::tr("Invalid XML file (see stdout for details).");
	delete [] buffer;
	return false;}
delete [] buffer;
QDomElement root=doc.documentElement();
if(root.tagName()!="wallet"){err+=QObject::tr("Invalid XML file."); return false;}
QDomNodeList groups=root.elementsByTagName("folder");
if(!groups.length()){err+=QObject::tr("Document does not contain data."); return false;}
for(int i=0;i<groups.length();i++){
	if(!groups.item(i).isElement()){err+=QObject::tr("Invalid XML file."); return false;}
	QDomElement CurrGroup=groups.item(i).toElement();
	if(!CurrGroup.hasAttribute("name")){err+=QObject::tr("Invalid XML file."); return false;}
 	CGroup* NewGroup=pwm->addGroup(NULL);
	NewGroup->Name=CurrGroup.attribute("name");
	QDomNodeList entries=CurrGroup.elementsByTagName("password");
	for(int j=0;j<entries.length();j++){
		if(!entries.item(j).isElement()){err+=QObject::tr("Invalid XML file."); return false;}
		QDomElement CurrEntry=entries.item(j).toElement();
		if(!CurrEntry.hasAttribute("name")){err+=QObject::tr("Invalid XML file."); return false;}
		CEntry* NewEntry=pwm->addEntry();
		NewEntry->Title=CurrEntry.attribute("name");
		NewEntry->GroupID=NewGroup->ID;
		QString pw=CurrEntry.text();
		NewEntry->Password.setString(pw,true);
		}
}
pwm->file=NULL;
pwm->SearchGroupID=-1;
pwm->CryptoAlgorithmus=ALGO_AES;
pwm->KeyEncRounds=6000;
return true;
}
