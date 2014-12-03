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

#ifndef HELPERMACX_H
#define HELPERMACX_H

#include "lib/AutoTypeMacX.h"

class HelperMacX{
	public:
#ifdef GLOBAL_AUTOTYPE
		static			  uint getShortcutModifierMask(const Shortcut& s);
		static		   Boolean isFrontProcess(pid_t pid);
#endif
		static			 pid_t getKeepassxPID();
		static		   Boolean getTargetWindowInfo(pid_t *pidPtr, int *windowNumberPtr, char* windowName, int maxWindowNameSize);
		static			OSType getProcessSignature(pid_t pid);
		static			  void initUnicodeToKeycodeWithModsMap();
		static			KeySym keycodeToKeysym(uint keycode);
		static			  uint keysymToKeycode(KeySym keysym);
		static KeycodeWithMods keysymToKeycodeWithMods(KeySym keysym);
		static			  void processToFront(pid_t pid);
};

#endif // HELPERMACX_H
