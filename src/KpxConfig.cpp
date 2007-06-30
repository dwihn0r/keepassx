/***************************************************************************
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   Copyright (C) 2007 by Constantin "Dinosaur" Makshin                   *
 *   dinosaur-rus@users.sourceforge.net                                    *
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

#include "KpxConfig.h"
#include <Qt>
#include <QDir>

KpxConfig::KpxConfig(const QString& filePath) : settings(filePath,QSettings::IniFormat){
	if (settings.contains("Options/GroupTreeRestore") && !settings.contains("Options/GroupTreeState")){
		switch (settings.value("Options/GroupTreeRestore",1).toInt()){
			case 0:
				setGroupTreeState(RestoreLast);
				break;

			case 2:
				setGroupTreeState(DoNothing);
				break;

			default:
				setGroupTreeState(ExpandAll);
		}
		settings.remove("Options/GroupTreeRestore");
	}
	if (urlCmd()=="<<default>>")
		setUrlCmd(QString());
}

unsigned KpxConfig::fileDlgHistorySize(){
	settings.beginGroup("FileDlgHistory");
	unsigned res=static_cast<unsigned>(settings.childKeys().size());
	settings.endGroup();
	return res;
}

QColor KpxConfig::stringToColor(const QString& str){
	QStringList ints=str.split(',');
	QColor res;
	if (ints.count()>0)
		res.setRed(ints.at(0).toInt());
	if (ints.count()>1)
		res.setGreen(ints.at(1).toInt());
	if (ints.count()>2)
		res.setBlue(ints.at(2).toInt());
	return res;
}

QBitArray KpxConfig::stringToBitArray(const QString& str, unsigned count){
	QBitArray res(static_cast<int>(count));
	if (static_cast<unsigned>(str.length())<count)
		count=static_cast<unsigned>(str.length());
	for (int i=0;i<static_cast<int>(count);i++){
		QChar c=str.at(i);
		if ((c=='1') || (c=='t') || (c=='y'))
			res.setBit(i);
	}
	return res;
}

KpxConfig::GrpTreeState KpxConfig::stringToGrpTreeState(const QString& str){
	GrpTreeState res=ExpandAll;
	if (!str.compare("Restore",Qt::CaseInsensitive))
		res=RestoreLast;
	else if (!str.compare("None",Qt::CaseInsensitive))
		res=DoNothing;
	return res;
}

QList<int> KpxConfig::stringToIntArray(const QString& str, unsigned count){
	QStringList ints=str.split(',');
	QList<int> res;
	unsigned i,
	         intsCount=qMin(static_cast<unsigned>(ints.count()),count);
	for (i=0;i<intsCount;i++)
		res.append(ints.at(i).toInt());
	for (;i<count;i++)
		res.append(0);
	return res;
}

KpxConfig::IntegrPluginType KpxConfig::stringToIntegrPluginType(const QString& str){
	IntegrPluginType res=NoIntegr;
	if (!str.compare("KDE",Qt::CaseInsensitive))
		res=KDE;
	else if (!str.compare("Gnome",Qt::CaseInsensitive))
		res=Gnome;
	return res;
}

tKeyType KpxConfig::stringToKeyType(const QString& str){
	tKeyType res=PASSWORD;
	if (!str.compare("KeyFile",Qt::CaseInsensitive))
		res=KEYFILE;
	else if (!str.compare("Composite",Qt::CaseInsensitive))
		res=BOTH;
	return res;
}

QString KpxConfig::bitArrayToString(const QBitArray& bitArray){
	QString res;
	for (int i=0;i<bitArray.count();i++)
		res.append(QString::number(bitArray.at(i)?1:0));
	return res;
}

QString KpxConfig::grpTreeStateToString(GrpTreeState grpTreeState){
	QString res;
	switch (grpTreeState){
		case RestoreLast:
			res="Restore";
			break;

		case ExpandAll:
			res="ExpandAll";
			break;

		case DoNothing:
			res="None";
	}
	return res;
}

QString KpxConfig::intArrayToString(const QList<int>& intArray){
	QString res;
	if (!intArray.isEmpty())
	{
		res.setNum(intArray.first());
		for (int i=1;i<intArray.count();i++)
			res.append(QString(",%1").arg(intArray.at(i)));
	}
	return res;
}

QString KpxConfig::integrPluginTypeToString(IntegrPluginType integrPluginType){
	QString res;
	switch (integrPluginType){
		case NoIntegr:
			res="None";
			break;

		case KDE:
			res="KDE";
			break;

		case Gnome:
			res="Gnome";
	}
	return res;
}

QString KpxConfig::keyTypeToString(tKeyType keyType){
	QString res;
	switch (keyType){
		case PASSWORD:
			res="Password";
			break;

		case KEYFILE:
			res="KeyFile";
			break;

		case BOTH:
			res="Composite";
	}
	return res;
}
