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

#include "lib/AutoTypeGlobalMacX.h"
#include "mainwindow.h"
#include "lib/HelperMacX.h"
#include "dialogs/AutoTypeDlg.h"

AutoTypeGlobal* autoType = NULL;

static bool inHotKeyEvent = false;

static OSStatus hotKeyHandler(EventHandlerCallRef nextHandler, EventRef event, void *userData);

void initAutoType(KeepassMainWindow* mainWin){
	autoType = new AutoTypeGlobalMacX(mainWin);
}

AutoTypeGlobalMacX::AutoTypeGlobalMacX(KeepassMainWindow* mainWin) : AutoTypeMacX(mainWin){
	shortcut.key = 0;
	oldCode = 0;
	oldMod = 0;
	inGlobalAutoType = false;

	// initialize hot key handling
	hotKeyRef = NULL;
	hotKeyID.signature = 'kpsx';
	hotKeyID.id = 1;
	EventTypeSpec eventType;
	eventType.eventClass = kEventClassKeyboard;
	eventType.eventKind = kEventHotKeyPressed;
	InstallApplicationEventHandler(&hotKeyHandler, 1, &eventType, this, NULL);
}

OSStatus hotKeyHandler(EventHandlerCallRef, EventRef, void *userData){
	// ignore nextHandler - should not be called
	if ((inHotKeyEvent) || HelperMacX::isFrontProcess(HelperMacX::getKeepassxPID())) return noErr;
	inHotKeyEvent = true;
	((AutoTypeGlobalMacX*)userData)->performGlobal();
	inHotKeyEvent = false;
	return noErr;
}

void AutoTypeGlobalMacX::cancelled(){
	pid_t pid;
	if (HelperMacX::getTargetWindowInfo(&pid, NULL, NULL, 0))
		HelperMacX::processToFront(pid);
	else HelperMacX::processToFront(targetPID);
	targetPID = 0;
}

void AutoTypeGlobalMacX::updateKeymap(){
	AutoTypeMacX::updateKeymap();
	registerGlobalShortcut(shortcut);
}

void AutoTypeGlobalMacX::perform(IEntryHandle* entry, bool hideWindow, int nr, bool wasLocked){
	if (inGlobalAutoType)
		return;
	inGlobalAutoType = true;
	AutoTypeMacX::perform(entry, hideWindow, nr, wasLocked);
	inGlobalAutoType = false;
}

QStringList AutoTypeGlobalMacX::getAllWindowTitles(){
	QStringList titleList;
	char windowName[256];
	pid_t keepassxPID = HelperMacX::getKeepassxPID();
	CFArrayRef windowInfo = CGWindowListCopyWindowInfo(
		kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID);
	CFIndex windowCount = CFArrayGetCount(windowInfo);
	for (CFIndex i = 0; i < windowCount; i++){
		CFDictionaryRef window = (CFDictionaryRef)CFArrayGetValueAtIndex(windowInfo, i);
		// only want windows in layer 0
		CFNumberRef windowLayerRef = (CFNumberRef)CFDictionaryGetValue(window, kCGWindowLayer);
		int windowLayer = -1;
		CFNumberGetValue(windowLayerRef, kCFNumberIntType, &windowLayer);
		if (0 != windowLayer) continue;
		// get the pid owning this window
		CFNumberRef pidRef = (CFNumberRef)CFDictionaryGetValue(window, kCGWindowOwnerPID);
		pid_t pid = -1;
		CFNumberGetValue(pidRef, kCFNumberIntType, &pid);
		// skip KeePassX windows
		if (keepassxPID == pid) continue;
		// get window name; continue if no name
		CFStringRef windowNameRef = (CFStringRef)CFDictionaryGetValue(window, kCGWindowName);
		if (!windowNameRef) continue;
		windowName[0] = 0;
		if (!CFStringGetCString(windowNameRef, windowName, sizeof(windowName), kCFStringEncodingUTF8) ||
			  (0 == windowName[0]))
			continue;
		titleList.append(QString::fromUtf8(windowName));
	}
	CFRelease(windowInfo);
	return titleList;
}

