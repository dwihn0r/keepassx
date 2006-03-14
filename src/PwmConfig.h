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
#ifndef _PWMCONFIG_H_
#define _PWMCONFIG_H_

#include "lib/IniReader.h"
#include <qcolor.h>

class CConfig{
public:
 int TimeFormat;
 int ClipboardTimeOut;
 bool Toolbar;
 bool EntryDetails;
 QString LastFile;
 bool OpenLast;
 bool Columns[10];
 QColor BannerColor1;
 QColor BannerColor2;
 QColor BannerTextColor;
 bool ShowPasswords;
 QString OpenUrlCommand;
 QString Language;
 bool SearchOptions[9];
 bool ListView_HidePasswords;
 bool ListView_HideUsernames;
 bool PwGenOptions[10];
 int PwGenLength;
 QString PwGenCharList;
 bool ExpandGroupTree;
 bool EnableKdePlugin;
 int MainWinHeight;
 int MainWinWidth;
 int MainWinSplit1;
 int MainWinSplit2;
 int ColumnSizes[10];
 bool ShowStatusbar;
 bool AlternatingRowColors;

 bool loadFromIni(QString filename);
 bool saveToIni(QString filename);

private:
 CIniFile ini;
 void ParseColumnString(QString str, bool* dst);
 void ParseBoolString(const QString &str,bool* dst, int count);
 void ParseIntString(const QString &str,int* dst, int count);
 QString CreateBoolString(bool* src, int count);
 QColor ParseColorString(QString str);
 QString CreateColumnString();
 QString CreateColorString(QColor);
 QString CreateIntString(int* src, int count);
};

#endif