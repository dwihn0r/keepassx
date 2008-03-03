/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QColor>
#include <QIcon>
#include <QFile>

#define APP_DISPLAY_NAME    "KeePassX"
#define APP_CODE_NAME       "keepassx"

#define APP_SHORT_FUNC      "Password Manager"
#define APP_LONG_FUNC       "Cross Platform Password Manager"

#define APP_VERSION         "0.3.0a"

#define BUILTIN_ICONS 65

#define CSTR(x)(x.toLocal8Bit().constData())

class IEntryHandle;

typedef enum tKeyType {PASSWORD=0,KEYFILE=1,BOTH=2};
class KpxConfig;

void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width);
void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width, QColor Color1, QColor Color2, QColor TextColor);
void openBrowser(IEntryHandle* entry);
void openBrowser(const QString& UrlString);
void showErrMsg(const QString& msg,QWidget* parent=NULL);
QString applicationFilePath();
QString applicationDirPath();
const QIcon& getIcon(const QString& name);
const QPixmap* getPixmap(const QString& name);
QString decodeFileError(QFile::FileError Code);
//QString findPlugin(const QString& filename); //TODO Plugins
QString makePathRelative(const QString& Abs,const QString& Cur);
extern QString PluginLoadError;

extern KpxConfig *config;
extern QString AppDir;
extern QString HomeDir;
extern QString DataDir;
extern bool TrActive;
extern QString DetailViewTemplate;
extern QPixmap *EntryIcons;


#endif
