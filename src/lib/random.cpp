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

#include "random.h"

#if defined(Q_WS_WIN)
	#include <windows.h>
	#include <QSysInfo>
#endif

RandomSource::RandomSource(){
	quint8 buffer[100];
	for (int i=0; i<2; i++){
		getRandomWeak(buffer,100);
		yarrowUpdateWeak(i,100*8,100,buffer);
	}
	
#ifdef HAS_DEV_RANDOM
	if (QFile::exists("/dev/random")){
		DevRandom* devRandom = new DevRandom(this);
		connect(devRandom, SIGNAL(randomAvailable(int,QByteArray,int)), SLOT(seedStrong(int,QByteArray,int)));
		connect(devRandom, SIGNAL(finished()), SLOT(deleteLater()));
		devRandom->start();
	}
	else{
		deleteLater();
	}
#else
	deleteLater();
#endif
}

void RandomSource::getRandomWeak(quint8* buffer, int length){
#if defined(HAS_DEV_RANDOM)
	QFile dev_urandom("/dev/urandom");
	if (dev_urandom.open(QIODevice::ReadOnly|QIODevice::Unbuffered) && dev_urandom.read((char*)buffer,length)==length)
		return;
#elif defined(Q_WS_WIN)
	// RtlGenRandom
	if (QSysInfo::WindowsVersion>=QSysInfo::WV_XP){
		bool success=false;
		HMODULE hLib=LoadLibraryA("ADVAPI32.DLL");
		if (hLib) {
			BOOLEAN (APIENTRY *pfn)(void*, ULONG) = (BOOLEAN (APIENTRY *)(void*,ULONG))GetProcAddress(hLib,"SystemFunction036");
			if (pfn && pfn(buffer,length)) {
				success=true;
			}
			FreeLibrary(hLib);
		}
		if (success)
			return;
	}
#endif
	
	srand(time(NULL));
	for(int i=0;i<length;i++){
		((quint8*)buffer)[i] = (quint8) (rand()%256);
	}
}

#ifdef HAS_DEV_RANDOM
void RandomSource::seedStrong(int source, QByteArray buffer, int length){
	yarrowUpdateStrong(source,length*8,length,(const quint8*)buffer.constData());
}

DevRandom::DevRandom(QObject* parent) : QThread(parent){
}

void DevRandom::run(){
	QByteArray buffer(50,0);
	for (int i=0; i<2; i++){
		if (getRandomStrong((quint8*)buffer.data(),50))
			emit randomAvailable(i,buffer,50);
	}
}

bool DevRandom::getRandomStrong(quint8* buffer, int length){
	QFile dev_random("/dev/random");
	return (dev_random.open(QIODevice::ReadOnly|QIODevice::Unbuffered) && dev_random.read((char*)buffer,length)==length);
}
#endif
