/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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
#ifndef _MAIN_H_
#define _MAIN_H_

#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QColor>
#include <QIcon>
#include <QFile>

#define APP_NAME "KeePassX"
#define APP_FUNC "Password Manager"
#define KEEPASS_VERSION "0.2.3"
#define BUILTIN_ICONS 62

typedef enum tKeyType {PASSWORD=0,KEYFILE=1,BOTH=2};
class KpxConfig;

void createBanner(QLabel *Banner,const QPixmap* symbol,QString text);
void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width);
void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width, QColor Color1, QColor Color2, QColor TextColor);
void openBrowser(QString url);
void showErrMsg(const QString& msg,QWidget* parent=NULL);
const QIcon& getIcon(const QString& name);
const QPixmap* getPixmap(const QString& name);
QString decodeFileError(QFile::FileError Code);
QString findPlugin(const QString& filename);
QString makePathRelative(const QString& Abs,const QString& Cur);
void loadDefaultDetailViewTemplate();
extern QString PluginLoadError;

extern KpxConfig *config;
extern QString AppDir;
extern bool TrActive;
extern QString DetailViewTemplate;
extern QPixmap *EntryIcons;


#endif
