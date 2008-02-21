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

#include <iostream>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QPainter>
#include <QImage>
#include <QStyleFactory>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QVarLengthArray>


/*
#include <QLibary>
#include <QPluginLoader>
#include "plugins/interfaces/IFileDialog.h"
#include "plugins/interfaces/IKdeInit.h"
#include "plugins/interfaces/IGnomeInit.h"
#include "plugins/interfaces/IIconTheme.h"
#include "lib/FileDialogs.h"
*/

#include "main.h"
#include "lib/FileDialogs.h"
#include "lib/bookmarks.h"
#include "KpxConfig.h"
#include "Kdb3Database.h"
#include "mainwindow.h"
#include "crypto/yarrow.h"
#if defined(Q_WS_X11) && defined(GLOBAL_AUTOTYPE)
#include "Application_X11.h"
#endif
#ifdef Q_WS_WIN
#include <windows.h>
#endif
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

using namespace std;

QHash<QString,QPixmap*>PixmapCache;
QHash<QString,QIcon*>IconCache;

KpxConfig *config;
QString  AppDir;
QString HomeDir;
QString PluginLoadError;
bool TrActive;
QString DetailViewTemplate;

QPixmap* EntryIcons;
//IIconTheme* IconLoader=NULL; //TODO plugins
char ** argv;

inline void loadImages();
inline void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg,QString& ArgLang,bool& ArgMin,bool& ArgLock);
bool loadTranslation(QTranslator* tr,const QString& prefix,const QString& LocaleCode,const QStringList& SearchPaths);

