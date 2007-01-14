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

#include "FileDialogs.h"
#include <QDir>

IFileDialog* KpxFileDialogs::iFileDialog=NULL;
QtStandardFileDialogs DefaultQtDlgs;

void KpxFileDialogs::setPlugin(IFileDialog* plugin){
	iFileDialog=plugin;	
}

QString KpxFileDialogs::openExistingFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,const QString& Dir)
{
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	//Load History here!
	return iFileDialog->openExistingFileDialog(Parent,Title,QDir::homePath(),Filters);													  
												  
}

QStringList KpxFileDialogs::openExistingFiles(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,const QString& Dir)
{
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	//Load History here!
	return iFileDialog->openExistingFilesDialog(Parent,Title,QString(),Filters);											  
}

QString KpxFileDialogs::saveFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,bool OverWriteWarn, const QString& Dir)
{
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	//Load History here!
	return iFileDialog->saveFileDialog(Parent,Title,QString(),Filters,OverWriteWarn);

												  
}

QString QtStandardFileDialogs::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFile);
	if(!FileDlg.exec())return QString();
	if(!FileDlg.selectedFiles().size())return QString();
	return 	FileDlg.selectedFiles()[0];	
}

QStringList QtStandardFileDialogs::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFiles);
	if(!FileDlg.exec())return QStringList();
	return FileDlg.selectedFiles();
}

QString QtStandardFileDialogs::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,bool ShowOverwriteWarning){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::AnyFile);
	FileDlg.setAcceptMode(QFileDialog::AcceptSave);
	FileDlg.setConfirmOverwrite(ShowOverwriteWarning);
	if(!FileDlg.exec())return QString();
	return FileDlg.selectedFiles()[0];	
}


