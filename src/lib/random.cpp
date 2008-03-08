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
#include <iostream>
#include <fstream>
#include "random.h"

#if defined(Q_WS_WIN)
	#include <QSysInfo>
#include <QMessageBox>
	#include <windows.h>
#endif

using namespace std;

void getRandomBytes(void* buffer,int NumBlocks){
#if defined(Q_WS_WIN)
	// RtlGenRandom
	if (QSysInfo::WindowsVersion>=QSysInfo::WV_XP){
		bool success=false;
		HMODULE hLib=LoadLibraryA("ADVAPI32.DLL");
		if (hLib) {
			BOOLEAN (APIENTRY *pfn)(void*, ULONG) = (BOOLEAN (APIENTRY *)(void*,ULONG))GetProcAddress(hLib,"SystemFunction036");
			if (pfn && pfn(buffer,NumBlocks)) {
				success=true;
			}
			FreeLibrary(hLib);
		}
		if (success)
			return;
	}
#else
	FILE* dev_random = fopen("/dev/random","r");
	if (dev_random){
		size_t bytesRead = fread(buffer,1,NumBlocks,dev_random);
		fclose(dev_random);
		if (bytesRead==NumBlocks)
			return;
	}
#endif
	
	srand(time(NULL));
	for(int i=0;i<NumBlocks;i++){
		((quint8*)buffer)[i] = (quint8) (rand()%256);
	}
}
