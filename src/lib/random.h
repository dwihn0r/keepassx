/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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
#ifndef RANDOM_H_
#define RANDOM_H_

#include <QObject>

#if defined(Q_WS_X11) || defined(Q_WS_MAC)
#define HAS_DEV_RANDOM
#include <QThread>
#endif

class RandomSource : public QObject {
	Q_OBJECT
	
	public:
		RandomSource();
	
	private:
		static void getRandomWeak(quint8* buffer, int length);

#ifdef HAS_DEV_RANDOM
	private slots:
		void seedStrong(int source, QByteArray buffer, int length);
#endif
};

#ifdef HAS_DEV_RANDOM
class DevRandom : public QThread {
	Q_OBJECT
	
	public:
		DevRandom(QObject* parent = 0);
		void run();
		
	signals:
		void randomAvailable(int source, QByteArray buffer, int length);
		
	private:
		static bool getRandomStrong(quint8* buffer, int length);
};
#endif

#endif
