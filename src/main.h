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

#define KEEPASS_VERSION "0.2.1"
#define BUILTIN_ICONS 62

typedef enum tKeyType {PASSWORD=0,KEYFILE=1,BOTH=2};
class CConfig;

void createBanner(QLabel *Banner,QPixmap* symbol,QString text);
void createBanner(QLabel *Banner,QPixmap* symbol,QString text,QColor color1,QColor color2,QColor textcolor);
void openBrowser(QString url);
void showErrMsg(const QString& msg,QWidget* parent=NULL);

extern CConfig  config;
extern QString  AppDir;
extern bool TrActive;
extern QPixmap *EntryIcons;
extern QPixmap *Icon_Key32x32;
extern QPixmap *Icon_Settings32x32;
extern QPixmap *Icon_Search32x32;
extern QPixmap *Icon_I18n32x32;
extern QPixmap *Icon_Ok16x16;
extern QIcon *Icon_FileNew;
extern QIcon *Icon_FileOpen;
extern QIcon *Icon_FileClose;
extern QIcon *Icon_FileSave;
extern QIcon *Icon_FileSaveAs;
extern QIcon *Icon_Exit;
extern QIcon *Icon_File_Export;
extern QIcon *Icon_EditDelete;
extern QIcon *Icon_EditAdd;
extern QIcon *Icon_EditEdit;
extern QIcon *Icon_EditDelete;
extern QIcon *Icon_EditUsernameToCb;
extern QIcon *Icon_EditPasswordToCb;
extern QIcon *Icon_EditClone;
extern QIcon *Icon_EditOpenUrl;
extern QIcon *Icon_EditSearch;
extern QIcon *Icon_Configure;
extern QIcon *Icon_Help;
extern QIcon *Icon_AutoType;


#endif
