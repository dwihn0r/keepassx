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

#include <QX11Info>
#include <X11/Xutil.h>
#include "mainwindow.h"
#include "HelperX11.h"
#include "AutoType.h"

#ifdef GLOBAL_AUTOTYPE
	#include "dialogs/AutoTypeDlg.h"
#endif

enum AutoTypeActionType{
	TypeKey, Delay
};

struct AutoTypeAction{
	AutoTypeAction(AutoTypeActionType t, quint16 d);
	AutoTypeActionType type;
	quint16 data;
};

class AutoTypePrivate{
	public:
		static void sleep(int msec);
		inline static void sleepKeyStrokeDelay(){ sleep(config->autoTypeKeyStrokeDelay()); };
		static void templateToKeysyms(const QString& Template, QList<AutoTypeAction>& KeySymList,IEntryHandle* entry);
		static void stringToKeysyms(const QString& string,QList<AutoTypeAction>& KeySymList);
};


// AutoType

KeepassMainWindow* AutoType::MainWin=NULL;
#ifdef GLOBAL_AUTOTYPE
Shortcut AutoType::shortcut;
#endif

void AutoType::perform(IEntryHandle* entry, QString& err,bool hideWindow,int nr){
	QString indexStr;
	if (nr==0)
		indexStr = "Auto-Type:";
	else
		indexStr = QString("Auto-Type-%1:").arg(nr);
	QString str;
	QString comment=entry->comment();
	int c=comment.count(indexStr, Qt::CaseInsensitive);
	if(c>1){
		err=QCoreApplication::translate("AutoType","More than one 'Auto-Type:' key sequence found.\nAllowed is only one per entry.");
		return;
	}
	else if(c==1){
		int start = comment.indexOf(indexStr,0,Qt::CaseInsensitive) + indexStr.length();
		int end = comment.indexOf("\n", start);
		if (end == -1)
			end = comment.length();
		
		str=comment.mid(start,end-start).trimmed();
		if (str.isEmpty())
			return;
	}
	else
		str="{USERNAME}{TAB}{PASSWORD}{ENTER}";
	
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
				err=QCoreApplication::translate("AutoType","Syntax Error in Auto-Type sequence near character %1\n\
						Found '{' without closing '}'").arg(i+10);
				return;
			}
			AutoTypePrivate::templateToKeysyms(tmpl.toLower(),Keys,entry);
			continue;
		}
		else{
			Keys << AutoTypeAction(TypeKey, HelperX11::getKeysym(str[i]));
		}
	}
	
	if (hideWindow)
		MainWin->hide();
	
	AutoTypePrivate::sleep(config->autoTypePreGap());
	
	Display* pDisplay = QX11Info::display();
	
	bool capsEnabled = HelperX11::keyboardModifiers(pDisplay)&LockMask;
	if (capsEnabled){
		XTestFakeKeyEvent(pDisplay,XKeysymToKeycode(pDisplay,XK_Caps_Lock),true,CurrentTime);
		XTestFakeKeyEvent(pDisplay,XKeysymToKeycode(pDisplay,XK_Caps_Lock),false,CurrentTime);
		AutoTypePrivate::sleepKeyStrokeDelay();
	}
	
	char keys_return[32];
	XQueryKeymap(pDisplay, keys_return);
	for (int i=0; i<32; i++)
		for (int j=0; j<8; j++)
			if ( keys_return[i] & (1<<j) ){
				XTestFakeKeyEvent(pDisplay,i*8+j,false,2);
				AutoTypePrivate::sleepKeyStrokeDelay();
			}
	
	for(int i=0;i<Keys.size();i++){
		if (Keys[i].type==TypeKey){
			int keycode=XKeysymToKeycode(pDisplay,Keys[i].data);
			if (keycode==0){
				err = QCoreApplication::translate("AutoType","Auto-Type string contains illegal characters");
				break;
			}
			int mods=HelperX11::getModifiers(pDisplay,Keys[i].data,keycode);
			HelperX11::pressModifiers(pDisplay,mods);
			AutoTypePrivate::sleepKeyStrokeDelay();
			XTestFakeKeyEvent(pDisplay,keycode,True,0);
			AutoTypePrivate::sleepKeyStrokeDelay();
			XTestFakeKeyEvent(pDisplay,keycode,False,1);
			AutoTypePrivate::sleepKeyStrokeDelay();
			HelperX11::releaseModifiers(pDisplay,mods);
			AutoTypePrivate::sleepKeyStrokeDelay();
		}
		else if (Keys[i].type==Delay){
			QCoreApplication::processEvents();
			AutoTypePrivate::sleep(Keys[i].data);
		}
	}
	
	if (capsEnabled){
		XTestFakeKeyEvent(pDisplay,XKeysymToKeycode(pDisplay,XK_Caps_Lock),true,CurrentTime);
		XTestFakeKeyEvent(pDisplay,XKeysymToKeycode(pDisplay,XK_Caps_Lock),false,CurrentTime);
	}
	
	if (hideWindow){
		MainWin->showMinimized();
		XIconifyWindow(pDisplay, MainWin->winId(), MainWin->x11Info().screen()); // workaround for Gnome
	}
}

