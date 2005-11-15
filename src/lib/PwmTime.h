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
#ifndef _PWMTIME_H_
#define _PWMTIME_H_

#include <qglobal.h>
#include <qstring.h>
class CPwmTime;
class CPwmTime{
public:
  Q_UINT16 Year;
  Q_UINT8 Month;
  Q_UINT8 Day;
  Q_UINT8 Hour;
  Q_UINT8 Minute;
  Q_UINT8 Second;

  QString GetString(Q_UINT16 format);
  void Set(Q_UINT8,Q_UINT8,Q_UINT16,Q_UINT8,Q_UINT8,Q_UINT8);
  void Set(unsigned char* packedTime);
  void GetPackedTime(unsigned char* dst);
  static bool IsValidDate(QString& string);
  static bool IsValidTime(QString& string);
  bool SetDate(QString string);
  bool SetTime(QString string);
  void SetToNow();
  bool operator==(const CPwmTime& t);
  bool operator!=(const CPwmTime& t);
//  bool operator>(const CPwmTime& t);
//  bool operator<(const CPwmTime& t);

};

#endif
