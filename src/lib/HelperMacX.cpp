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

#include "lib/HelperMacX.h"
#include <map>

static pid_t keepassxPID;

#ifdef GLOBAL_AUTOTYPE
#include "lib/AutoTypeGlobalMacX.h"

uint HelperMacX::getShortcutModifierMask(const Shortcut& s){
	AutoTypeGlobalMacX* autoTypeGlobal = static_cast<AutoTypeGlobalMacX*>(autoType);

	uint mod = 0;
	if (s.ctrl)  mod |= autoTypeGlobal->maskCtrl();
	if (s.shift) mod |= autoTypeGlobal->maskShift();
	if (s.alt)   mod |= autoTypeGlobal->maskAlt();
	if (s.altgr) mod |= autoTypeGlobal->maskAltGr();
	if (s.win)   mod |= autoTypeGlobal->maskMeta();
	
	return mod;
}

Boolean HelperMacX::isFrontProcess(pid_t pid){
	Boolean result;
	ProcessSerialNumber pidPSN;
	ProcessSerialNumber frontPSN;
	OSStatus status = GetProcessForPID(pid, &pidPSN);
	if (noErr != status) {
		qWarning("HelperMacX::isFrontProcess: GetProcessForPID error for pid %d: %d", pid, (int)status);
		return false;
	}
	GetFrontProcess(&frontPSN);
	SameProcess(&pidPSN, &frontPSN, &result);
	return result;
}
#endif

pid_t HelperMacX::getKeepassxPID(){
	if (0 == keepassxPID) {
		ProcessSerialNumber processSerialNumber;
		GetCurrentProcess(&processSerialNumber);
		GetProcessPID(&processSerialNumber, &keepassxPID);
	}
	return keepassxPID;
}

Boolean HelperMacX::getTargetWindowInfo(pid_t *pidPtr, int *windowNumberPtr,
										char* windowName, int maxWindowNameSize){
	char windowNameBuffer[256];
	if (NULL == windowName) {
		windowName = windowNameBuffer;
		maxWindowNameSize = sizeof(windowNameBuffer);
	}
	// get info for on screen windows (excluding desktop elements) in top to bottom order
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
		if (getKeepassxPID() == pid) continue;
		// get window name; continue if no name
		CFStringRef windowNameRef = (CFStringRef)CFDictionaryGetValue(window, kCGWindowName);
		if (!windowNameRef) continue;
		windowName[0] = 0;
		if (!CFStringGetCString(windowNameRef, windowName, maxWindowNameSize, kCFStringEncodingUTF8) ||
			  (0 == windowName[0]))
			continue;
		if (NULL != pidPtr) *pidPtr = pid;
		if (NULL != windowNumberPtr) {
			CFNumberRef windowNumberRef = (CFNumberRef)CFDictionaryGetValue(window, kCGWindowNumber);
			CFNumberGetValue(windowNumberRef, kCGWindowIDCFNumberType, windowNumberPtr);
		}
		CFRelease(windowInfo);
		return true;
	}
	CFRelease(windowInfo);
	return false;
}

OSType HelperMacX::getProcessSignature(pid_t pid){
	OSErr err;
	ProcessSerialNumber processSerialNumber;
	ProcessInfoRec processInfoRec;
	processInfoRec.processInfoLength = sizeof(processInfoRec);
	processInfoRec.processAppSpec = NULL;
	processInfoRec.processName = NULL;
	err = GetProcessForPID(pid, &processSerialNumber);
	if (noErr != err) {
		qWarning("HelperMacX::getProcessSignature: GetProcessForPID error for pid %d: %d", pid, err);
		return 0;
	}
	err = GetProcessInformation(&processSerialNumber, &processInfoRec);
	if (noErr != err) {
		qWarning("HelperMacX::getProcessSignature: GetProcessInformation error for pid %d: %d\n", pid, err);
		return 0;
	}
	return processInfoRec.processSignature;
}

static uint orderedModifiers[] = {
	0,
	(  shiftKey                                   ) >> 8,
	(controlKey                                   ) >> 8,
	( optionKey                                   ) >> 8,
	( cmdKey                                      ) >> 8,
	(  shiftKey | controlKey                      ) >> 8,
	(  shiftKey | optionKey                       ) >> 8,
	(  shiftKey | cmdKey                          ) >> 8,
	(controlKey | optionKey                       ) >> 8,
	(controlKey | cmdKey                          ) >> 8,
	( optionKey | cmdKey                          ) >> 8,
	(  shiftKey | controlKey | optionKey          ) >> 8,
	(  shiftKey | controlKey | cmdKey             ) >> 8,
	(  shiftKey | optionKey  | cmdKey             ) >> 8,
	(controlKey | optionKey  | cmdKey             ) >> 8,
	(  shiftKey | controlKey | optionKey | cmdKey ) >> 8
};

static std::map<uint,KeycodeWithMods> unicodeToKeycodeWithModsMap;

