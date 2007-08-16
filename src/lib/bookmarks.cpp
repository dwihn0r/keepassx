/***************************************************************************
 *   Copyright (C) 2007 by Tarek Saidi                                     *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <QFile>
#include "bookmarks.h"
#include "main.h"
#define CSTR(x)(x.toUtf8().data())

QList<KpxBookmarks::BookmarkEntry> KpxBookmarks::Bookmarks;
QString KpxBookmarks::filename;

bool KpxBookmarks::load(const QString& _filename){
	/* 
      Fileformat:
	   "Title1" "Path1"\n
	   "Title2" "Path2"\n
		...	 
	*/
	
	filename=_filename;
	QFile file(filename);
	if(!file.exists()){
		return true;
	}
	if(!file.open(QIODevice::ReadOnly)){
		qWarning("Reading bookmarks failed: %s",CSTR(decodeFileError(file.error())));
		return false;		
	}
	QString content=QString::fromUtf8(file.readAll());
	file.close();
	content.replace("\r","");
	QStringList lines=content.split("\n");
	for(int i=0;i<lines.size();i++){
		if(lines[i].simplified()==QString()) continue; //skip empty line
		if(lines[i].count("\"")!=4){
			qWarning("Bookmark parsing error: Skipping line %i.",i);
			continue;		
		}
		int a_title=lines[i].indexOf("\"");
		int b_title=lines[i].indexOf("\"",a_title+1);
		int a_path=lines[i].indexOf("\"",b_title+1);
		int b_path=lines[i].indexOf("\"",a_path+1);
		BookmarkEntry entry;
		entry.Title=lines[i].mid(a_title+1,b_title-a_title-1);
		entry.Path=lines[i].mid(a_path+1,b_path-a_path-1);
		Bookmarks << entry;
	}
	return true;		
}

int KpxBookmarks::count(){
	return Bookmarks.size();
}

QString KpxBookmarks::title(int i){
	return Bookmarks[i].Title;
}

QString KpxBookmarks::path(int i){
	return Bookmarks[i].Path;
}


int KpxBookmarks::add(const QString& Title,const QString& Path){
	BookmarkEntry entry;
	entry.Title=Title;
	entry.Path=Path;
	entry.Index=Bookmarks.size();
	Bookmarks<<entry;
	save();
	return Bookmarks.size()-1;
}

bool KpxBookmarks::save(){
	QFile file(filename);
	if(!file.exists()){
		return false;
	}
	if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		qWarning("Writing bookmarks failed: %s",CSTR(decodeFileError(file.error())));
		return false;		
	}
	QString data;
	for(int i=0;i<Bookmarks.size();i++){
		data+=QString("\"%1\" \"%2\"\n").arg(Bookmarks[i].Title)
				                        .arg(Bookmarks[i].Path);		
	}
	file.write(data.toUtf8());
	file.close();	
}

void KpxBookmarks::remove(int index){
	Bookmarks.removeAt(index);
	save();
}

void KpxBookmarks::edit(const QString& Title,const QString& Path,int i){
	Bookmarks[i].Title=Title;
	Bookmarks[i].Path=Path;
	save();
}


void KpxBookmarks::resort(QList<int> order){
	QList<BookmarkEntry> NewList;
	for(int i=0;i<order.size();i++){
		NewList << Bookmarks[order[i]];		
	}
	Bookmarks=NewList;
	save();
}