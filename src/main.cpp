/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <iostream.h>
#include <qapplication.h>
#include <qlibrary.h>
#include <qlocale.h>
#include <qdir.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QTranslator>

#include "pwsafe.h"
#include "PwmConfig.h"
#include "lib/KdePlugin.h"

void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg){
if(argc>1){
int i=1;
	if(argv[i][0]!='-'){
		ArgFile=QString::fromUtf8(argv[i]);
		i++;
	}
	for(i; i<argc;i++){
		if(QString(argv[i])=="-h"){
			cout << "Keepass 0.1.3 (Alpha)" << endl;
			cout << "Usage: keepass [Filename] [Options]" << endl;
			cout << "  -h This Help" << endl;
			cout << "  -cfg ConfigFile Use specified configuration" << endl;
			exit(0);
			}
		else if(QString(argv[i])=="-cfg"){
			if(i-1==argc) cout << "No configuration file specified." << endl;
			else{ArgCfg=QString::fromUtf8(argv[i+1]); i++;}
			}
		else{cout << "** Unrecognized option: " << argv[i] <<  endl;
			exit(1);}
	}

}
}

int main(int argc, char **argv)
{
QString ArgFile,ArgCfg,IniFilename;
parseCmdLineArgs(argc,argv,ArgFile,ArgCfg);
CConfig config;

//Load Config
if(ArgCfg==""){
 if(!QDir(QDir::homeDirPath()+"/.keepass").exists()){
	QDir conf(QDir::homeDirPath());
	if(!conf.mkdir(".keepass")){
		cout << "Warning: Could not create directory '~/.keepass'." << endl;}
 }
 IniFilename=QDir::homeDirPath()+"/.keepass/config";
 config.loadFromIni(IniFilename);
}
else
{
 IniFilename=ArgCfg;
 config.loadFromIni(IniFilename);
}

//KDE PlugIn
QApplication* app=NULL;
if(config.EnableKdePlugin){
cout << "don't activate the kde plugin option" << endl;
exit(1);
/*
  QLibrary lib("/home/tarek/Documents/keepass_kde/bin/libkeepass_kde.so");
  if(!lib.load()){
		cout << "Could not load KDE plugin." << endl;
		exit(1);
  }
  CKdePlugin KdePlugin;
  if(!KdePlugin.resolveSymbols(lib)){
		cout << "KDE plugin: Symbols could not be resolved." << endl;
		exit(1);
	}
  app=KdePlugin.getAppObj(argc,argv);
  QApplication*(*_getAppObj)(int,char**);
  _getAppObj=(QApplication*(*)(int,char**))lib.resolve("getAppObj");
  app=_getAppObj(argc,argv**);
  if(!app){cout << "app==NULL" << endl;
		 exit(1);
			}
*/
}
else{
app=new QApplication(argc,argv);
}



//Internationalization
QLocale loc=QLocale::system();
QTranslator* translator = 0;
translator =new QTranslator;
if(config.Language==""){
 switch(loc.language()){
   case QLocale::German:
	config.Language="_DEUTSCH_";
	break;
   case QLocale::Russian:
	config.Language="russian.qm";
	break;
   case QLocale::English:
	config.Language="english.qm";
	break;
   default:
	config.Language="english.qm";
	break;}
}
if(config.Language!="_DEUTSCH_"){
  if(!translator->load(app->applicationDirPath()+"/../share/keepass/i18n/"+config.Language)){
   if(!translator->load(app->applicationDirPath()+"/share/i18n/"+config.Language)){
    config.Language="_DEUTSCH_";
    QMessageBox::warning(NULL,"Warning",
			 QString("Translation file '%1' could not be loaded.")
			 .arg(config.Language),"OK",0,0,2,1);
    delete translator;
   translator=NULL;}}
  else app->installTranslator(translator);
}

 PwSafe *mainWin = 0;
 
 mainWin = new PwSafe(app,ArgFile,&config);
 app->setMainWidget( mainWin );
 mainWin->show();
 int ret=app->exec();
 if(!config.saveToIni(IniFilename))
	QMessageBox::warning(NULL,QObject::tr("Warnung"),QObject::trUtf8("Die Konfigurationsdatei konnte nicht gespeichert werden.Stellen Sie sicher, dass\nSie Schreibrechte im Verzeichnis ~/.keepass besitzen."),QObject::tr("OK"),"","",0.0);
 delete app;
 return ret;
}








/********* KDE **************
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("Keepass", I18N_NOOP("Keepass"), version, description,
                     KAboutData::License_BSD, "(C) %{YEAR} Tarek Saidi", 0, 0, "tareks@arcor.de");
    about.addAuthor( "Tarek Saidi", 0, "tareks@arcor.de" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    PwSafe *mainWin = 0;

    if (app.isRestored())
    {
        RESTORE(PwSafe);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here

        mainWin = new PwSafe();
        app.setMainWidget( mainWin );
        mainWin->show();

        args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

*/