void AutoTypeGlobalMacX::performGlobal(){
	if (AutoTypeDlg::isDialogVisible()) {
		qWarning("Already performing auto-type, ignoring this one");
		return;
	}
	char titleUtf8[256];
	titleUtf8[0] = 0;
	if (!HelperMacX::getTargetWindowInfo(&targetPID, &windowNumber, titleUtf8, sizeof(titleUtf8))
		  || (0 == titleUtf8[0])) {
		targetPID = 0;
		return;
	}

	bool wasLocked = mainWin->isLocked();
	if (wasLocked)
		mainWin->OnUnLockWorkspace();
	
	if (!mainWin->isOpened()) {
		targetPID = 0;
		return;
	}
	QString title = QString::fromUtf8(titleUtf8).toLower();

	QList<IEntryHandle*> validEntries;
	QList<int> entryNumbers;
	QList<IEntryHandle*> entries = mainWin->db->entries();
	QRegExp lineMatch("Auto-Type-Window(?:-(\\d+)|):([^\\n]+)", Qt::CaseInsensitive, QRegExp::RegExp2);
	QDateTime now = QDateTime::currentDateTime();
	for (int i=0; i<entries.size(); i++){
		if ( (entries[i]->expire()!=Date_Never && entries[i]->expire()<now) ||
			 (getRootGroupName(entries[i]).compare("backup",Qt::CaseInsensitive)==0)
		){
			continue;
		}
		
		bool hasWindowEntry=false;
		QString comment = entries[i]->comment();
		int offset = 0;
		while ( (offset=lineMatch.indexIn(comment, offset))!=-1 ){
			QStringList captured = lineMatch.capturedTexts();
			offset += captured[0].length();
			int nr;
			QString entryWindow;
			bool valid;
			if (captured.size()==2){
				nr = 0;
				entryWindow = captured[1].trimmed().toLower();
			}
			else{
				nr = captured[1].toInt();
				entryWindow = captured[2].trimmed().toLower();
			}
			if (entryWindow.length()==0) continue;
			
			hasWindowEntry = true;
			bool wildStart = (entryWindow[0]=='*');
			bool wildEnd = (entryWindow[entryWindow.size()-1]=='*');
			if (wildStart&&wildEnd){
				entryWindow.remove(0,1);
				if (entryWindow.length()!=0){
					entryWindow.remove(entryWindow.size()-1,1);
					valid = title.contains(entryWindow);
				}
				else
					valid = true;
			}
			else if (wildStart){
				entryWindow.remove(0,1);
				valid = title.endsWith(entryWindow);
			}
			else if (wildEnd){
				entryWindow.remove(entryWindow.size()-1,1);
				valid = title.startsWith(entryWindow);
			}
			else {
				valid = (title==entryWindow);
			}
			
			if (valid){
				validEntries << entries[i];
				entryNumbers << nr;
				break;
			}
		}
		
		if (!hasWindowEntry && config->entryTitlesMatch()){
			QString entryTitle = entries[i]->title().toLower();
			if (!entryTitle.isEmpty() && title.contains(entryTitle)){
				validEntries << entries[i];
				entryNumbers << 0;
			}
		}
	}
	
	if (validEntries.size()==1){
		perform(validEntries[0],wasLocked,entryNumbers[0],wasLocked);
	}
	else if (validEntries.size()>1){
		AutoTypeDlg* dlg = new AutoTypeDlg(validEntries, entryNumbers, wasLocked);
		HelperMacX::processToFront(HelperMacX::getKeepassxPID());
		dlg->show();
	}
}

bool AutoTypeGlobalMacX::registerGlobalShortcut(const Shortcut& s){

	if (s.key == 0)
		return false;

	int code=HelperMacX::keysymToKeycode(s.key);
	uint mod=HelperMacX::getShortcutModifierMask(s);
	
	if (s.key==shortcut.key && s.ctrl==shortcut.ctrl && s.shift==shortcut.shift && s.alt==shortcut.alt
		  && s.altgr==shortcut.altgr && s.win==shortcut.win && code==oldCode && mod==oldMod)
		return true;

	// need to unregister old before registering new
	unregisterGlobalShortcut();
	OSStatus status = RegisterEventHotKey(code, mod, hotKeyID, GetApplicationEventTarget(), 0, &hotKeyRef);
	if (noErr == status) {
		shortcut	= s;
		oldCode	 = code;
		oldMod	  = mod;
		return true;
	} else {
		qWarning("Error registering global shortcut: %d", (int)status);
		RegisterEventHotKey(oldCode, oldMod, hotKeyID, GetApplicationEventTarget(), 0, &hotKeyRef);
		return false;
	}
}

void AutoTypeGlobalMacX::unregisterGlobalShortcut(){
	if (shortcut.key==0) return;
	
	if (NULL != hotKeyRef) {
		UnregisterEventHotKey(hotKeyRef);
		hotKeyRef = NULL;
	}
	
	shortcut.key = 0;
	oldCode = 0;
	oldMod = 0;
}

QString AutoTypeGlobalMacX::getRootGroupName(IEntryHandle* entry){
	IGroupHandle* group = entry->group();
	int level = group->level();
	for (int i=0; i<level; i++)
		group = group->parent();
	
	return group->title();
}
