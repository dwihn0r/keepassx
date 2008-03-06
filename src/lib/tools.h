/***************************************************************************
 *   Copyright (C) 2005-2008 by Tarek Saidi                                *
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
#ifndef TOOLS_H
#define TOOLS_H

#define CSTR(x)(x.toLocal8Bit().constData())
class IEntryHandle;
typedef enum tKeyType {PASSWORD=0,KEYFILE=1,BOTH=2};
const QIcon& getIcon(const QString& name);
const QPixmap* getPixmap(const QString& name);
void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width);
void createBanner(QPixmap* Pixmap, const QPixmap* IconAlpha,const QString& Text,int Width, QColor Color1, QColor Color2, QColor TextColor);
void openBrowser(const QString& UrlString);
void openBrowser(IEntryHandle* entry);
void showErrMsg(const QString& msg,QWidget* parent=NULL);
QString decodeFileError(QFile::FileError Code);
QString makePathRelative(const QString& Abs,const QString& Cur);
QString getImageFile(const QString& name);
bool createKeyFile(const QString& filename,QString* err, int length=32, bool Hex=true);

#endif //TOOLS_H
