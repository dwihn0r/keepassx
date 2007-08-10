/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
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

#ifndef _ARCFOUR_H_
#define _ARCFOUR_H_

#ifndef byte
#define byte unsigned char
#endif

class CArcFour{
public:
 void encrypt(const byte* src, byte* dst,int length);
 inline void decrypt(const byte* src, byte* dst,int length){encrypt(src,dst,length);} //just for readability
 void setKey(byte* key, int length);
 QByteArray RawKey;
private:
 void prepareKey();

 typedef struct rc4_key{      
   byte state[256];      
   byte x;        
   byte y;}rc4_key;
 rc4_key key;
};


#endif
