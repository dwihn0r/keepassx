/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
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
#ifndef _SECSTRING_H_
#define _SECSTRING_H_

#include <qstring.h>
#include <qglobal.h>
#include "crypto/rijndael.h"

class SecString{
public:
 SecString(unsigned char* key);
 SecString();
 ~SecString();

 void getString(QString& str);
 QString& getString();
 void setString(QString& str, bool DelSrc=false);
 void delRef();
 static void overwrite(char* str,int len);
 static void overwrite(QString& str);
 int length();
 static void generateSessionKey();

private:
 static UINT8 Key[32];
 QString plaintext;
 char* data;
 int len;
 int cryptlen;
};


#endif