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
#include "main.h"
#include "KpxConfig.h"
#include "FileDialogs.h"


IFileDialog* KpxFileDialogs::iFileDialog=NULL;
QtStandardFileDialogs DefaultQtDlgs;
FileDlgHistory fileDlgHistory;

void KpxFileDialogs::setPlugin(IFileDialog* plugin){
	iFileDialog=plugin;
}

QString KpxFileDialogs::openExistingFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,QString Dir,int SelectedFilter)
{
	if(!iFileDialog)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	if(Dir==QString())
		Dir=fileDlgHistory.getDir(Name);
	if(SelectedFilter==-1)
		SelectedFilter=fileDlgHistory.getFilter(Name);
	QString result = iFileDialog->openExistingFileDialog(Parent,Title,Dir,Filters,SelectedFilter);
	if(!result.isEmpty()){
		fileDlgHistory.set(Name,result.left(result.lastIndexOf("/")+1),iFileDialog->getLastFilter());
	}
	return result;
}

QStringList KpxFileDialogs::openExistingFiles(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,QString Dir,int SelectedFilter)
{
	if(!iFileDialog)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	if(Dir==QString())
		Dir=fileDlgHistory.getDir(Name);
	if(SelectedFilter==-1)
		SelectedFilter=fileDlgHistory.getFilter(Name);
	QStringList results=iFileDialog->openExistingFilesDialog(Parent,Title,QString(),Filters,SelectedFilter);
	if(!results.isEmpty()){
		fileDlgHistory.set(Name,results[0].left(results[0].lastIndexOf("/")+1),iFileDialog->getLastFilter());
	}
	return results;
}

QString KpxFileDialogs::saveFile(QWidget* Parent, const QString& Name, const QString& Title,const QStringList& Filters,bool OverWriteWarn,QString Dir,int SelectedFilter)
{
	if(!iFileDialog)iFileDialog=dynamic_cast<IFileDialog*>(&DefaultQtDlgs);
	if(Dir==QString())
		Dir=fileDlgHistory.getDir(Name);
	if(SelectedFilter==-1)
		SelectedFilter=fileDlgHistory.getFilter(Name);
	QString result = iFileDialog->saveFileDialog(Parent,Title,QString(),Filters,SelectedFilter,OverWriteWarn);
	if(!result.isEmpty()){
		fileDlgHistory.set(Name,result.left(result.lastIndexOf("/")+1),iFileDialog->getLastFilter());
	}
	return result;
}

QString QtStandardFileDialogs::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){
	if(SelectedFilter >= Filters.size())
		SelectedFilter=0;
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFile);
	FileDlg.selectFilter(Filters[SelectedFilter]);
	if(!FileDlg.exec())return QString();
	if(!FileDlg.selectedFiles().size())return QString();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return FileDlg.selectedFiles()[0];
}

QStringList QtStandardFileDialogs::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::ExistingFiles);
	if(!FileDlg.exec())return QStringList();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return FileDlg.selectedFiles();
}

QString QtStandardFileDialogs::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,int SelectedFilter, bool ShowOverwriteWarning){
	QFileDialog FileDlg(parent,title,dir);
	FileDlg.setFilters(Filters);
	FileDlg.setFileMode(QFileDialog::AnyFile);
	FileDlg.setAcceptMode(QFileDialog::AcceptSave);
	FileDlg.setConfirmOverwrite(ShowOverwriteWarning);
	if(!FileDlg.exec())return QString();
	LastFilter=FileDlg.filters().indexOf(FileDlg.selectedFilter());
	return FileDlg.selectedFiles().first();
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
	if(config->saveFileDlgHistory()){
		//settings->beginGroup("FileDlgHistory");
		for(unsigned i=0;i<static_cast<unsigned>(History.size());i++){
			QStringList entry;
			entry << History.keys().at(i)
				  << History.values().at(i).Dir
				  << QString::number(History.values().at(i).Filter);
			//settings->setValue(QString("ENTRY%1").arg(i),QVariant(entry));
			config->setFileDlgHistory(i,entry);
		}
		//settings->endGroup();
	}
}

void FileDlgHistory::load(){
	if(config->saveFileDlgHistory()){
		//settings->beginGroup("FileDlgHistory");
		//QStringList keys=settings->childKeys();
		unsigned count=config->fileDlgHistorySize();
		for(unsigned i=0;i</*keys.size()*/count;i++){
			Entry entry;
			QStringList value=config->fileDlgHistory(i);//settings->value(QString("ENTRY%1").arg(i)).toStringList();
			entry.Dir=value[1];
			entry.Filter=value[2].toInt();
			History[value[0]]=entry;
		}
		//settings->endGroup();
	}
	else{
		config->clearFileDlgHistory();
		//settings->remove("FileDlgHistory");
	}
}