#ifdef GLOBAL_AUTOTYPE
void AutoType::performGlobal(){
	if (MainWin->db==NULL) return;
	
	Display* d = QX11Info::display();
	Window w;
	int revert_to_return;
	XGetInputFocus(d, &w, &revert_to_return);
	char** list;
	int tree;
	do {
		XTextProperty textProp;
		XGetWMName(d, w, &textProp);
		int count;
		Xutf8TextPropertyToTextList(d, &textProp, &list, &count);
		if (list) break;
		Window root = 0;
		Window parent = 0;
		Window* children = NULL;
		unsigned int num_children;
		tree = XQueryTree(d, w, &root, &parent, &children, &num_children);
		w=parent;
		if (children) XFree(children);
	} while (tree && w);
	if (!list) return;
	QString title = QString::fromUtf8(list[0]).toLower();
	XFreeStringList(list);
	
	QList<IEntryHandle*> validEntries;
	QList<int> entryNumbers;
	QList<IEntryHandle*> entries = MainWin->db->entries();
	QRegExp lineMatch("Auto-Type-Window(?:-(\\d+)|):([^\\n]+)", Qt::CaseInsensitive, QRegExp::RegExp2);
	QDateTime now = QDateTime::currentDateTime();
	for (int i=0; i<entries.size(); i++){
		if (entries[i]->expire()!=Date_Never && entries[i]->expire()<now)
			continue;
		
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
		QString err;
		perform(validEntries[0],err,false,entryNumbers[0]);
	}
	else if (validEntries.size()>1){
		AutoTypeDlg* dlg = new AutoTypeDlg(validEntries, entryNumbers);
		dlg->show();
	}
}

