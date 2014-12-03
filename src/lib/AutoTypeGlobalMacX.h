/***************************************************************************
 *   Copyright (C) 2009 Jeff Gibbons                                       *
 *   Copyright (C) 2005-2008 by Tarek Saidi, Felix Geyer                   *
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

#ifndef _AUTOTYPEGLOBALMACX_H_
#define _AUTOTYPEGLOBALMACX_H_

#include "lib/AutoTypeMacX.h"

class AutoTypeGlobalMacX : public AutoTypeMacX, public AutoTypeGlobal{
	public:
		AutoTypeGlobalMacX(KeepassMainWindow* mainWin);
		void perform(IEntryHandle* entry, bool hideWindow=true, int nr=0, bool wasLocked=false);
		void performGlobal();
		bool registerGlobalShortcut(const Shortcut& s);
		void unregisterGlobalShortcut();
		QStringList getAllWindowTitles();
		void  cancelled();
		void  updateKeymap();
		inline int maskShift() { return shift_mask; };
		inline int maskCtrl()  { return  ctrl_mask; };
		inline int maskAlt()   { return   alt_mask; };
		inline int maskAltGr() { return altgr_mask; };
		inline int maskMeta()  { return  meta_mask; };
	
	private:
		QString getRootGroupName(IEntryHandle* entry);

		int  oldCode;
		uint oldMod;

		EventHotKeyRef hotKeyRef;
		EventHotKeyID  hotKeyID;
};

#endif // _AUTOTYPEGLOBALMACX_H_