void HelperMacX::initUnicodeToKeycodeWithModsMap(){
	unicodeToKeycodeWithModsMap.clear();
	TISInputSourceRef inputSourceRef = TISCopyCurrentKeyboardInputSource();
	if (NULL == inputSourceRef) {
		qWarning("HelperMacX::initUnicodeToKeycodeWithModsMap: inputSourceRef is NULL");
		return;
	}
	CFDataRef unicodeKeyLayoutDataRef = (CFDataRef)TISGetInputSourceProperty(inputSourceRef,
		kTISPropertyUnicodeKeyLayoutData);
	if (NULL == unicodeKeyLayoutDataRef) {
		qWarning("HelperMacX::initUnicodeToKeycodeWithModsMap: unicodeKeyLayoutDataRef is NULL");
		return;
	}
	UCKeyboardLayout *unicodeKeyLayoutDataPtr = (UCKeyboardLayout*)CFDataGetBytePtr(unicodeKeyLayoutDataRef);

	UInt32 deadKeyState;
	UniChar unicodeString[8];
	UniCharCount len;
	for (int m = 0; m < 16; m++) {
		uint mods =  orderedModifiers[m];
		for (uint keycode = 0; keycode < 0x80; keycode++) {
			deadKeyState = 0;
			len = 0;
			OSStatus status = UCKeyTranslate(unicodeKeyLayoutDataPtr, keycode, kUCKeyActionDown,
											 mods, LMGetKbdType(), kUCKeyTranslateNoDeadKeysMask,
											 &deadKeyState, sizeof(unicodeString), &len, unicodeString);
			if (noErr != status) {
				qWarning("HelperMacX::initUnicodeToKeycodeWithModsMap: UCKeyTranslate error: %d keycode 0x%02X modifiers 0x%02X",
						 (int)status, keycode, mods);
				continue;
			}
			// store if only one char and not already in store
			if ((1 != len) || (0 < unicodeToKeycodeWithModsMap.count(unicodeString[0]))) continue;
			unicodeToKeycodeWithModsMap[unicodeString[0]] = (KeycodeWithMods){ keycode, mods };
		}
	}
}

KeySym HelperMacX::keycodeToKeysym(uint keycode){
	TISInputSourceRef inputSourceRef = TISCopyCurrentKeyboardInputSource();
	if (NULL == inputSourceRef) {
		qWarning("HelperMacX::keycodeToKeysym: inputSourceRef is NULL");
		return NoSymbol;
	}
	CFDataRef unicodeKeyLayoutDataRef = (CFDataRef)TISGetInputSourceProperty(inputSourceRef,
		kTISPropertyUnicodeKeyLayoutData);
	if (NULL == unicodeKeyLayoutDataRef) {
		CFRelease(inputSourceRef);
		qWarning("HelperMacX::keycodeToKeysym: unicodeKeyLayoutDataRef is NULL");
		return NoSymbol;
	}
	UCKeyboardLayout *unicodeKeyLayoutDataPtr = (UCKeyboardLayout*)CFDataGetBytePtr(unicodeKeyLayoutDataRef);
	UInt32 deadKeyState = 0;
	UniChar unicodeString[8];
	UniCharCount len = 0;
	OSStatus status = UCKeyTranslate(unicodeKeyLayoutDataPtr, keycode, kUCKeyActionDown,
									 0, LMGetKbdType(), kUCKeyTranslateNoDeadKeysMask,
									 &deadKeyState, sizeof(unicodeString), &len, unicodeString);
	CFRelease(inputSourceRef);
	if (noErr != status) {
		qWarning("HelperMacX::keycodeToKeysym: UCKeyTranslate error: %d", (int)status);
		return NoSymbol;
	}
	return 1 != len ? NoSymbol : unicodeString[0];
}

uint HelperMacX::keysymToKeycode(KeySym keysym){
	return keysymToKeycodeWithMods(keysym).keycode;
}

static const KeycodeWithMods NoKeycodeWithMods = (KeycodeWithMods){ NoKeycode, 0 };

KeycodeWithMods HelperMacX::keysymToKeycodeWithMods(KeySym keysym){
	return 0 == unicodeToKeycodeWithModsMap.count(keysym)
			? NoKeycodeWithMods : unicodeToKeycodeWithModsMap[keysym];
}

void HelperMacX::processToFront(pid_t pid){
	OSStatus status;
	ProcessSerialNumber processSerialNumber;
	status = GetProcessForPID(pid, &processSerialNumber);
	if (noErr != status) {
		qWarning("HelperMacX::processToFront: GetProcessForPID error for pid %d: %d", pid, (int)status);
		return;
	}
	status = SetFrontProcessWithOptions(&processSerialNumber, kSetFrontProcessFrontWindowOnly);
	if (noErr != status) {
		qWarning("HelperMacX::processToFront: SetFrontProcessWithOptions for pid %d: %d", pid, (int)status);
		return;
	}
}