bool AutoType::registerGlobalShortcut(const Shortcut& s){
	if (s.key==shortcut.key && s.ctrl==shortcut.ctrl && s.shift==shortcut.shift && s.alt==shortcut.alt && s.altgr==shortcut.altgr && s.win==shortcut.win)
		return true;
	
	Display* display = QX11Info::display();
	Window root = XDefaultRootWindow(display);
	int code=XKeysymToKeycode(display, HelperX11::getKeysym(s.key));
	int mod=HelperX11::getShortcutModifierMask(s);
	
	HelperX11::startCatchErrors();
	XGrabKey(display, code, mod, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(display, code, mod | Mod2Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(display, code, mod | LockMask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(display, code, mod | Mod2Mask | LockMask, root, True, GrabModeAsync, GrabModeAsync);
	HelperX11::stopCatchErrors();
	
	if (HelperX11::errorOccurred()){
		XUngrabKey(display, code, mod, root);
		XUngrabKey(display, code, mod | Mod2Mask, root);
		XUngrabKey(display, code, mod | LockMask, root);
		XUngrabKey(display, code, mod | Mod2Mask | LockMask, root);
		return false;
	}
	else {
		unregisterGlobalShortcut();
		shortcut = s;
		return true;
	}
}

void AutoType::unregisterGlobalShortcut(){
	if (shortcut.key==0) return;
	
	Display* display = QX11Info::display();
	Window root = XDefaultRootWindow(display);
	int code=XKeysymToKeycode(display, HelperX11::getKeysym(shortcut.key));
	int mod=HelperX11::getShortcutModifierMask(shortcut);
	
	XUngrabKey(display, code, mod, root);
	XUngrabKey(display, code, mod | Mod2Mask, root);
	XUngrabKey(display, code, mod | LockMask, root);
	XUngrabKey(display, code, mod | Mod2Mask | LockMask, root);
	
	shortcut.key = 0;
}

#endif // GLOBAL_AUTOTYPE

AutoTypeAction::AutoTypeAction(AutoTypeActionType t, quint16 d) : type(t), data(d){
}


// AutoTypePrivate

void AutoTypePrivate::sleep(int msec){
	if (msec==0) return;
	timespec timeOut, remains;
	timeOut.tv_sec = msec/1000;
	timeOut.tv_nsec = (msec%1000)*1000000;
	nanosleep(&timeOut, &remains);
}

void AutoTypePrivate::templateToKeysyms(const QString& tmpl, QList<AutoTypeAction>& keys,IEntryHandle* entry){
	//tmpl must be lower case!!!
	if(!tmpl.compare("title")){
		stringToKeysyms(entry->title(),keys);
		return;}
	if(!tmpl.compare("username")){
		stringToKeysyms(entry->username(),keys);
		return;}
	if(!tmpl.compare("url")){
		stringToKeysyms(entry->url(),keys);
		return;}
	if(!tmpl.compare("password")){
		SecString password=entry->password();
		password.unlock();
		stringToKeysyms(password,keys);
		return;
	}
	if(!tmpl.compare("space")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym(' '));
		return;}
	
	if(!tmpl.compare("backspace") || !tmpl.compare("bs") || !tmpl.compare("bksp")){
		keys << AutoTypeAction(TypeKey, XK_BackSpace);
		return;}
	
	if(!tmpl.compare("break")){
		keys << AutoTypeAction(TypeKey, XK_Break);
		return;}
	
	if(!tmpl.compare("capslock")){
		keys << AutoTypeAction(TypeKey, XK_Caps_Lock);
		return;}
	
	if(!tmpl.compare("del") || !tmpl.compare("delete")){
		keys << AutoTypeAction(TypeKey, XK_Delete);
		return;}
	
	if(!tmpl.compare("end")){
		keys << AutoTypeAction(TypeKey, XK_End);
		return;}
	
	if(!tmpl.compare("enter")){
		keys << AutoTypeAction(TypeKey, XK_Return);
		return;}
	
	if(!tmpl.compare("esc")){
		keys << AutoTypeAction(TypeKey, XK_Escape);
		return;}
	
	if(!tmpl.compare("help")){
		keys << AutoTypeAction(TypeKey, XK_Help);
		return;}
	
	if(!tmpl.compare("home")){
		keys << AutoTypeAction(TypeKey, XK_Home);
		return;}
	
	if(!tmpl.compare("insert") || !tmpl.compare("ins")){
		keys << AutoTypeAction(TypeKey, XK_Insert);
		return;}
	
	if(!tmpl.compare("numlock")){
		keys << AutoTypeAction(TypeKey, XK_Num_Lock);
		return;}
	
	if(!tmpl.compare("scroll")){
		keys << AutoTypeAction(TypeKey, XK_Scroll_Lock);
		return;}
	
	if(!tmpl.compare("pgdn")){
		keys << AutoTypeAction(TypeKey, XK_Page_Down);
		return;}
	
	if(!tmpl.compare("pgup")){
		keys << AutoTypeAction(TypeKey, XK_Page_Up);
		return;}
	
	if(!tmpl.compare("prtsc")){
		keys << AutoTypeAction(TypeKey, XK_3270_PrintScreen);
		return;}
	
	if(!tmpl.compare("up")){
		keys << AutoTypeAction(TypeKey, XK_Up);
		return;}
	
	if(!tmpl.compare("down")){
		keys << AutoTypeAction(TypeKey, XK_Down);
		return;}
	
	if(!tmpl.compare("left")){
		keys << AutoTypeAction(TypeKey, XK_Left);
		return;}
	
	if(!tmpl.compare("right")){
		keys << AutoTypeAction(TypeKey, XK_Right);
		return;}
	
	if(!tmpl.compare("f1")){
		keys << AutoTypeAction(TypeKey, XK_F1);
		return;}
	
	if(!tmpl.compare("f2")){
		keys << AutoTypeAction(TypeKey, XK_F2);
		return;}
	
	if(!tmpl.compare("f3")){
		keys << AutoTypeAction(TypeKey, XK_F3);
		return;}
	
	if(!tmpl.compare("f4")){
		keys << AutoTypeAction(TypeKey, XK_F4);
		return;}
	
	if(!tmpl.compare("f5")){
		keys << AutoTypeAction(TypeKey, XK_F5);
		return;}
	
	if(!tmpl.compare("f6")){
		keys << AutoTypeAction(TypeKey, XK_F6);
		return;}
	
	if(!tmpl.compare("f7")){
		keys << AutoTypeAction(TypeKey, XK_F7);
		return;}
	
	if(!tmpl.compare("f8")){
		keys << AutoTypeAction(TypeKey, XK_F8);
		return;}
	
	if(!tmpl.compare("f9")){
		keys << AutoTypeAction(TypeKey, XK_F9);
		return;}
	
	if(!tmpl.compare("f10")){
		keys << AutoTypeAction(TypeKey, XK_F10);
		return;}
	
	if(!tmpl.compare("f11")){
		keys << AutoTypeAction(TypeKey, XK_F11);
		return;}
	
	if(!tmpl.compare("f12")){
		keys << AutoTypeAction(TypeKey, XK_F12);
		return;}
	
	if(!tmpl.compare("f13")){
		keys << AutoTypeAction(TypeKey, XK_F13);
		return;}
	
	if(!tmpl.compare("f14")){
		keys << AutoTypeAction(TypeKey, XK_F14);
		return;}
	
	if(!tmpl.compare("f15")){
		keys << AutoTypeAction(TypeKey, XK_F15);
		return;}
	
	if(!tmpl.compare("f16")){
		keys << AutoTypeAction(TypeKey, XK_F16);
		return;}
	
	if(!tmpl.compare("add") || !tmpl.compare("plus")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('+'));
		return;}
	
	if(!tmpl.compare("subtract")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('-'));
		return;}
	
	if(!tmpl.compare("multiply")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('+'));
		return;}
	
	if(!tmpl.compare("divide")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('/'));
		return;}
	
	if(!tmpl.compare("at")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('@'));
		return;}
	
	if(!tmpl.compare("percent")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('%'));
		return;}
	
	if(!tmpl.compare("caret")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('^'));
		return;}
	
	if(!tmpl.compare("tilde")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('~'));
		return;}
	
	if(!tmpl.compare("leftbrace")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('{'));
		return;}
	
	if(!tmpl.compare("rightbrace")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('}'));
		return;}
	
	if(!tmpl.compare("leftparen")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym('('));
		return;}
	
	if(!tmpl.compare("rightparen")){
		keys << AutoTypeAction(TypeKey, HelperX11::getKeysym(')'));
		return;}
	
	if(!tmpl.compare("winl")){
		keys << AutoTypeAction(TypeKey, XK_Super_L);
		return;}
	
	if(!tmpl.compare("winr")){
		keys << AutoTypeAction(TypeKey, XK_Super_R);
		return;}
	
	if(!tmpl.compare("win")){
		keys << AutoTypeAction(TypeKey, XK_Super_L);
		return;}
	
	if(!tmpl.compare("tab")){
		keys << AutoTypeAction(TypeKey, XK_Tab);
		return;}
	
	if(tmpl.startsWith("delay ") && tmpl.length()>6){
		bool ok;
		quint16 delay = tmpl.right(tmpl.length()-6).toInt(&ok);
		if (ok && delay>0 && delay<=10000)
			keys << AutoTypeAction(Delay, delay);
	}
}

void AutoTypePrivate::stringToKeysyms(const QString& string,QList<AutoTypeAction>& KeySymList){
	for(int i=0; i<string.length();i++)
		KeySymList << AutoTypeAction(TypeKey, HelperX11::getKeysym(string[i]));
}
