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

#include <iostream>
#include <qapplication.h>
#include <qlibrary.h>
#include <qlocale.h>
#include <qdir.h>
#include <QMessageBox>
#include <QTranslator>
#include <QPainter>
#include <QImage>
#include <QStyleFactory>
#include <QProcess>
#include <QLibraryInfo>

#include "main.h"
#include "PwmConfig.h"
#include "PwManager.h"
#include "mainwindow.h"
using namespace std;

CConfig config;
QString  AppDir;
bool TrActive;
QPixmap *Icon_Key32x32;
QPixmap *Icon_Settings32x32;
QPixmap *Icon_Search32x32;
QPixmap *Icon_I18n32x32;
QPixmap *Icon_Ok16x16;
QPixmap *EntryIcons;
QIcon *Icon_FileNew;
QIcon *Icon_FileOpen;
QIcon *Icon_FileClose;
QIcon *Icon_FileSave;
QIcon *Icon_FileSaveAs;
QIcon *Icon_Exit;
QIcon *Icon_File_Export;
QIcon *Icon_EditDelete;
QIcon *Icon_EditAdd;
QIcon *Icon_EditEdit;
QIcon *Icon_EditUsernameToCb;
QIcon *Icon_EditPasswordToCb;
QIcon *Icon_EditClone;
QIcon *Icon_EditOpenUrl;
QIcon *Icon_EditSearch;
QIcon *Icon_Configure;
QIcon *Icon_Help;


inline void loadImages();
inline void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg);


