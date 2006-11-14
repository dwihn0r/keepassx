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
#include "keepassx-kde.h"


Q_EXPORT_PLUGIN2(keepassx_kde, KdePlugin)

QString KdePlugin::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters){	
	return KFileDialog::getOpenFileName();

}


QStringList KdePlugin::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	return QStringList();
}

QString KdePlugin::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,bool OverWriteWarn){return QString();}

QApplication* KdePlugin::getMainAppObject(int argc, char** argv){
	KCmdLineArgs::init(argc,argv,"keepassx","KeePassX","Cross Platform Password Manager","0.2.3");
	return dynamic_cast<QApplication*>( new KApplication() );
}
