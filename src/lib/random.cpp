/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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
#include <iostream>
#include <fstream>
#include <qdatetime.h>
#include <qobject.h>
#include "random.h"
using namespace std;

void getRandomBytes(void* buffer,int NumBlocks,int BlockSize,bool Strong){
FILE *dev_random;
if(Strong){
dev_random = fopen("/dev/random","r");}
else{
dev_random = fopen("/dev/urandom","r");}

if (dev_random==NULL){
 srand(QTime(0,0,0).secsTo(QTime::currentTime()));
 for(int i=0;i<NumBlocks*BlockSize;i++){
	quint8 rnd=rand()%256;
	((quint8*)buffer)[i]=rnd;
	}
 return;
}
else{
fread(buffer,BlockSize,NumBlocks,dev_random);
fclose(dev_random);
return;
}
}
