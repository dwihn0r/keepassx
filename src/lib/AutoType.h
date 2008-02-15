/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi, Felix Geyer                   *
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

#ifndef _AUTOTYPE_H_
#define _AUTOTYPE_H_

#include <QString>
#include "Database.h"

#ifdef GLOBAL_AUTOTYPE
struct Shortcut{
	bool ctrl, shift, alt, altgr, win;
	quint32 key;
};
#endif

#ifdef AUTOTYPE
class KeepassMainWindow;

class AutoType{
	public:
		static KeepassMainWindow* MainWin;
		static void perform(IEntryHandle* entry, QString& err,bool hideWindow=true,int nr=0);
#ifdef GLOBAL_AUTOTYPE
		static Shortcut shortcut;
		static void performGlobal();
		static bool registerGlobalShortcut(const Shortcut& s);
		static void unregisterGlobalShortcut();
#endif // GLOBAL_AUTOTYPE
};
#endif // AUTOTYPE

#endif
