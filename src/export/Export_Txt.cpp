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
 
#include <QString>
#include <QFile>
#include "main.h"
#include "../lib/SecString.h"
#include "Export_Txt.h"

QString EntryTemplate=QString("\n\
  Title:    %1\n\
  Username: %2\n\
  Url:      %3\n\
  Password: %4\n\
  Comment:  %5\n\
");

QString GroupTemplate=QString("\n\
*** Group: %1 ***\n\
");

bool Export_Txt::exportFile(const QString& filename,PwDatabase* db,QString& err){
QFile file(filename);
if(!file.open(QIODevice::Truncate | QIODevice::WriteOnly)){
	err+=tr("Could not open file (FileError=%1)").arg(file.error());
	return false;
}

for(int g=0;g<db->Groups.size();g++){
	file.write(GroupTemplate.arg(db->Groups[g].Name).utf8());
	for(int e=0;e<db->Entries.size();e++){
		if(db->Groups[g].ID==db->Entries[e].GroupID){
			db->Entries[e].Password.unlock();
			file.write(EntryTemplate.arg(db->Entries[e].Title)
									.arg(db->Entries[e].UserName)
									.arg(db->Entries[e].URL)
									.arg(db->Entries[e].Password.string())
									.arg(db->Entries[e].Additional)
									.utf8());
			db->Entries[e].Password.lock();
		}
	}
}
file.close();
}