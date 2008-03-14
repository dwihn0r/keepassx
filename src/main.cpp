/***************************************************************************
 *   Copyright (C) 1992-2007 Trolltech ASA								   *
 *                                                                         *
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
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

#include "plugins/interfaces/IFileDialog.h"
#include "plugins/interfaces/IKdeInit.h"
#include "plugins/interfaces/IGnomeInit.h"


#include <QTranslator>
#include <QLibraryInfo>
#include <QPluginLoader>
#include "mainwindow.h"
#include "main.h"
#if defined(Q_WS_X11) && defined(GLOBAL_AUTOTYPE)
	#include "Application_X11.h"
#endif

using namespace std;

KpxConfig *config;
QString  AppDir;
QString HomeDir;
QString DataDir;
QString PluginLoadError;
bool TrActive;
QString DetailViewTemplate;
bool EventOccurred;
bool EventOccurredBlock = false;
EventListener* eventListener;

QPixmap* EntryIcons;
IIconTheme* IconLoader=NULL;


int main(int argc, char **argv)
{
	QApplication* app=NULL;
	initAppPaths(argc,argv);
	CmdLineArgs args;
	if(!args.preparse(argc,argv)){ // searches only for the -cfg parameter
		qCritical(CSTR(args.error()));
		return 1;
	}

	//Load Config
	QString IniFilename;
	if(args.configLocation().isEmpty()){
		if(!QDir(HomeDir).exists()){
			QDir conf(QDir::homePath());
			if(!QDir().mkpath(HomeDir))
				qWarning("Warning: Could not create directory '%s'", CSTR(HomeDir));
		}
		IniFilename=HomeDir+"/config";
	}
	else
		IniFilename=args.configLocation();

	config = new KpxConfig(IniFilename);
	fileDlgHistory.load();
	
	// PlugIns

	if(config->integrPlugin()!=KpxConfig::NoIntegr){
		QString LibName="libkeepassx-";
		if(config->integrPlugin()==KpxConfig::KDE)
			LibName+="kde.so";
		else if(config->integrPlugin()==KpxConfig::Gnome)
			LibName+="gnome.so";
		QString filename=findPlugin(LibName);
		if(filename!=QString()){
			QPluginLoader plugin(filename);
			if(!plugin.load()){
				PluginLoadError=plugin.errorString();
				qWarning("Could not load desktop integration plugin:");
				qWarning(CSTR(PluginLoadError));
			}
			else{
				QObject *plugininstance=plugin.instance();
				IFileDialog* fdlg=qobject_cast<IFileDialog*>(plugininstance);
				IconLoader=qobject_cast<IIconTheme*>(plugininstance);
				if(IconLoader==NULL){
					qWarning("Error: Integration Plugin: Could not initialize IconTheme interface.");
				}
				KpxFileDialogs::setPlugin(fdlg);
				if(config->integrPlugin()==KpxConfig::KDE){
					IKdeInit* kdeinit=qobject_cast<IKdeInit*>(plugin.instance());
					app=kdeinit->getMainAppObject(argc,argv);
					if(!app)PluginLoadError=QObject::tr("Initialization failed.");
				}
				if(config->integrPlugin()==KpxConfig::Gnome){
					IGnomeInit* ginit=qobject_cast<IGnomeInit*>(plugin.instance());
					if(!ginit->init(argc,argv)){
						KpxFileDialogs::setPlugin(NULL);
						qWarning("GtkIntegrPlugin: Gtk init failed.");
						PluginLoadError=QObject::tr("Initialization failed.");
					}
				}
			}
		}
		else{
			qWarning(CSTR(QString("Could not load desktop integration plugin: File '%1' not found.").arg(LibName)));
			PluginLoadError=QObject::tr("Could not locate library file.");
		}
	}
	if(!app){
		#if defined(Q_WS_X11) && defined(GLOBAL_AUTOTYPE)
			app = new KeepassApplication(argc,argv);
		#else
			app = new QApplication(argc,argv);
		#endif	
	}
	args.parse(QApplication::arguments());
	
	
	//Internationalization
	QLocale loc;
	if(!args.language().size())
		loc=QLocale::system();
	else
		loc=QLocale(args.language());

	QTranslator* translator = NULL;
	QTranslator* qtTranslator=NULL;
	translator=new QTranslator;
	qtTranslator=new QTranslator;

	if(loadTranslation(translator,"keepass-",loc.name(),QStringList()
						<< DataDir+"/i18n/"
						<< HomeDir))
	{
		app->installTranslator(translator);
		TrActive=true;
	}
	else{
		if(loc.name()!="en_US")
			qWarning(QString("Kpx: No Translation found for '%1 (%2)' using 'English (UnitedStates)'")
				.arg(QLocale::languageToString(loc.language()))
				.arg(QLocale::countryToString(loc.country())).toAscii());
		delete translator;
		TrActive=false;
	}
	
	if(TrActive){
		if(loadTranslation(qtTranslator,"qt_",loc.name(),QStringList()
							<< QLibraryInfo::location(QLibraryInfo::TranslationsPath)
							<< DataDir+"/i18n/"
							<< HomeDir))
			app->installTranslator(qtTranslator);
		else{
			if(loc.name()!="en_US")
				qWarning(QString("Qt: No Translation found for '%1 (%2)' using 'English (UnitedStates)'")
					.arg(QLocale::languageToString(loc.language()))
					.arg(QLocale::countryToString(loc.country())).toAscii());
			delete qtTranslator;
		}
	}

	DetailViewTemplate=config->detailViewTemplate();

	loadImages();
	KpxBookmarks::load();
	initYarrow(); //init random number generator
	SecString::generateSessionKey();

	eventListener = new EventListener();
	app->installEventFilter(eventListener);

	QApplication::setQuitOnLastWindowClosed(false);
	KeepassMainWindow *mainWin = new KeepassMainWindow(args.file(), args.startMinimized(), args.startLocked());
	int r=app->exec();
	delete mainWin;
	delete eventListener;

	fileDlgHistory.save();
	delete app;
	delete config;
	return r;
}


///TODO 0.2.3 remove
void loadImages(){
	QPixmap tmpImg(getImageFile("clientic.png"));
	EntryIcons=new QPixmap[BUILTIN_ICONS];
	for(int i=0;i<BUILTIN_ICONS;i++){
	EntryIcons[i]=tmpImg.copy(i*16,0,16,16);}

}


bool loadTranslation(QTranslator* tr,const QString& prefix,const QString& loc,const QStringList& paths){
	for(int i=0;i<paths.size();i++)
		if(tr->load(prefix+loc+".qm",paths[i])) return true;
	
	for(int i=0;i<paths.size();i++){
		QDir dir(paths[i]);
		QStringList TrFiles=dir.entryList(QStringList()<<"*.qm",QDir::Files);
		for(int j=0;j<TrFiles.size();j++){
			if(TrFiles[j].left(prefix.length()+2)==prefix+loc.left(2)){
				if(tr->load(TrFiles[j],paths[i]))return true;
			}
		}
	}
	return false;
}


CmdLineArgs::CmdLineArgs(){
	StartMinimized=false;
	StartLocked=false;
	Help=false;
}

bool CmdLineArgs::parse(const QStringList& argv){
	for(int i=1;i<argv.size();i++){
		if(argv[i]=="-help"){
			Help=true;
			break; // break, because other arguments will be ignored anyway
		}
		if(argv[i]=="-lang"){
			if(argv.size()==i+1){
				Error="Missing argument for '-lang'.";
				return false;
			}
			if(argv[i+1].size() != 2 && argv[i+1].size() != 5 ){
				Error=QString("'%1' is not a valid language code.").arg(argv[i+1]);
				return false;
			}
			Language=argv[i+1];
			i++;
			continue;
		}
		if(argv[i]=="-cfg"){
			//already done in preparse() -> skip
			i++;
			continue;
		}
		if(argv[i]=="-min"){
			StartMinimized=true;
			continue;
		}
		if(argv[i]=="-lock"){
			StartLocked=true;
			continue;
		}
		if(i==1){
			File=argv[1];
			continue;
		}
		Error=QString("** Unrecognized argument: '%1'").arg(argv[i]);
		return false;
	}
	return true;	
}

bool CmdLineArgs::preparse(int argc,char** argv){
	for(int i=1;i<argc;i++){
		if(QString(argv[i])=="-cfg"){
			if(argc==i+1){
				Error="Missing argument for '-cfg'.";
				return false;
			}
			if(QString(argv[i+1]).left(1)=="-"){
				Error=QString("Expected a path as argument for '-cfg' but got '%1.'").arg(argv[i+1]);
				return false;
			}
			QFileInfo file(argv[i+1]);
			ConfigLocation=file.absoluteFilePath();
			i++;
			return true;
		}
	}
	return true;
}

void CmdLineArgs::printHelp(){
	cout << "KeePassX" << APP_VERSION << endl;
	cout << "Usage: keepassx  [Filename] [Options]" << endl;
	cout << "  -help             This Help" << endl;
	cout << "  -cfg <CONFIG>     Use specified file for loading/saving the configuration." << endl;
	cout << "  -min              Start minimized." << endl;
	cout << "  -lock             Start locked." << endl;
	cout << "  -lang <LOCALE>    Use specified language instead of systems default." << endl;
	cout << "                    <LOCALE> is the ISO-639 language code with or without ISO-3166 country code" << endl;
	cout << "                    Examples: de     German" << endl;
	cout << "                              de_CH  German(Switzerland)"<<endl;
	cout << "                              pt_BR  Portuguese(Brazil)"<<endl;
}


QString findPlugin(const QString& filename){
	QFileInfo info;	
	info.setFile(AppDir+"/../lib/"+filename);
	if(info.exists() && info.isFile())
		return AppDir+"/../lib/"+filename;	
	return QString();
}


bool EventListener::eventFilter(QObject*, QEvent* event){
	if (!EventOccurred){
		int t = event->type();
		if ( t>=QEvent::MouseButtonPress&&t<=QEvent::KeyRelease || t>=QEvent::HoverEnter&&t<=QEvent::HoverMove )
			EventOccurred = true;
	}
	
	return false;
}
