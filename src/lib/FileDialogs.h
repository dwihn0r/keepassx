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
#ifndef _FILE_DIALOGS_H_
#define _FILE_DIALOGS_H_
 
#include <QObject>
#include <QFileDialog>
#include <QList>
#include <QHash>
#include "plugins/interfaces/IFileDialog.h"


class FileDlgHistory{
	public:
		QString getDir(const QString& name);
		int getFilter(const QString& name);
		void set(const QString& name,const QString& dir,int filter);
		void save();
		void load();	
	private:
		class Entry{
			public:
				Entry(){Filter=-1;}
				QString Dir;
				int Filter;
				bool isNull(){if(Filter==-1)return true;
								else return false;}
		};
		
		QHash<QString,Entry>History;
	
};


class KpxFileDialogs{
	public:
		static void setPlugin(IFileDialog* FileDlgPlugin);
		static QString openExistingFile(QWidget* parent, const QString& Name,
										const QString& Title,
										const QStringList& Filters,
										const QString& Dir=QString());
		static QStringList openExistingFiles(QWidget* parent, const QString& Name,
											const QString& Title,
											const QStringList& Filters,
											const QString& Dir=QString());
		static QString saveFile(QWidget* parent, const QString& Name,
									const QString& Title,
									const QStringList& Filters,
									bool ShowOverwriteWarning=true,
									const QString& Dir=QString()
								   );		
	private:
		static IFileDialog* iFileDialog;



};


class QtStandardFileDialogs:public QObject,public IFileDialog{
	Q_OBJECT	
		public:
			QString openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters);
			QStringList openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters);		
			QString saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,bool ShowOverwriteWarning);
			int getLastFilter();
		private:
			int LastFilter;
	
};

extern FileDlgHistory fileDlgHistory; 
 
#endif
