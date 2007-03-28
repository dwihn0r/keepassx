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
 
#ifndef KPXFIREFOX_H
#define KPXFIREFOX_H

/*
#include <QtDBus/QtDBus>
#include <QObject>
#include "Database.h"

class KpxFirefox:public QObject
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.keepassx.DBus.KpxFirefox")
	public:
		KpxFirefox(IDatabase* db){};
		//~KpxFirefox(){};
	public slots:
		QString getMyValue(int Zahl){return QString("You sent the number %1 over DBUS").arg(Zahl);}
};

class KpxFirefoxAdaptor:public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.keepassx.DBus.KpxFirefox")
	public:
		KpxFirefoxAdaptor(KpxFirefox* kpx);
		~KpxFirefoxAdaptor(){};
	public slots:
		QString getMyValue(int Zahl){
			QString serv=QDBusConnection::sender().baseService();
			QDBusConnectionInterface *inter=QDBusConnection::sender().interface();
			qDebug("PID=%i",inter->servicePid(serv).value());
			return kpx->getMyValue(Zahl);
		
		}
	private:
		KpxFirefox* kpx;
};
*/


#endif
