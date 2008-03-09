/***************************************************************************
 *   Copyright (C) 2005-2008 by Felix Geyer                                *
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


#include "Application_X11.h"
#include "lib/AutoType.h"
#include "lib/HelperX11.h"

const unsigned int KeepassApplication::remove_invalid = ControlMask|ShiftMask|Mod1Mask|Mod5Mask|Mod4Mask;

KeepassApplication::KeepassApplication(int& argc, char** argv) : QApplication(argc, argv){
}

bool KeepassApplication::x11EventFilter(XEvent* event){
	if (event->type==KeyPress && AutoType::shortcut.key!=0u && event->xkey.keycode==XKeysymToKeycode(event->xkey.display,HelperX11::getKeysym(AutoType::shortcut.key)) && (event->xkey.state&remove_invalid)==HelperX11::getShortcutModifierMask(AutoType::shortcut) && focusWidget()==NULL ){
		EventOccurred = true;
		AutoType::performGlobal();
		return true;
	}
	else{
		return QApplication::x11EventFilter(event);
	}
}
