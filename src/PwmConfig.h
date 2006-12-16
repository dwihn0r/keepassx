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

#include <QColor>
#include "main.h"
#include "lib/IniReader.h"
		
class CConfig{
	public:
	enum IntegrPluginType{NONE,KDE,GNOME};
		int TimeFormat;
		int ClipboardTimeOut;
		int MainWinHeight;
		int MainWinWidth;
		int MainWinSplit1;
		int MainWinSplit2;
		int ToolbarIconSize;
		int PwGenLength;
		int GroupTreeRestore; // 0:Restore Last; 1:Expand All; 2:Don't Expand
		int ColumnSizes[10];
		bool Toolbar;
		bool EntryDetails;
		bool OpenLast;		
		bool ListView_HidePasswords;
		bool ListView_HideUsernames;
		bool SearchOptions[9];
		bool PwGenOptions[10];
		bool Columns[10];
		bool ShowPasswords;
		bool ExpandGroupTree;
		bool EnableKdePlugin;
		bool ShowStatusbar;
		bool AlternatingRowColors;
		bool RememberLastKey;
		bool ShowSysTrayIcon;
		bool MinimizeToTray;
		bool SaveFileDlgHistory;
		bool EnableBookmarkMenu;
		QString PwGenCharList;
		QString MountDir;
		QString LastKeyLocation;
		QString OpenUrlCommand;
		QString Language;
		QString LastFile;		
		QColor BannerColor1;
		QColor BannerColor2;
		QColor BannerTextColor;		
		IntegrPluginType IntegrPlugin;
		tKeyType LastKeyType;
		
		bool loadFromIni(QString filename);
		bool saveToIni(QString filename);

	private:
		CIniFile ini;
		void ParseColumnString(QString str, bool* dst);
		void ParseBoolString(const QString &str,const QString &defaults,bool* dst, int count);
		void ParseIntString(const QString &str,int* dst, int count);
		QString CreateBoolString(bool* src, int count);
		QColor ParseColorString(QString str);
		QString CreateColumnString();
		QString CreateColorString(QColor);
		QString CreateIntString(int* src, int count);
};

#endif