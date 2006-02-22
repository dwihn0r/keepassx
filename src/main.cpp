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

#include "main.h"
#include "PwmConfig.h"
#include "PwManager.h"
#include "mainwindow.h"
using namespace std;

CConfig config;
QString  AppDir;
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
QString DateTimeFormat("no-format-string");


inline void loadImages();
inline void parseCmdLineArgs(int argc, char** argv,QString &ArgFile,QString& ArgCfg);


int main(int argc, char **argv)
{
QApplication* app=new QApplication(argc,argv);
QString ArgFile,ArgCfg,IniFilename;
parseCmdLineArgs(argc,argv,ArgFile,ArgCfg);
AppDir=app->applicationDirPath();
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
else{
 IniFilename=ArgCfg;
 config.loadFromIni(IniFilename);}


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

DateTimeFormat=QObject::trUtf8("dd'.'MM'.'yy' 'hh':'mm");
loadImages();

SecString::generateSessionKey();

KeepassMainWindow *mainWin = new KeepassMainWindow();
mainWin->show();
int r=app->exec();
if(!config.saveToIni(IniFilename))
	QMessageBox::warning(NULL,QObject::tr("Warnung"),QObject::trUtf8("Die Konfigurationsdatei konnte nicht gespeichert werden.Stellen Sie sicher, dass\nSie Schreibrechte im Verzeichnis ~/.keepass besitzen."),QObject::tr("OK"),"","",0.0);
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
 QMessageBox::critical(0,QObject::trUtf8("Fehler"),QObject::trUtf8("Die Datei '%1' konnte nicht gefunden werden.")
				   .arg(name),QObject::tr("OK"),0,0,2,1);
 exit(1);
}}

}



void loadImages(){
QString ThemeDir="nuvola/32x32";
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
loadImg(ThemeDir+"/actions/filenew.png",tmpImg);
Icon_FileNew=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/fileopen.png",tmpImg);
Icon_FileOpen=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/filesave.png",tmpImg);
Icon_FileSave=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/filesaveas.png",tmpImg);
Icon_FileSaveAs=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/fileclose.png",tmpImg);
Icon_FileClose=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/exit.png",tmpImg);
Icon_Exit=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/editdelete.png",tmpImg);
Icon_EditDelete=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/edit_add.png",tmpImg);
Icon_EditAdd=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/edit.png",tmpImg);
Icon_EditEdit=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/identity.png",tmpImg);
Icon_EditUsernameToCb=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/klipper_dock.png",tmpImg);
Icon_EditPasswordToCb=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/editcopy.png",tmpImg);
Icon_EditClone=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/run.png",tmpImg);
Icon_EditOpenUrl=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/find.png",tmpImg);
Icon_EditSearch=new QIcon(tmpImg);
//--------------------------
loadImg(ThemeDir+"/actions/configure.png",tmpImg);
Icon_Configure=new QIcon(tmpImg);

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
QMessageBox::critical(parent,QObject::tr("Fehler"),msg,QObject::tr("OK"));
}
