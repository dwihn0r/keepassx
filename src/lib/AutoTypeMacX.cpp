/***************************************************************************
 *   Copyright (C) 2009-2010 Jeff Gibbons                                  *
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

#include "lib/AutoTypeMacX.h"
#include "lib/HelperMacX.h"
#include "mainwindow.h"

#ifndef GLOBAL_AUTOTYPE
AutoType* autoType = NULL;

void initAutoType(KeepassMainWindow* mainWin) {
	autoType = new AutoTypeMacX(mainWin);
}
#endif

// resusable events
static CGEventRef	 keyEvent = CGEventCreateKeyboardEvent(NULL, 0, true);
static CGEventRef unicodeEvent = CGEventCreateKeyboardEvent(NULL, 0, true);

#define UNICODE_BUFFER_SIZE 20 // max from documentation
static UniChar	  unicodeBuffer[UNICODE_BUFFER_SIZE];
static UniCharCount unicodePtr = 0;

AutoTypeAction::AutoTypeAction(AutoTypeActionType t, quint16 data)
	: type(t), data(data){}
AutoTypeAction::AutoTypeAction(AutoTypeActionType t, KeycodeWithMods keycodeWithMods)
	: type(t), keycodeWithMods(keycodeWithMods){}

AutoTypeMacX::AutoTypeMacX(KeepassMainWindow* mainWin){
	this->mainWin = mainWin;
	inAutoType = false;
	targetPID  = 0;
	
	ctrl_mask  = controlKey;
	shift_mask =   shiftKey;
	alt_mask   =  optionKey;
	meta_mask  =	 cmdKey;
	altgr_mask = 0;

	updateKeymap();
}

void AutoTypeMacX::updateKeymap(){
	HelperMacX::initUnicodeToKeycodeWithModsMap();
}

void AutoTypeMacX::perform(IEntryHandle* entry, bool hideWindow, int nr, bool wasLocked){
	if (inAutoType)
		return;
	if ((0 == targetPID) && !HelperMacX::getTargetWindowInfo(&targetPID, &windowNumber, NULL, 0)) {
		targetPID = 0;
		return;
	}
	inAutoType = true;
	
	QString indexStr;
	if (nr==0)
		indexStr = "Auto-Type:";
	else
		indexStr = QString("Auto-Type-%1:").arg(nr);
	QString str;
	QString comment=entry->comment();
	int c=comment.count(indexStr, Qt::CaseInsensitive);
	if(c>1) {
		qWarning("More than one 'Auto-Type:' key sequence found.\nAllowed is only one per entry.");
		targetPID  = 0;
		inAutoType = false;
		return;
	}
	else if (c==1) {
		int start = comment.indexOf(indexStr,0,Qt::CaseInsensitive) + indexStr.length();
		int end = comment.indexOf("\n", start);
		if (end == -1)
			end = comment.length();
		
		str=comment.mid(start,end-start).trimmed();
		if (str.isEmpty()) {
			targetPID = 0;
			inAutoType = false;
			return;
		}
	}
	else {
		bool usernameEmpty = entry->username().trimmed().isEmpty();
		SecString password=entry->password();
		password.unlock();
		bool passwordEmpty = password.string().trimmed().isEmpty();
		if (usernameEmpty && passwordEmpty) {
			targetPID  = 0;
			inAutoType = false;
			return;
		}
		else if (usernameEmpty)
			str="{PASSWORD}{ENTER}";
		else if (passwordEmpty)
			str="{USERNAME}{ENTER}";
		else
			str="{USERNAME}{TAB}{PASSWORD}{ENTER}";
	}
	
    HelperMacX::processToFront(targetPID);
    appSignature = HelperMacX::getProcessSignature(targetPID);
	checkWindowType();
	targetPID = 0;
	if (hideWindow)
		mainWin->hide();
	
	QList<AutoTypeAction> Keys;
	for(int i=0;i<str.size();i++){
		if(str[i]=='{'){
			QString tmpl;
			i++;
			while(str[i]!='}' && i<str.size()){
				tmpl += str[i];
				i++;
			}
			if(i>=str.size()){
				qWarning("Syntax Error in Auto-Type sequence near character %d\nFound '{' without closing '}'", i+10);
				break;
			}
			templateToKeysyms(tmpl.toLower(),Keys,entry);
			continue;
		}
		else{
			Keys << AutoTypeAction(SendUnicodeAction, str[i].unicode());
		}
	}
	
	QApplication::processEvents();
	sleepTime(config->autoTypePreGap());
	
	QString type;
	for(int i=0;i<Keys.size();i++){
		int currentWindowNumber;
		if (!HelperMacX::getTargetWindowInfo(NULL, &currentWindowNumber, NULL, 0)
			|| (windowNumber != currentWindowNumber)) {
			qWarning("Focus window changed, interrupting auto-type");
			unicodePtr = 0;
			break;
		}
		if (Keys[i].type==SendKeycodeAction){
			sendKeycode(Keys[i].keycodeWithMods);
		}
		else if (Keys[i].type==SendUnicodeAction){
			sendUnicode(Keys[i].data);
		}
		else if (Keys[i].type==DelayAction){
			flushUnicode();
			QApplication::processEvents();
			sleepTime(Keys[i].data);
		}
	}
	flushUnicode();
	
	if (config->lockOnMinimize()){
		if (hideWindow || wasLocked){
			if ( !(config->showSysTrayIcon() && config->minimizeTray()) )
				mainWin->showMinimized();
			else
				mainWin->OnUnLockWorkspace();
		}
	}
	else{
		if (hideWindow && !(config->showSysTrayIcon() && config->minimizeTray()) )
			mainWin->showMinimized();
	}
	
	inAutoType = false;
}

void AutoTypeMacX::sleepTime(int msec){
	if (msec==0) return;
	timespec timeOut, remains;
	timeOut.tv_sec = msec/1000;
	timeOut.tv_nsec = (msec%1000)*1000000;
	nanosleep(&timeOut, &remains);
}

void AutoTypeMacX::templateToKeysyms(const QString& tmpl, QList<AutoTypeAction>& keys,IEntryHandle* entry){
	//tmpl must be lower case!!!
	if(!tmpl.compare("title")){
		stringToKeysyms(entry->title(),keys);
		return;
	}
	if(!tmpl.compare("username")){
		stringToKeysyms(entry->username(),keys);
		return;
	}
	if(!tmpl.compare("url")){
		stringToKeysyms(entry->url(),keys);
		return;
	}
	if(!tmpl.compare("password")){
		SecString password=entry->password();
		password.unlock();
		stringToKeysyms(password,keys);
		return;
	}
	if(!tmpl.compare("space")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Space, 0});
		return;
	}
	
	if(!tmpl.compare("backspace") || !tmpl.compare("bs") || !tmpl.compare("bksp")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Delete, 0});
		return;
	}
	
//	if(!tmpl.compare("break")){
//		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){?, 0});
//		return;
//	}
	
	if(!tmpl.compare("capslock")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_CapsLock, 0});
		return;
	}
	
	if(!tmpl.compare("del") || !tmpl.compare("delete")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ForwardDelete, 0});
		return;
	}
	
	if(!tmpl.compare("end")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_End, 0});
		return;
	}
	
	if(!tmpl.compare("enter")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Return, 0});
		return;
	}
	
	if(!tmpl.compare("esc")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Escape, 0});
		return;
	}
	
	if(!tmpl.compare("help")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Help, 0});
		return;
	}
	
	if(!tmpl.compare("home")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Home, 0});
		return;
	}
	
	if(!tmpl.compare("insert") || !tmpl.compare("ins")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Help, 0});
		return;
	}
	
	if(!tmpl.compare("numlock")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ANSI_KeypadClear, 0});
		return;
	}
	
//	if(!tmpl.compare("scroll")){
//		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){?, 0});
//		return;
//	}
	
	if(!tmpl.compare("pgdn")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_PageDown, 0});
		return;
	}
	
	if(!tmpl.compare("pgup")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_PageUp, 0});
		return;
	}
	
	if(!tmpl.compare("prtsc")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F13, 0});
		return;
	}
	
	if(!tmpl.compare("up")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_UpArrow, 0});
		return;
	}
	
	if(!tmpl.compare("down")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_DownArrow, 0});
		return;
	}
	
	if(!tmpl.compare("left")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_LeftArrow, 0});
		return;
	}
	
	if(!tmpl.compare("right")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_RightArrow, 0});
		return;
	}
	
	if(!tmpl.compare("f1")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F1, 0});
		return;
	}
	
	if(!tmpl.compare("f2")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F2, 0});
		return;
	}
	
	if(!tmpl.compare("f3")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F3, 0});
		return;
	}
	
	if(!tmpl.compare("f4")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F4, 0});
		return;
	}
	
	if(!tmpl.compare("f5")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F5, 0});
		return;
	}
	
	if(!tmpl.compare("f6")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F6, 0});
		return;
	}
	
	if(!tmpl.compare("f7")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F7, 0});
		return;
	}
	
	if(!tmpl.compare("f8")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F8, 0});
		return;
	}
	
	if(!tmpl.compare("f9")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F9, 0});
		return;
	}
	
	if(!tmpl.compare("f10")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F10, 0});
		return;
	}
	
	if(!tmpl.compare("f11")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F11, 0});
		return;
	}
	
	if(!tmpl.compare("f12")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F12, 0});
		return;
	}
	
	if(!tmpl.compare("f13")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F13, 0});
		return;
	}
	
	if(!tmpl.compare("f14")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F14, 0});
		return;
	}
	
	if(!tmpl.compare("f15")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F15, 0});
		return;
	}
	
	if(!tmpl.compare("f16")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_F16, 0});
		return;
	}
	
	if(!tmpl.compare("add") || !tmpl.compare("plus")){
		keys << AutoTypeAction(SendUnicodeAction, '+');
		return;
	}
	
	if(!tmpl.compare("subtract")){
		keys << AutoTypeAction(SendUnicodeAction, '-');
		return;
	}
	
	if(!tmpl.compare("multiply")){
		keys << AutoTypeAction(SendUnicodeAction, '*');
		return;
	}
	
	if(!tmpl.compare("divide")){
		keys << AutoTypeAction(SendUnicodeAction, '/');
		return;
	}
	
	if(!tmpl.compare("at")){
		keys << AutoTypeAction(SendUnicodeAction, '@');
		return;
	}
	
	if(!tmpl.compare("percent")){
		keys << AutoTypeAction(SendUnicodeAction, '%');
		return;
	}
	
	if(!tmpl.compare("caret")){
		keys << AutoTypeAction(SendUnicodeAction, '^');
		return;
	}
	
	if(!tmpl.compare("tilde")){
		keys << AutoTypeAction(SendUnicodeAction, '~');
		return;
	}
	
	if(!tmpl.compare("leftbrace")){
		keys << AutoTypeAction(SendUnicodeAction, '{');
		return;
	}
	
	if(!tmpl.compare("rightbrace")){
		keys << AutoTypeAction(SendUnicodeAction, '}');
		return;
	}
	
	if(!tmpl.compare("leftparen")){
		keys << AutoTypeAction(SendUnicodeAction, '(');
		return;
	}
	
	if(!tmpl.compare("rightparen")){
		keys << AutoTypeAction(SendUnicodeAction, ')');
		return;
	}

	if(!tmpl.compare("tab")){
		keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_Tab, 0});
		return;
	}

	if(tmpl.startsWith("unicode ") && tmpl.length() > 8){
		char* str = tmpl.right(tmpl.length() - 8).toUtf8().data();
		int n;
		uint16 unicode;
		while (1 == sscanf(str, " %hi%n", &unicode, &n)) {
			keys << AutoTypeAction(SendUnicodeAction, unicode);
			str += n;
		}
		return;
	}
	
    if(!tmpl.compare("clearfield")){
        if(('x11a' == appSignature) || ('????' == appSignature)){
            // ^A to beginning of line then ^K kill to end of line for X11 or Terminal.app
            keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ANSI_A, controlKey >> 8});
            keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ANSI_K, controlKey >> 8});
        } else {
            // Cmd-A to select all then delete for everything else
            keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ANSI_A, cmdKey >> 8});
            keys << AutoTypeAction(SendKeycodeAction, (KeycodeWithMods){kVK_ForwardDelete, 0});
        }
		return;
	}
	
	if(tmpl.startsWith("macsendkeycodes")){
		onlySendKeycodes = true;
	}
		  
	if(tmpl.startsWith("delay ") && tmpl.length()>6){
		bool ok;
		quint16 delay = tmpl.right(tmpl.length()-6).toInt(&ok);
		if (ok && delay>0 && delay<=10000)
			keys << AutoTypeAction(DelayAction, delay);
		return;
	}
}

void AutoTypeMacX::stringToKeysyms(const QString& string,QList<AutoTypeAction>& KeySymList){
	for(int i=0; i<string.length();i++)
		KeySymList << AutoTypeAction(SendUnicodeAction, string[i].unicode());
}

void AutoTypeMacX::keyDownUp(CGEventRef theEvent){
	// posting Key Down/Up events also annoyingly sets mouse location so must first get
	// current mouse location and set it in the event
	CGEventRef eventLocation = CGEventCreate(NULL);
	CGEventSetLocation(theEvent, CGEventGetLocation(eventLocation));
	CFRelease(eventLocation);

	CGEventSetType(theEvent, kCGEventKeyDown);
	CGEventPost(kCGHIDEventTap, theEvent);
	CGEventSetType(theEvent, kCGEventKeyUp);
	CGEventPost(kCGHIDEventTap, theEvent);
}
					
void AutoTypeMacX::sendKeycode(KeycodeWithMods keycodeWithMods){
	flushUnicode();
	uint keycode = keycodeWithMods.keycode;
	uint	mods = keycodeWithMods.mods << 8; 
	uint   flags = 0;
	if (0 != (  shiftKey & mods)) flags |= kCGEventFlagMaskShift;
	if (0 != (controlKey & mods)) flags |= kCGEventFlagMaskControl;
	if (0 != ( optionKey & mods)) flags |= kCGEventFlagMaskAlternate;
	if (0 != (	cmdKey & mods)) flags |= kCGEventFlagMaskCommand;
	CGEventSetIntegerValueField(keyEvent, kCGKeyboardEventKeycode, (int64_t)keycode);
	CGEventSetFlags(keyEvent, flags);
	keyDownUp(keyEvent);
	sleepKeyStrokeDelay();
}

void AutoTypeMacX::sendUnicode(KeySym keysym){
	if (onlySendKeycodes) {
		KeycodeWithMods keycodeWithMods = HelperMacX::keysymToKeycodeWithMods(keysym);
		if (NoKeycode == keycodeWithMods.keycode) return;
		sendKeycode(keycodeWithMods);
		return;
	}
	unicodeBuffer[unicodePtr++] = keysym;
	if (UNICODE_BUFFER_SIZE == unicodePtr) flushUnicode();
}

void AutoTypeMacX::flushUnicode(){
	if (0 == unicodePtr) return;
	CGEventKeyboardSetUnicodeString(unicodeEvent, unicodePtr, unicodeBuffer);
	keyDownUp(unicodeEvent);
	unicodePtr = 0;
	sleepKeyStrokeDelay();
}

void AutoTypeMacX::checkWindowType(){
	// sendUnicode does not work with X11 windows so revert to only send keycodes
	// this can be extended if other types of windows prove to fail for unicode
	onlySendKeycodes = 'x11a' == appSignature;
}
