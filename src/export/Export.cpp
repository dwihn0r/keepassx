/***************************************************************************
 *   Copyright (C) 2007 by Tarek Saidi                                     *
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

#include <QMessageBox>
#include "main.h"
#include "Export.h"
#include "lib/FileDialogs.h"
#include "dialogs/SimplePasswordDlg.h"

QFile* ExporterBase::openFile(QWidget* parent, QString id, QStringList Filters){
	QString filename=KpxFileDialogs::saveFile(parent,id,tr("Import File..."),Filters);
	if(filename==QString())return NULL;
	QFile* file=new QFile(filename);
	if(!file->open(QIODevice::ReadWrite|QIODevice::Truncate)){
		QMessageBox::critical(parent,tr("Export Failed"),decodeFileError(file->error()));
		delete file;
		return NULL;
	}
	return file;	
} 
