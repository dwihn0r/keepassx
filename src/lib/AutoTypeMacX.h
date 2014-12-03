/***************************************************************************
 *   Copyright (C) 2009 by Jeff Gibbons                                    *
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

#ifndef _AUTOTYPEMACX_H_
#define _AUTOTYPEMACX_H_

#include "lib/AutoType.h"
#include <Carbon/Carbon.h>

typedef quint32	KeySym;
#define NoSymbol  (KeySym)0
#define NoKeycode (uint16)-1

struct KeycodeWithMods {
	uint16 keycode;
	uint16 mods;
};

enum AutoTypeActionType{
	SendUnicodeAction, SendKeycodeAction, DelayAction
};

struct AutoTypeAction{
	AutoTypeAction(AutoTypeActionType t, quint16		 data);
	AutoTypeAction(AutoTypeActionType t, KeycodeWithMods keycodeWithMods);
	AutoTypeActionType type;
	union {
		quint16		 data;
		KeycodeWithMods keycodeWithMods;
	};
};

class AutoTypeMacX : public AutoType {
	public:
		AutoTypeMacX(KeepassMainWindow* mainWin);
		void perform(IEntryHandle* entry, bool hideWindow=true, int nr=0, bool wasLocked=false);
		virtual void updateKeymap();
	
	protected:
		void sleepTime(int msec);
		inline void sleepKeyStrokeDelay(){ sleepTime(config->autoTypeKeyStrokeDelay()); };
		void templateToKeysyms(const QString& Template, QList<AutoTypeAction>& KeySymList,IEntryHandle* entry);
		void stringToKeysyms(const QString& string,QList<AutoTypeAction>& KeySymList);
		void sendKeycode(KeycodeWithMods keycodeWithMods);
		void sendUnicode(KeySym keysym);
		void flushUnicode();
		
		KeepassMainWindow* mainWin;
		
		int   ctrl_mask;
		int   shift_mask;
		int   alt_mask;
		int   meta_mask;
		int   altgr_mask;
		bool  inGlobalAutoType;
		pid_t targetPID;
		int   windowNumber;
	
	private:
        bool   inAutoType;
        OSType appSignature;
		bool   onlySendKeycodes;
		void   checkWindowType();
		void   keyDownUp(CGEventRef theEvent);
};

#endif // _AUTOTYPEMACX_H_
