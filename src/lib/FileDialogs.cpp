/***************************************************************************
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
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

#include <QDir>
#include <QSettings>
#include "main.h"
#include "FileDialogs.h"


IFileDialog* KpxFileDialogs::iFileDialog=NULL;
QtStandardFileDialogs DefaultQtDlgs;
FileDlgHistory fileDlgHistory;

void KpxFileDialogs::setPlugin(IFileDialog* plugin){
	iFileDialog=plugin;	
}

QString KpxFileDialogs::openExistingFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,const QString& Dir)
{
	QString dir;
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	if(Dir==QString()) dir=fileDlgHistory.getDir(Name);	
	else dir=Dir;
	QString result = iFileDialog->openExistingFileDialog(Parent,Title,dir,Filters);
	if(result!=QString()){
		fileDlgHistory.set(Name,result.left(result.lastIndexOf("/")+1),iFileDialog->getLastFilter());							  
	}
	return result;											  
}

QStringList KpxFileDialogs::openExistingFiles(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,const QString& Dir)
{
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	//Load History here!
	QStringList results=iFileDialog->openExistingFilesDialog(Parent,Title,QString(),Filters);
	if(results.size()){
		fileDlgHistory.set(Name,results[0].left(results[0].lastIndexOf("/")+1),iFileDialog->getLastFilter());								  
	}
	return results;
}

QString KpxFileDialogs::saveFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,bool OverWriteWarn, const QString& Dir)
{
	if(iFileDialog==NULL)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	//Load History here!
	QString result = iFileDialog->saveFileDialog(Parent,Title,QString(),Filters,OverWriteWarn);
	if(result!=QString()){
		fileDlgHistory.set(Name,result.left(result.lastIndexOf("/")+1),iFileDialog->getLastFilter());							  
	}
	return result;											  
}

QString QtStandardFileDialogs::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFile);
	if(!FileDlg.exec())return QString();
	if(!FileDlg.selectedFiles().size())return QString();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return 	FileDlg.selectedFiles()[0];	
}

QStringList QtStandardFileDialogs::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFiles);
	if(!FileDlg.exec())return QStringList();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return FileDlg.selectedFiles();
}

QString QtStandardFileDialogs::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,bool ShowOverwriteWarning){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::AnyFile);
	FileDlg.setAcceptMode(QFileDialog::AcceptSave);
	FileDlg.setConfirmOverwrite(ShowOverwriteWarning);
	if(!FileDlg.exec())return QString();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return FileDlg.selectedFiles()[0];	
}

int QtStandardFileDialogs::getLastFilter(){
	return LastFilter;	
}


QString FileDlgHistory::getDir(const QString& name){
	Entry e=History.value(name);
	if(e.isNull())
		return QDir::homePath();
	else
		return e.Dir;
}

int FileDlgHistory::getFilter(const QString& name){
	Entry e=History.value(name);
	if(e.isNull())
		return 0;
	else
		return e.Filter;
}

void FileDlgHistory::set(const QString& name,const QString& dir, int filter){
	History[name]=Entry();
	History[name].Dir=dir;
	History[name].Filter=filter;		
}
void FileDlgHistory::save(){
	if(settings->value("General/SaveFileDlgHistory",QVariant(true)).toBool()){
		settings->beginGroup("FileDlgHistory");
		for(int i=0;i<History.size();i++){
			QStringList entry;
			entry << History.keys()[i]
				  << History.values()[i].Dir
				  << QString::number(History.values()[i].Filter);
			settings->setValue(QString("ENTRY%1").arg(i),QVariant(entry));		
		}
		settings->endGroup();
	}
}

void FileDlgHistory::load(){
	if(settings->value("General/SaveFileDlgHistory",QVariant(true)).toBool()){
		settings->beginGroup("FileDlgHistory");
		QStringList keys=settings->childKeys();
		for(int i=0;i<keys.size();i++){
			Entry entry;
			QStringList value=settings->value(QString("ENTRY%1").arg(i)).toStringList();
			entry.Dir=value[1];
			entry.Filter=value[2].toInt();
			History[value[0]]=entry;		
		}
		settings->endGroup();
	}
	else{
		settings->remove("FileDlgHistory");		
	}
}