int main(int argc, char **_argv)
{
	argv=_argv;
	QString ArgFile,ArgCfg,ArgLang,IniFilename;
	QApplication* app=NULL;
	AppDir=applicationDirPath();
#if defined Q_WS_WIN
	HomeDir = QString::fromLocal8Bit(qgetenv("APPDATA").constData());
	if(!HomeDir.isEmpty() && QFile::exists(HomeDir))
		HomeDir = QDir::fromNativeSeparators(HomeDir)+"/KeePassX";
	else
		HomeDir = QDir::homePath()+"/KeePassX";
#else
	HomeDir = QDir::homePath()+"/.keepassx";
#endif
	bool ArgMin = false;
	bool ArgLock = false;
	parseCmdLineArgs(argc,argv,ArgFile,ArgCfg,ArgLang,ArgMin,ArgLock);
	qDebug(CSTR(QDir::current().absolutePath()));

	//Load Config
	if(ArgCfg.isEmpty()){
		if(!QDir(HomeDir).exists()){
			QDir conf(QDir::homePath());
			if(!QDir().mkpath(HomeDir))
				qDebug("Warning: Could not create directory '%s'", CSTR(HomeDir));
		}
		IniFilename=HomeDir+"/config";
	}
	else
		IniFilename=ArgCfg;


	config = new KpxConfig(IniFilename);
	fileDlgHistory.load();

	// TODO Plugins
	/*if(config->integrPlugin()!=KpxConfig::NoIntegr){
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
	}*/
	
#if defined(Q_WS_X11) && defined(GLOBAL_AUTOTYPE)
	if(!app) new KeepassApplication(argc,argv);
#else
	if(!app) new QApplication(argc,argv);
#endif


	//Internationalization
	QLocale loc;
	if(!ArgLang.size())
		loc=QLocale::system();
	else
		loc=QLocale(ArgLang);

	QTranslator* translator = NULL;
	QTranslator* qtTranslator=NULL;
	translator=new QTranslator;
	qtTranslator=new QTranslator;

	if(loadTranslation(translator,"keepass-",loc.name(),QStringList()
						<< AppDir+"/../share/keepass/i18n/"
						<< AppDir+"/share/i18n/"
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
							<< AppDir+"/../share/keepass/i18n/"
							<< AppDir+"/share/i18n/"
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

	QApplication::setQuitOnLastWindowClosed(false);
	KeepassMainWindow *mainWin = new KeepassMainWindow(ArgFile,ArgMin,ArgLock);
	int r=app->exec();
	delete mainWin;

	fileDlgHistory.save();
	delete app;
	delete config;
	return r;
}


void createBanner(QPixmap* Pixmap,const QPixmap* IconAlpha,const QString& Text,int Width){
	createBanner(Pixmap,IconAlpha,Text,Width,config->bannerColor1(),config->bannerColor2(),config->bannerTextColor());
}

void createBanner(QPixmap* Pixmap,const QPixmap* IconAlpha,const QString& Text,int Width, QColor Color1, QColor Color2, QColor TextColor){
	*Pixmap=QPixmap(Width,50);
	QPainter painter(Pixmap);
	QLinearGradient grad(0,0,Width,0);
	grad.setColorAt(0,Color1);
	grad.setColorAt(1,Color2);
	painter.setPen(Qt::NoPen);
	painter.setBrush(grad);
	painter.drawRect(0,0,Width,50);

	QPixmap Icon(32,32);
	if(IconAlpha){
		Icon.fill(TextColor);
		Icon.setAlphaChannel(*IconAlpha);
		painter.drawPixmap(10,10,Icon);
	}

	painter.setPen(QPen(TextColor));
	painter.setFont(QFont(QApplication::font().family(),16));
	painter.drawText(50,35,Text);
}

QString decodeFileError(QFile::FileError Code){
	switch(Code){
		case QFile::NoError: return QApplication::translate("FileErrors","No error occurred.");
		case QFile::ReadError: return QApplication::translate("FileErrors","An error occurred while reading from the file.");
		case QFile::WriteError: return QApplication::translate("FileErrors","An error occurred while writing to the file.");
		case QFile::FatalError: return QApplication::translate("FileErrors","A fatal error occurred.");
		case QFile::ResourceError: return QApplication::translate("FileErrors","An resource error occurred.");
		case QFile::OpenError: return QApplication::translate("FileErrors","The file could not be opened.");
		case QFile::AbortError: return QApplication::translate("FileErrors","The operation was aborted.");
		case QFile::TimeOutError: return QApplication::translate("FileErrors","A timeout occurred.");
		case QFile::UnspecifiedError: return QApplication::translate("FileErrors","An unspecified error occurred.");
		case QFile::RemoveError: return QApplication::translate("FileErrors","The file could not be removed.");
		case QFile::RenameError: return QApplication::translate("FileErrors","The file could not be renamed.");
		case QFile::PositionError: return QApplication::translate("FileErrors","The position in the file could not be changed.");
		case QFile::ResizeError: return QApplication::translate("FileErrors","The file could not be resized.");
		case QFile::PermissionsError: return QApplication::translate("FileErrors","The file could not be accessed.");
		case QFile::CopyError: return QApplication::translate("FileErrors","The file could not be copied.");
	}
	return QString();
}

void openBrowser(IEntryHandle* entry){
	QString url = entry->url();
	url.replace("{TITLE}", entry->title(), Qt::CaseInsensitive);
	url.replace("{USERNAME}", entry->username(), Qt::CaseInsensitive);
	
	if (url.contains("{PASSWORD}",Qt::CaseInsensitive)){
		SecString password=entry->password();
		password.unlock();
		url.replace("{PASSWORD}", password, Qt::CaseInsensitive);
	}
	
	openBrowser(url);
}

void openBrowser(const QString& UrlString){
	if (UrlString.startsWith("cmd://") && UrlString.length()>6){
		QProcess::startDetached(UrlString.right(UrlString.length()-6));
		return;
	}
	
	QUrl url(UrlString);
	if(url.scheme().isEmpty())
		url=QUrl("http://"+UrlString);
	if(config->urlCmdDef() || url.scheme()=="mailto"){
		QDesktopServices::openUrl(url);
	}
	else{
		QByteArray UrlEncoded = url.toEncoded();
		QString browser = config->urlCmd();
		if (browser.contains("%u", Qt::CaseInsensitive))
			browser.replace("%u", UrlEncoded, Qt::CaseInsensitive);
		else if (browser.contains("%1"))
			browser.replace("%1", UrlEncoded);
		else
			browser.append(" ").append(UrlEncoded);
		QProcess::startDetached(browser);
	}
}

QString getImageFile(const QString& name){
	if (QFile::exists(AppDir+"/../share/keepass/icons/"+name))
		return AppDir+"/../share/keepass/icons/"+name;
	else if (QFile::exists(AppDir+"/share/icons/"+name))
		return AppDir+"/share/icons/"+name;
	else{
		QMessageBox::critical(0,QApplication::translate("Main","Error"),
			QApplication::translate("Main","File '%1' could not be found.")
			.arg(name),QApplication::translate("Main","OK"),0,0,2,1);
		exit(1);
	}
}

///TODO 0.2.3 remove
void loadImages(){
	QPixmap tmpImg(getImageFile("clientic.png"));
	EntryIcons=new QPixmap[BUILTIN_ICONS];
	for(int i=0;i<BUILTIN_ICONS;i++){
	EntryIcons[i]=tmpImg.copy(i*16,0,16,16);}

}

const QIcon& getIcon(const QString& name){
	QIcon* CachedIcon=IconCache.value(name);
	if(CachedIcon)
		return *CachedIcon;
	QIcon* NewIcon=NULL;
	//TODO plugins
	/*if(IconLoader){
		NewIcon=new QIcon(IconLoader->getIcon(name));
		if(NewIcon->isNull()){
			delete NewIcon;
			NewIcon=NULL;
		}
		else
			IconCache.insert(name,NewIcon);
	}*/
	if(!NewIcon)
	{
		NewIcon=new QIcon(getImageFile(name+".png"));
		IconCache.insert(name,NewIcon);
	}
	return *NewIcon;
}

const QPixmap* getPixmap(const QString& name){
	QPixmap* CachedPixmap=PixmapCache.value(name);
	if(CachedPixmap)
		return CachedPixmap;
	QImage img(getImageFile(name+".png"));
	QPixmap* NewPixmap=new QPixmap(QPixmap::fromImage(img));
	PixmapCache.insert(name,NewPixmap);
	return NewPixmap;
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

void printHelp(){
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

void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg,QString& ArgLang,bool& ArgMin, bool& ArgLock){
if(argc>1){
	int i=1;
	if(argv[i][0]!='-'){
		ArgFile=QString::fromUtf8(argv[i]);
		i++;
	}
	for(; i<argc; i++){
		if(QString(argv[i])=="-help"){
			printHelp();
			exit(0);
			}
		else if(QString(argv[i])=="-cfg"){
			if(i-1==argc){ cout << "Missing argument for -cfg" << endl; exit(1);}
			else{ArgCfg=QString::fromUtf8(argv[i+1]); i++;}
			}
		else if(QString(argv[i])=="-lang"){
			if(i-1==argc)
				cout << "Missing argument for -lang" << endl;
			else
				ArgLang=QString::fromUtf8(argv[i+1]); i++;
		}
		else if(QString(argv[i])=="-min"){
			ArgMin = true;
		}
		else if(QString(argv[i])=="-lock"){
			ArgLock = true;
		}
		/*else if(QString(argv[i])=="-test"){
			if (testDatabase()) exit(0);
			else exit(1);
		}*/
		else{
			cout << "** Unrecognized argument: " << argv[i] <<  endl << endl;
			printHelp();
			exit(1);
		}
	}
   }
}

void showErrMsg(const QString& msg,QWidget* parent){
	QMessageBox::critical(parent,QApplication::translate("Main","Error"),msg,QApplication::translate("Main","OK"));
}

//TODO Plugins
/*
QString findPlugin(const QString& filename){
	QFileInfo info;

	info.setFile(AppDir+"/../lib/"+filename);
	if(info.exists() && info.isFile())
		return AppDir+"/../lib/"+filename;

	return QString();
}
*/

QString makePathRelative(const QString& AbsDir,const QString& CurDir){
	QStringList abs=AbsDir.split('/');
	QStringList cur=CurDir.split('/');
	QString rel="./";
	int common=0;
	for(common; common < abs.size() && common < cur.size(); common++){
		if(abs[common]!=cur[common])break;
	}
	for(int i=0;i<cur.size()-common;i++)
		rel.append("../");
	for(int i=common;i<abs.size();i++)
		rel.append(abs[i]+"/");
	return rel;
}


QString applicationDirPath(){
	QString filepath=applicationFilePath();
	filepath.truncate(filepath.lastIndexOf("/"));
	return filepath;
}

QString applicationFilePath()
{
	#if defined( Q_WS_WIN )
    QFileInfo filePath;
    QT_WA({
        wchar_t module_name[256];
        GetModuleFileNameW(0, module_name, sizeof(module_name) / sizeof(wchar_t));
        filePath = QString::fromUtf16((ushort *)module_name);
    }, {
        char module_name[256];
        GetModuleFileNameA(0, module_name, sizeof(module_name));
        filePath = QString::fromLocal8Bit(module_name);
    });

    return filePath.filePath();
	#elif defined(Q_WS_MAC)
	
	CFURLRef bundleURL(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
	assert(bundleURL);
	CFStringRef cfPath(CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle));
	assert(cfPath);
    CFIndex length = CFStringGetLength(cfPath);
    const UniChar *chars = CFStringGetCharactersPtr(cfPath);
    if (chars)
        return QString(reinterpret_cast<const QChar *>(chars), length);

    QVarLengthArray<UniChar> buffer(length);
    CFStringGetCharacters(cfPath, CFRangeMake(0, length), buffer.data());
    return QString(reinterpret_cast<const QChar *>(buffer.constData()), length);

	#elif defined( Q_OS_UNIX )
	#ifdef Q_OS_LINUX
    // Try looking for a /proc/<pid>/exe symlink first which points to
    // the absolute path of the executable
    QFileInfo pfi(QString::fromLatin1("/proc/%1/exe").arg(getpid()));
    if (pfi.exists() && pfi.isSymLink())
        return pfi.canonicalFilePath();
	#endif

    QString argv0 = QFile::decodeName(QByteArray(argv[0]));
    QString absPath;

    if (!argv0.isEmpty() && argv0.at(0) == QLatin1Char('/')) {
        /*
          If argv0 starts with a slash, it is already an absolute
          file path.
        */
        absPath = argv0;
    } else if (argv0.contains(QLatin1Char('/'))) {
        /*
          If argv0 contains one or more slashes, it is a file path
          relative to the current directory.
        */
        absPath = QDir::current().absoluteFilePath(argv0);
    } else {
        /*
          Otherwise, the file path has to be determined using the
          PATH environment variable.
        */
        QByteArray pEnv = qgetenv("PATH");
        QDir currentDir = QDir::current();
        QStringList paths = QString::fromLocal8Bit(pEnv.constData()).split(QLatin1String(":"));
        for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
            if ((*p).isEmpty())
                continue;
            QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
            QFileInfo candidate_fi(candidate);
            if (candidate_fi.exists() && !candidate_fi.isDir()) {
                absPath = candidate;
                break;
            }
        }
    }

    absPath = QDir::cleanPath(absPath);

    QFileInfo fi(absPath);
    return fi.exists() ? fi.canonicalFilePath() : QString();
	#endif
}