int main(int argc, char **argv)
{
QApplication* app=new QApplication(argc,argv);
QString ArgFile,ArgCfg,IniFilename;
parseCmdLineArgs(argc,argv,ArgFile,ArgCfg);
AppDir=app->applicationDirPath();
//Load Config
if(ArgCfg==QString()){
 if(!QDir(QDir::homeDirPath()+"/.keepass").exists()){
	QDir conf(QDir::homeDirPath());
	if(!conf.mkdir(".keepass")){
		cout << "Warning: Could not create directory '~/.keepass'." << endl;}
 }
 IniFilename=QDir::homeDirPath()+"/.keepass/config";
 config.loadFromIni(IniFilename);
}
else{
 IniFilename=ArgCfg;
 config.loadFromIni(IniFilename);}



//Internationalization
QLocale loc=QLocale::system();
QTranslator* translator = NULL;
QTranslator* qtTranslator=NULL;
translator=new QTranslator;
qtTranslator=new QTranslator;
bool TrFound=true;
if(!translator->load("keepass-"+loc.name(),app->applicationDirPath()+"/../share/keepass/i18n/")){
	if(!translator->load("keepass-"+loc.name(),QDir::homeDirPath()+"/.keepass/")){
		qWarning(QString("KeePassX: No Translation found for language '%1 (%2)'")
				.arg(QLocale::languageToString(loc.language()))
				.arg(QLocale::countryToString(loc.country())));
		TrFound=false;
	}
}

if(TrFound)
	app->installTranslator(translator);
else 
	delete translator;


if(!qtTranslator->load("qt_"+loc.name().left(2),QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
	qWarning(QString("Qt: No Translation found for '%1 (%2)'")
			.arg(QLocale::languageToString(loc.language()))
			.arg(QLocale::countryToString(loc.country())));
	delete qtTranslator;
}else{
	app->installTranslator(qtTranslator);
}



TrActive=TrFound;
loadImages();
SecString::generateSessionKey();
int r=0;
KeepassMainWindow *mainWin = new KeepassMainWindow(ArgFile);
if(mainWin->Start){
	mainWin->show();
	r=app->exec();
}
delete mainWin;
if(!config.saveToIni(IniFilename))
	QMessageBox::warning(NULL,QObject::tr("Warning"),
			QObject::tr("Could not save configuration file.\nMake sure you have write access to '~/.keepass'."),
			QObject::tr("OK"),"","",0.0);
delete app;
return r;
}




void createBanner(QLabel *Banner,QPixmap* symbol,QString text){
createBanner(Banner,symbol,text,config.BannerColor1
			       ,config.BannerColor2
			       ,config.BannerTextColor); //call overloaded function
}


void createBanner(QLabel *Banner,QPixmap* symbol,QString text,QColor color1,QColor color2,QColor textcolor){
int w=Banner->width();
int h=Banner->height();
QColor color;
float b1[3];
float b2[3];
float a1,a2;
QPixmap* banner_pixmap=new QPixmap(w,h); ///@FIXME löscht der Destruktor von QLabel die Pixmap zum schluss???
QPainter painter(banner_pixmap);
QPen pen;
pen.setWidth(1);
painter.setPen(pen);
QFont font("Arial",16);
painter.setFont(font);
if(color1!=color2){
 b1[0]=color1.red();
 b1[1]=color1.green();
 b1[2]=color1.blue();
 b2[0]=color2.red();
 b2[1]=color2.green();
 b2[2]=color2.blue();
 for(int x=0;x<w;x++){
  a2=(float)x/(float)w;
  a1=1-a2;
  color.setRgb(	(int)(a1*b1[0]+a2*b2[0]),
		(int)(a1*b1[1]+a2*b2[1]),
		(int)(a1*b1[2]+a2*b2[2]));
  pen.setColor(color);
  painter.setPen(pen);
  painter.drawLine(x,0,x,h);
 }
}
else{
 banner_pixmap->fill(color1);
}
painter.drawPixmap(10,10,*symbol);
pen.setColor(textcolor);
painter.setPen(pen);
painter.drawText(50,30,text);
Banner->setPixmap(*banner_pixmap);
}



void openBrowser(QString url){
QProcess browser;
QStringList args=QStringList::split(' ',config.OpenUrlCommand.arg(url));
QString cmd=args.takeFirst();
browser.startDetached(cmd,args);
}


void loadImg(QString name,QPixmap& Img){
if(Img.load(AppDir+"/../share/keepass/icons/"+name)==false){
 if(Img.load(AppDir+"/share/"+name)==false){
 QMessageBox::critical(0,QObject::tr("Error"),QObject::tr("File '%1' could not be found.")
				   .arg(name),QObject::tr("OK"),0,0,2,1);
 exit(1);
}}

}


#define _loadIcon(Icon,PATH)\
	Icon=new QIcon(ThemeDir+PATH);


void loadImages(){

bool small=true;
QString ThemeDir=AppDir+"/../share/keepass/icons/nuvola/32x32";
QPixmap tmpImg;

//-----------------------
loadImg("clientic.png",tmpImg);
EntryIcons=new QPixmap[NUM_CLIENT_ICONS];
for(int i=0;i<NUM_CLIENT_ICONS;i++){
EntryIcons[i]=tmpImg.copy(i*16,0,16,16);}

//--------------------------
loadImg("key.png",tmpImg);
Icon_Key32x32=new QPixmap;
*Icon_Key32x32=tmpImg;
//--------------------------
loadImg("settings.png",tmpImg);
Icon_Settings32x32=new QPixmap;
*Icon_Settings32x32=tmpImg;
//--------------------------
loadImg("i18n.png",tmpImg);
Icon_I18n32x32=new QPixmap;
*Icon_I18n32x32=tmpImg;
//--------------------------
loadImg("ok.png",tmpImg);
Icon_Ok16x16=new QPixmap;
*Icon_Ok16x16=tmpImg;
//--------------------------
loadImg("search.png",tmpImg);
Icon_Search32x32=new QPixmap;
*Icon_Search32x32=tmpImg;
//--------------------------


_loadIcon(Icon_FileNew,"/actions/filenew.png");
_loadIcon(Icon_FileOpen,"/actions/fileopen.png");
_loadIcon(Icon_FileSave,"/actions/filesave.png");
_loadIcon(Icon_FileSaveAs,"/actions/filesaveas.png");
_loadIcon(Icon_FileClose,"/actions/fileclose.png");
_loadIcon(Icon_Exit,"/actions/exit.png");
_loadIcon(Icon_EditDelete,"/actions/editdelete.png");
_loadIcon(Icon_EditAdd,"/actions/edit_add.png");
_loadIcon(Icon_EditEdit,"/actions/edit.png");
_loadIcon(Icon_EditUsernameToCb,"/actions/identity.png");
_loadIcon(Icon_EditPasswordToCb,"/actions/klipper_dock.png");
_loadIcon(Icon_EditClone,"/actions/editcopy.png");
_loadIcon(Icon_EditOpenUrl,"/actions/run.png");
_loadIcon(Icon_EditSearch,"/actions/find.png");
_loadIcon(Icon_Configure,"/actions/configure.png");
_loadIcon(Icon_Help,"/actions/help.png");

}


void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg){
if(argc>1){
int i=1;
	if(argv[i][0]!='-'){
		ArgFile=QString::fromUtf8(argv[i]);
		i++; }
	for(i; i<argc;i++){
		if(QString(argv[i])=="-h"){
			cout << "Keepass 0.2.0" << endl;
			cout << "Usage: keepass [Filename] [Options]" << endl;
			cout << "  -h This Help" << endl;
			cout << "  -cfg ConfigFile Use specified configuration" << endl;
			exit(0);
			}
		else if(QString(argv[i])=="-cfg"){
			if(i-1==argc) cout << "No configuration file specified." << endl;
			else{ArgCfg=QString::fromUtf8(argv[i+1]); i++;}
			}
		else if(QString(argv[i])=="-test"){
				if (testDatabase()) exit(0);
				else exit(1);
			}
		else{cout << "** Unrecognized argument: " << argv[i] <<  endl;
			exit(1);}
	}
   }
}

void showErrMsg(const QString& msg,QWidget* parent){
QMessageBox::critical(parent,QObject::tr("Error"),msg,QObject::tr("OK"));
}
