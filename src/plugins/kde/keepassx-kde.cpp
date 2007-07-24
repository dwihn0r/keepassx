/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
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

#include <kapplication.h>
#include <kfiledialog.h>
#include <kcmdlineargs.h>
#include <kiconloader.h>
#include <QPixmap>
#include <QHash>
#include "keepassx-kde.h"

QHash<QString,QString>IconMap;

Q_EXPORT_PLUGIN2(keepassx_kde, KdePlugin)


void createIconMap(){
	IconMap["alarmclock"]="alarmclock";
	IconMap["appsettings"]="configure";
	IconMap["autotype"]="input-keyboard";
	IconMap["clock"]="chronometer";
	IconMap["clonenetry"]="edit-copy";
	IconMap["copypwd"]="kgpg-export-kgpg";
	IconMap["copyusername"]="user";
	IconMap["dbsearch"]="edit-find";
	IconMap["dbsettings"]="configure";
	IconMap["delete"]="edit-delete";
	IconMap["delete-entry"]="edit-delete";
	IconMap["delete-group"]="edit-delete";
	IconMap["editentry"]="edit";
	IconMap["editgroup"]="edit";
	IconMap["exit"]="application-exit";
	IconMap["expired"]="flag-red";
	IconMap["fileclose"]="dialog-close";
	IconMap["filedelete"]="edit-delete";
	IconMap["filenew"]="document-new";
	IconMap["fileopen"]="document-open";
	IconMap["filesave"]="document-save";
	IconMap["filesaveas"]="document-save-as";
	IconMap["filesaveasdisabled"]="document-save-as"; ///FIXME
	IconMap["generator"]="roll";
	IconMap["groupsearch"]="file-find";
	IconMap["help"]="help-contents";
	IconMap["key"]="password";
	IconMap["manual"]="help-contents";
}

QString KdePlugin::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){	
	return KFileDialog::getOpenFileName();

}

QStringList KdePlugin::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){
	return QStringList();
}

QString KdePlugin::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter,bool OverWriteWarn){return QString();}

QApplication* KdePlugin::getMainAppObject(int argc, char** argv){
	KCmdLineArgs::init(argc,argv,"keepassx","KeePassX","Cross Platform Password Manager","0.2.3");
	createIconMap();
	return dynamic_cast<QApplication*>( new KApplication() );
}


QIcon KdePlugin::getIcon(const QString& name){
	KIconLoader loader;
	QPixmap pxm=loader.loadIcon(IconMap.value(name),K3Icon::Desktop);
	QIcon icon(pxm);
	return icon;
}


