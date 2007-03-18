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
#ifndef _EXPORT_H_
#define _EXPORT_H_

#include <QWidget>
#include <QFile>

#include "Database.h"
 
class IExport{
	public:
		virtual ~IExport(){};
		virtual bool exportDatabase(QWidget* GuiParent, IDatabase* Database)=0;
		virtual QString identifier()=0;
		virtual QString title()=0;
};

class ExporterBase:public QObject{
	protected:
		virtual QFile* openFile(QWidget* GuiParent,QString id,QStringList Filter);
};




#endif