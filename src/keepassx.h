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

#ifndef KEEPASSX_H_
#define KEEPASSX_H_

// global defines 

#define APP_DISPLAY_NAME    "KeePassX"
#define APP_CODE_NAME       "keepassx"
#define APP_SHORT_FUNC      "Password Manager"
#define APP_LONG_FUNC       "Cross Platform Password Manager"
#define APP_VERSION         "0.3.0a"

#define BUILTIN_ICONS 65

// often used, stable header files for pch

#ifdef __cplusplus

#include <iostream>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <cstdlib>
#include <QHash>
#include <QList>
#include <QStringList>
#include <QBitArray>
#include <QByteArray>
#include <QVarLengthArray>
#include <QDataStream>
#include <QString>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QPixmap>
#include <QMessageBox>
#include <QDialog>
#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSettings>
#include <QLabel>
#include <QDateTime>
#include <QTreeWidget>
#include <QTimer>
#include <QApplication>
#include <QPaintEvent>
#include <QDomElement>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QUrl>

#include "crypto/blowfish.h"
#include "crypto/sha1.h"
#include "crypto/twoclass.h"
#include "crypto/aescpp.h"
#include "crypto/sha256.h"
#include "crypto/yarrow.h"
#include "crypto/arcfour.h"
#include "lib/random.h"
#include "lib/SecString.h"
#include "lib/tools.h"
#include "lib/FileDialogs.h"
#include "lib/bookmarks.h"
#include "lib/UrlLabel.h"
#include "KpxConfig.h"
#include "Database.h"

extern QString PluginLoadError;
extern KpxConfig *config;
extern QString AppDir;
extern QString HomeDir;
extern QString DataDir;
extern bool TrActive;
extern QString DetailViewTemplate;
extern QPixmap *EntryIcons;

#endif //__cplusplus
#endif //KEEPASS_X_