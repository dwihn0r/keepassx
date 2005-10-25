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

//QT
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qimage.h>
#include <qlistview.h>
#include <qtextedit.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qprocess.h>
#include <qfiledialog.h>
#include <qpoint.h>
#include <qstatusbar.h>
#include <qcolor.h>
#include <qsplitter.h>
#include <qvaluelist.h>
#include <qlocale.h>
#include <qmessagebox.h>
//STD
#include <time.h>
//local
#include "lib/random.h"
#include "lib/IniReader.h"
#include "ListViews.h"
#include <iostream.h>

#include "dialogs/AboutDlg.h"
#include "dialogs/EditGroupDlg.h"
#include "dialogs/SearchDlg.h"
#include "dialogs/ChangeKeyDlg.h"
#include "dialogs/LanguageDlg.h"
#include "dialogs/SettingsDlg.h"
#include "dialogs/DatabaseSettingsDlg.h"
#include "dialogs/PasswordDlg.h"
#include "dialogs/SimplePasswordDlg.h"
#include "dialogs/EditEntryDlg.h"
#include "dialogs/PasswordGenDlg.h"

#include "import/Import_PwManager.h"
#include "import/Import_KWalletXml.h"

CMainWindow::CMainWindow(QApplication* app,QString ArgFile,QString ArgCfg,QWidget* parent,const char* name, WFlags fl)
: MainFrame(parent,name,fl)
{
FileOpen=false;
App=app;
appdir=app->applicationDirPath();
parentWidget()->setCaption(tr("Keepass Passwortsafe"));
SecString::generateSessionKey();

// Config //
if(ArgCfg==""){
 if(!QDir(QDir::homeDirPath()+"/.keepass").exists()){
	QDir conf(QDir::homeDirPath());
	if(!conf.mkdir(".keepass")){
		cout << trUtf8("Warnung: Verzeichnis ~/.keepass konnte nicht erstellt werden.") << endl;}
 }
 IniFilename=QDir::homeDirPath()+"/.keepass/config";
 config.loadFromIni(IniFilename);
}
else
{
 IniFilename=ArgCfg;
 config.loadFromIni(IniFilename);
}



CGroup::UI_ExpandByDefault=config.ExpandGroupTree;

// Language //
QLocale loc=QLocale::system();
translator = 0;
translator =new QTranslator;

if(config.Language==""){
 switch(loc.language()){
   case QLocale::German:
	config.Language="_DEUTSCH_";
	break;
   case QLocale::English:
	config.Language="english.qm";
	break;
   default:
	config.Language="english.qm";
	break;}
}

if(config.Language!="_DEUTSCH_"){
  if(!translator->load(appdir+"/../share/keepass/i18n/"+config.Language)){
   if(!translator->load(appdir+"/share/i18n/"+config.Language)){
    config.Language="_DEUTSCH_";
    QMessageBox::warning(this,tr("Warnung"),
			 trUtf8("Die Übersetzungsdatei '%1' konnte nicht geladen werden.\n Die Sprache wurde auf Deutsch zurückgesetzt.")
			 .arg(config.Language),tr("OK"),0,0,2,1);
    delete translator;
   translator=NULL;}}
  else app->installTranslator(translator);
}


// Icons, Pixmaps //

QImage tmpImg;
//-----------------------
LoadImg("clientic.png",tmpImg);
EntryIcons=new QPixmap[52];
for(int i=0;i<52;i++){
EntryIcons[i]=tmpImg.copy(i*16,0,16,16);
}

//--------------------------
LoadImg("key.png",tmpImg);
Icon_Key32x32=new QPixmap;
*Icon_Key32x32=tmpImg;

//--------------------------
LoadImg("settings.png",tmpImg);
Icon_Settings32x32=new QPixmap;
*Icon_Settings32x32=tmpImg;

//--------------------------
LoadImg("i18n.png",tmpImg);
Icon_I18n32x32=new QPixmap;
*Icon_I18n32x32=tmpImg;

//--------------------------
LoadImg("ok.png",tmpImg);
Icon_Ok16x16=new QPixmap;
*Icon_Ok16x16=tmpImg;

//--------------------------
LoadImg("search.png",tmpImg);
Icon_Search32x32=new QPixmap;
*Icon_Search32x32=tmpImg;
///nicht vergessen: die müssen im Destr. auch wieder gelöscht werden!///


// Signal-Slot Connections //
connect(&ClipboardTimer, SIGNAL(timeout()), this, SLOT(OnClipboardTimerEvent()));
connect(GroupView,SIGNAL(collapsed(QListViewItem*)),this, SLOT(OnGroupItemCollapsed(QListViewItem*)));
connect(GroupView,SIGNAL(expanded(QListViewItem*)),this, SLOT(OnGroupItemExpanded(QListViewItem*)));


// MainWnd //
CurrentGroup=NULL;
Clipboard=QApplication::clipboard();
GroupView->setSorting(-1);

if(config.Toolbar){
	View_ShowToolBar->setOn(true);}
else{	Toolbar->hide();
	View_ShowToolBar->setOn(false);}

if(config.EntryDetails){
	View_ShowEntryDetails->setOn(true);}
else{	SummaryField->hide();
	View_ShowEntryDetails->setOn(false);}


if(config.Columns[0]){
	View_Column_Title->setOn(true);}
else{
	View_Column_Title->setOn(false);}

if(config.Columns[1]){
	View_Column_Username->setOn(true);}
else{
	View_Column_Username->setOn(false);}

if(config.Columns[2]){
	View_Column_URL->setOn(true);}
else{
	View_Column_URL->setOn(false);}

if(config.Columns[3]){
	View_Column_Password->setOn(true);}
else{
	View_Column_Password->setOn(false);}

if(config.Columns[4]){
	View_Column_Comment->setOn(true);}
else{
	View_Column_Comment->setOn(false);}

if(config.Columns[5]){
	View_Column_Expire->setOn(true);}
else{
	View_Column_Expire->setOn(false);}

if(config.Columns[6]){
	View_Column_Creation->setOn(true);}
else{
	View_Column_Creation->setOn(false);}

if(config.Columns[7]){
	View_Column_LastMod->setOn(true);}
else{
	View_Column_LastMod->setOn(false);}

if(config.Columns[8]){
	View_Column_LastAccess->setOn(true);}
else{
	View_Column_LastAccess->setOn(false);}

if(config.Columns[9]){
	View_Column_Attachment->setOn(true);}
else{
	View_Column_Attachment->setOn(false);}

if(config.ListView_HidePasswords){
	View_HidePasswords->setOn(true);}
else{
	View_HidePasswords->setOn(false);}

if(config.ListView_HideUsernames){
	View_HideUsernames->setOn(true);}
else{
	View_HideUsernames->setOn(false);}

////////////////////////////////////////////////
GroupView->addColumn(trUtf8("Gruppen"));
GroupView->setColumnWidth(0,GroupView->width()-4);
SetupColumns();
InitMenus();

if(ArgFile==""){
 if(config.OpenLast && config.LastFile!="")
  {QFileInfo file(config.LastFile);
   if(file.exists() && file.isFile())OpenDatabase(config.LastFile);
   else config.LastFile="";}
}
else
{
  QFileInfo file(ArgFile);
  if(file.exists() && file.isFile())OpenDatabase(ArgFile);
   else cout << "file not found "<< ArgFile << endl;
}


}

void CMainWindow::LoadImg(QString name,QImage& tmpImg){
if(tmpImg.load(appdir+"/../share/keepass/"+name)==false){
 if(tmpImg.load(appdir+"/share/"+name)==false){
 QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei '%1' konnte nicht gefunden werden.")
				   .arg(name),trUtf8("OK"),0,0,2,1);
 exit(-1);
}}

}


CMainWindow::~CMainWindow()
{
OnClose();
if(!config.saveToIni(IniFilename))
	QMessageBox::warning(this,tr("Warnung"),trUtf8("Die Konfigurationsdatei konnte nicht gespeichert werden.Stellen Sie sicher, dass\nSie Schreibrechte im Verzeichnis ~/.keepass besitzen."),tr("OK"),"","",0.0);
if(translator)delete translator;
delete [] EntryIcons;
delete Icon_Key32x32;
delete Icon_Settings32x32;
delete Icon_Search32x32;
delete Icon_I18n32x32;
delete Icon_Ok16x16;
}

void CMainWindow::OnClose()
{
if(FileOpen) CloseDataBase();
parentWidget()->close();
}

void CMainWindow::OnMainWinResize()
{

}

void CMainWindow::OnFileOpen(){
QString filename=QFileDialog::getOpenFileName(QDir::homeDirPath(),"*.kdb",this,trUtf8("Datenbank öffnen"));
if(filename=="")return;
OpenDatabase(filename);
}


void CMainWindow::OpenDatabase(QString filename)
{
if(FileOpen==true) CloseDataBase();
CPasswordDialog dlg(this,0,true);
dlg.setCaption(filename);
dlg.exec();

if(dlg.canceled)return;

db = new PwDatabase();

if(dlg.password!="" && dlg.keyfile==""){
db->CalcMasterKeyByPassword(dlg.password);
}
if(dlg.password=="" && dlg.keyfile!=""){
db->CalcMasterKeyByFile(dlg.keyfile);
}
if(dlg.password!="" && dlg.keyfile!=""){
db->CalcMasterKeyByFileAndPw(dlg.keyfile,dlg.password);
}
QString err;
if(db->loadDatabase(filename,err)==true){
updateGroupView();
SetFileMenuState(STATE_FileOpen);
setModFlag(false);
parentWidget()->setCaption(tr("Keepass - %1").arg(filename));
FileOpen=true;
}
else{
///@FIXME MessageBox erscheint im Hintergrund
//mgl parent MainWindow ist noch nicht offen
//mgl parent CPasswordDlg ist schon zu
//tritt nur mit libqtkde.so auf
delete db;
FileOpen=false;
QMessageBox::critical(NULL,trUtf8("Fehler"),trUtf8("Beim öffnen der Datenbank ist ein Fehler aufgetreten:\n%1")
				 .arg(err),trUtf8("OK"),0,0);
}
}

void CMainWindow::updateGroupView(){
GroupView->clear();
GroupItems.clear();
EntryView->clear();
EntryItems.clear();

//Level==0 -> Parent=GroupView
//Level>lastLevel -> Parent=lastItem
//Level<=lastLevel -> Parent=last higher Item(reverse)

for(GroupItr i=db->Groups.begin();i!=db->Groups.end();i++){
if((*i).Level==0){
 if(GroupItems.size())GroupItems.push_back(new GroupItem(&(*i),GroupView,getLastSameLevelItem(0),(*i).Name));
 else GroupItems.push_back(new GroupItem(&(*i),GroupView,(*i).Name));
}
 else{
 if((*i).Level>(*(i-1)).Level){
 GroupItems.push_back(new GroupItem(&(*i),(*(GroupItems.end()-1)),getLastSameLevelItem((*i).Level),(*i).Name));
 }
 if((*i).Level<=(*(i-1)).Level){
   GroupItemItr j;
   for(j=GroupItems.end()-1;j!=GroupItems.begin();j--){
    if((*j)->pGroup->Level<(*i).Level)break;}
     GroupItems.push_back(new GroupItem(&(*i),(*j),getLastSameLevelItem((*i).Level),(*i).Name));
     }
 }
(*(GroupItems.end()-1))->setPixmap(0,EntryIcons[(*i).ImageID]);
}

for(int i=0;i<GroupItems.size();i++){
 GroupView->setOpen(GroupItems[i],GroupItems[i]->pGroup->UI_ItemIsExpanded);
}

}


void CMainWindow::OnGroupChanged(QListViewItem* item){
if(item){
SetEditMenuState(STATE_SingleGroupSelected);
setCurrentGroup((GroupItem*)item);
}
else SetEditMenuState(STATE_NoGroupSelected);
SummaryField->setText("");
SetEditMenuState(STATE_NoEntrySelected);
}

void CMainWindow::setCurrentGroup(GroupItem* item){
CurrentGroup=item;
updateEntryView();
}

void CMainWindow::SetupColumns(){
int NumColumns=EntryView->columns();
for(int i=0;i<NumColumns;i++)EntryView->removeColumn(0);
int columnID[10];
int i=0;
if(config.Columns[0]){
columnID[0]=EntryView->addColumn(trUtf8("Titel"),0); i++;}
if(config.Columns[1]){
columnID[1]=EntryView->addColumn(trUtf8("Benutzername"),0); i++;}
if(config.Columns[2]){
columnID[2]=EntryView->addColumn(trUtf8("URL"),0); i++;}
if(config.Columns[3]){
columnID[3]=EntryView->addColumn(trUtf8("Passwort"),0); i++;}
if(config.Columns[4]){
columnID[4]=EntryView->addColumn(trUtf8("Kommentare"),0); i++;}
if(config.Columns[5]){
columnID[5]=EntryView->addColumn(trUtf8("Gültig bis"),0); i++;}
if(config.Columns[6]){
columnID[6]=EntryView->addColumn(trUtf8("Erstellung"),0); i++;}
if(config.Columns[7]){
columnID[7]=EntryView->addColumn(trUtf8("letzte Änderung"),0); i++;}
if(config.Columns[8]){
columnID[8]=EntryView->addColumn(trUtf8("letzter Zugriff"),0); i++;}
if(config.Columns[9]){
columnID[9]=EntryView->addColumn(trUtf8("Anhang"),0); i++;}
ResizeColumns();

}

void CMainWindow::ResizeColumns(){
int cols=EntryView->columns();
int width=(EntryView->width()-4)/cols; //Ränder mit je 2 Px Breite???
for(int i=0;i<cols;i++){
EntryView->setColumnWidth(i,width);
}
}


void CMainWindow::updateEntryView(){
//  Achtung:
//  Die ->pEntry bzw ->pGroup Zeiger sind zu diesem Zeitpunkt ungültig!
EntryView->clear();
EntryItems.clear();
if(!CurrentGroup)return;
EntryItem *tmp=NULL;
for(int i=0;i<db->Entries.size();i++){
CEntry* entry=&db->Entries[i];
if((CurrentGroup->pGroup->ID==entry->GroupID) || (CurrentGroup->pGroup->ID==db->SearchGroupID && isInSearchResults(entry))){
EntryItems.push_back(tmp=new EntryItem(entry,EntryView));
int j=0;
if(config.Columns[0]){
tmp->setText(j++,entry->Title);}
if(config.Columns[1]){
  if(config.ListView_HideUsernames)
    tmp->setText(j++,"******");
  else
    tmp->setText(j++,entry->UserName);}
if(config.Columns[2]){
tmp->setText(j++,entry->URL);}
if(config.Columns[3]){
  if(config.ListView_HidePasswords)
    tmp->setText(j++,"******");
  else{
    tmp->setText(j++,entry->Password.getString());
    entry->Password.delRef();}}
if(config.Columns[4]){
tmp->setText(j++,entry->Additional.section('\n',0,0));}
if(config.Columns[5]){
tmp->setText(j++,entry->Expire.GetString(0));}
if(config.Columns[6]){
tmp->setText(j++,entry->Creation.GetString(0));}
if(config.Columns[7]){
tmp->setText(j++,entry->LastMod.GetString(0));}
if(config.Columns[8]){
tmp->setText(j++,entry->LastAccess.GetString(0));}
if(config.Columns[9]){
tmp->setText(j++,entry->BinaryDesc);}
  (*(EntryItems.end()-1))->setPixmap(0,EntryIcons[entry->ImageID]);
  }
}
}

void CMainWindow::OnEntryChanged(QListViewItem* item){
if(item)SetEditMenuState(STATE_SingleEntrySelected);
else SetEditMenuState(STATE_NoEntrySelected);
updateEntryDetails((EntryItem*)item);
}


void CMainWindow::showEvent(QShowEvent *event){
if(event->spontaneous()==false){
SetEditMenuState(STATE_NoGroupSelected);
QValueList<int> s;
s.push_back(25); s.push_back(100);
parentWidget()->resize(750,450);
splitter->setSizes(s);



////////////////////////////////////
/*Beim öffnen der Datenbank ist ein Fehler aufgetreten.
if(db->IsOpen==true) CloseDataBase();
QString filename="/home/tarek/Desktop/wallet.xml";
Import_KWalletXml importer;
QString err;
if(!importer.importFile(filename,&pwmanager,err)){
 cout << err << endl;
 return;
}
cout << "Erfolg" << endl;
updateGroupView();
SetFileMenuState(STATE_FileOpen);
*/
////////////////////////////////////
}
}

void CMainWindow::OnEntryDoubleClicked(QListViewItem* item,const QPoint& Point, int column){
if(item && (column==0)){
OnEditEntry();
}
}


void CMainWindow::CreateBanner(QLabel *Banner,QPixmap* symbol,QString text){
CreateBanner(Banner,symbol,text,config.BannerColor1
			       ,config.BannerColor2
			       ,config.BannerTextColor); //überladene Funktion wird aufgerufen
}

void CMainWindow::CreateBanner(QLabel *Banner,QPixmap* symbol,QString text,QColor color1,QColor color2,QColor textcolor){
int w=Banner->width();
int h=Banner->height();
QColor color;
float b1[3];
float b2[3];
float a1,a2;
QPixmap* banner_pixmap=new QPixmap(w,h); ///@FIXME löscht der Destruktor von QLabel die Pixmap zum schluss???
QPainter painter(banner_pixmap,24);
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
///@FIXME Farbtiefe < 24 Bit (Dithering?)
for(int x=0;x<w;x++){
a2=(float)x/(float)w;
a1=1-a2;
color.setRgb(	(int)(a1*b1[0]+a2*b2[0]),
		(int)(a1*b1[1]+a2*b2[1]),
		(int)(a1*b1[2]+a2*b2[2]));
pen.setColor(color);
painter.setPen(pen);
painter.drawLine(x,0,x,h);
}}
else{
banner_pixmap->fill(color1);
}

painter.drawPixmap(10,10,*symbol);
pen.setColor(textcolor);
painter.setPen(pen);
painter.drawText(50,30,text);
Banner->setPixmap(*banner_pixmap);
}


void CMainWindow::CloseDataBase(){
if(modflag){
int r=QMessageBox::question(this,trUtf8("Datenbank speichern?"),trUtf8("Soll die aktuelle Datenbank vor dem Schließen gespeichert werden?")
		      ,trUtf8("Ja"),trUtf8("Nein"),trUtf8("Abbrechen"),2,2);
if(r==2)return;
if(r==0)OnFileSave();}
SummaryField->setText("");
SetEditMenuState(STATE_NoEntrySelected);
SetEditMenuState(STATE_NoGroupSelected);
EntryView->clear();
EntryItems.clear();
GroupView->clear();
GroupItems.clear();
config.LastFile=db->filename;
db->CloseDataBase();
delete db;
FileOpen=false;
}

void CMainWindow::OnMenu_closeDB()
{
CloseDataBase();
SetFileMenuState(STATE_NoFileOpen);
SetEditMenuState(STATE_NoEntrySelected);
SetEditMenuState(STATE_NoGroupSelected);
parentWidget()->setCaption(tr("Keepass Passwortsafe"));
}

void CMainWindow::OnFileSave()
{
if(db->filename==""){
OnFileSaveAs();
return;
}
if(db->SaveDataBase(db->filename))setModFlag(false);
}

void CMainWindow::OnFileSaveAs()
{
QString filename=QFileDialog::getSaveFileName(QDir::homeDirPath(),"*.kdb",this,trUtf8("Datenbank öffnen"));
if(filename=="")return;
db->filename=filename;
config.LastFile=filename;
db->SaveDataBase(filename);
setModFlag(false);
parentWidget()->setCaption(tr("Keepass - %1").arg(filename));
}

void CMainWindow::InitMenus(){
SetEditMenuState(STATE_NoEntrySelected);
SetEditMenuState(STATE_NoGroupSelected);
SetFileMenuState(STATE_NoFileOpen);
}


void CMainWindow::SetFileMenuState(FileMenuState status){
switch(status){
  case STATE_FileOpen:
    File_Save->setEnabled(true);
    File_SaveAs->setEnabled(true);
    File_Close->setEnabled(true);
    File_ChangeKey->setEnabled(true);
    File_Settings->setEnabled(true);
    Edit_NewGroup->setEnabled(true);
    Edit_GlobalSearch->setEnabled(true);
    toolButtonSave->setEnabled(true);
    toolButtonAddEntry->setEnabled(true);
    GroupView->setEnabled(true);
    EntryView->setEnabled(true);
    SummaryField->setEnabled(true);
    break;
  case STATE_NoFileOpen:
    File_Save->setDisabled(true);
    File_SaveAs->setDisabled(true);
    File_Close->setDisabled(true);
    File_ChangeKey->setDisabled(true);
    File_Settings->setDisabled(true);
    Edit_NewGroup->setDisabled(true);
    Edit_GlobalSearch->setDisabled(true);
    toolButtonSave->setDisabled(true);
    toolButtonAddEntry->setEnabled(false);
    GroupView->setDisabled(true);
    EntryView->setDisabled(true);
    SummaryField->setDisabled(true);
    break;
}
}

void CMainWindow::SetEditMenuState(EditMenuState status){
switch(status){
  case STATE_NoGroupSelected:
    Edit_NewSubGroup->setDisabled(true);
    Edit_GroupProperties->setDisabled(true);
    Edit_DeleteGroup->setDisabled(true);
    Edit_AddEntry->setDisabled(true);
    Edit_GroupSearch->setDisabled(true);
    toolButtonAddEntry->setEnabled(false);
    break;
  case STATE_NoEntrySelected:
    Edit_UserNameToClipboard->setDisabled(true);
    Edit_PasswordToClipboard->setDisabled(true);
    Edit_SaveAttachment->setDisabled(true);
    Edit_OpenURL->setDisabled(true);
    Edit_DeleteEntry->setDisabled(true);
    Edit_EditEntry->setDisabled(true);
    Edit_CopyEntry->setDisabled(true);
    toolButtonEditEntry->setDisabled(true);
    toolButtonDeleteEntry->setDisabled(true);
    toolButtonUserNameToClipboard->setDisabled(true);
    toolButtonPasswordToClipboard->setDisabled(true);
    break;
  case STATE_SingleGroupSelected:
    Edit_NewSubGroup->setEnabled(true);
    Edit_GroupProperties->setEnabled(true);
    Edit_DeleteGroup->setEnabled(true);
    Edit_AddEntry->setEnabled(true);
    Edit_GroupSearch->setEnabled(true);
    toolButtonAddEntry->setEnabled(true);
    break;
  case STATE_SingleEntrySelected:
    toolButtonEditEntry->setDisabled(false);
    toolButtonDeleteEntry->setDisabled(false);
    toolButtonUserNameToClipboard->setDisabled(false);
    toolButtonPasswordToClipboard->setDisabled(false);
    Edit_UserNameToClipboard->setEnabled(true);
    Edit_PasswordToClipboard->setEnabled(true);
    Edit_SaveAttachment->setEnabled(true);
    Edit_OpenURL->setEnabled(true);
    Edit_DeleteEntry->setEnabled(true);
    Edit_EditEntry->setEnabled(true);
    Edit_CopyEntry->setEnabled(true);
    break;
}
}

void CMainWindow::OnMenuExit()
{
OnClose();
}

void CMainWindow::OnAddGroup()
{
CEditGroupDialog dlg(this,trUtf8("Gruppeneigenschaften"),true,NULL);
dlg.exec();
if(dlg.OK==false)return;
CGroup* NewGroup=db->addGroup(NULL);
NewGroup->Name=dlg.GroupName;
NewGroup->ImageID=dlg.IconID;
updateGroupView();
setModFlag(true);
}


void CMainWindow::OnAddSubGroup()
{
CEditGroupDialog dlg(this,trUtf8("Gruppeneigenschaften"),true,NULL);
dlg.exec();
if(dlg.OK==false)return;
CGroup* NewGroup=db->addGroup(CurrentGroup->pGroup);
NewGroup->Name=dlg.GroupName;
NewGroup->ImageID=dlg.IconID;
//GroupItems.push_back(new GroupItem(NewGroup,CurrentGroup,NewGroup->Name));
updateGroupView();
setModFlag(true);
}

void CMainWindow::OnEditGroup()
{
GroupItem* item=CurrentGroup;
CGroup& group=*item->pGroup;
CEditGroupDialog dlg(this,trUtf8("Gruppeneigenschaften"),true,NULL);
dlg.GroupName=group.Name;
dlg.IconID=group.ImageID;
dlg.exec();
if(dlg.OK==false)return;
group.Name=dlg.GroupName;
group.ImageID=dlg.IconID;
item->setText(0,group.Name);
item->setPixmap(0,EntryIcons[group.ImageID]);
setModFlag(true);
}



void CMainWindow::OnDeleteGroup()
{
db->deleteGroup(CurrentGroup->pGroup);
CurrentGroup=NULL;
SetEditMenuState(STATE_NoGroupSelected);
updateGroupView();
updateEntryView();
setModFlag(true);
}


void CMainWindow::OnPasswordToClipboard()
{
Clipboard->setText(currentEntry()->Password.getString(),QClipboard::Clipboard);
ClipboardTimer.start(config.ClipboardTimeOut*1000,true);
currentEntry()->Password.delRef();
}

void CMainWindow::OnUserNameToClipboard()
{
Clipboard->setText(currentEntry()->UserName,  QClipboard::Clipboard);
ClipboardTimer.start(config.ClipboardTimeOut*1000,true);
}

void CMainWindow::OnClipboardTimerEvent(){
Clipboard->clear(QClipboard::Clipboard); //löscht nicht den KDE-Klipper
}

void CMainWindow::OnOpenURL()
{
OpenURL(currentEntry()->URL);
}

void CMainWindow::OpenURL(QString url){
QProcess browser;
browser.setArguments(QStringList::split(' ',config.OpenUrlCommand.arg(url)));
browser.start();
}

void CMainWindow::OnSaveAttachment()
{
CEntry& entry=*currentEntry();
if(entry.BinaryDataLength==0){
QMessageBox::information(NULL,trUtf8("Hinweis"),trUtf8("Dieser Eintrag hat keinen Dateianhang."),"OK");
return;
}
QString filename=QFileDialog::getSaveFileName(QDir::homeDirPath(),"",this,trUtf8("Anhang speichern..."));
if(filename=="")return;
QFile file(filename);
if(file.exists()){
int r=QMessageBox::warning(this,trUtf8("Vorhandene Datei überschreiben?"),trUtf8("Unter dem gewählten Dateinamen existiert bereits eine Datei.\nSoll sie überschrieben werden?"),trUtf8("Ja"),trUtf8("Nein"),NULL,1,1);
if(r==1)return;
if(file.remove()==false){
QMessageBox::critical(NULL,trUtf8("Fehler"),trUtf8("Datei konnte nicht überschrieben werden."),trUtf8("OK"));
return;}
}
if(file.open(IO_WriteOnly)==false){
QMessageBox::critical(NULL,trUtf8("Fehler"),trUtf8("Datei konnte nicht erstellt werden."),trUtf8("OK"));
return;
}

int r=file.writeBlock((char*)entry.pBinaryData,entry.BinaryDataLength);
if(r==-1){
file.close();
QMessageBox::critical(NULL,trUtf8("Fehler"),trUtf8("Beim schreiben in der Datei ist ein Fehler aufgetreten."),trUtf8("OK"));
return;
}
if(r!=entry.BinaryDataLength){
file.close();
QMessageBox::critical(NULL,trUtf8("Fehler"),trUtf8("Die Datei konnte nicht vollständig geschrieben werden."),trUtf8("OK"));
return;
}
file.close();
}

void CMainWindow::OnAddEntry()
{
CEntry entry;
entry.ImageID=0;
entry.GroupID=CurrentGroup->pGroup->ID;
entry.Creation.SetToNow();
entry.LastMod.SetToNow();
entry.LastAccess.SetToNow();
entry.Expire.Set(28,12,2999,0,0,0);
entry.BinaryDataLength=0;
entry.pBinaryData=NULL;
entry.PasswordLength=0;

CEditEntryDlg* pDlg= new CEditEntryDlg(this,0,true);
pDlg->entry=&entry;
if(pDlg->exec()){
 if(db->Entries.size()==0){
 entry.sID=0;
 getRandomBytes(&entry.ID,16,1,false);
 }
 else {
 entry.sID=(*(db->Entries.end()-1)).sID+1;
 while(1){
 bool used=false;
 getRandomBytes(&entry.ID,16,1,false);
  for(int j=0;j<db->Entries.size();j++){
	int k;
	for(k=0;k<16;k++){if(db->Entries[j].ID[k]!=entry.ID[k])k=0;break;}
	if(k==15)used=true;}
 if(used==false)break;
 }}
 db->Entries.push_back(entry);
 updateEntryView();
 setModFlag(true);
}

}

void CMainWindow::OnEditEntry()
{
CEditEntryDlg* pDlg= new CEditEntryDlg(this,0,true);
pDlg->entry=currentEntry();
pDlg->exec();
updateEntryView();
if(pDlg->ModFlag)setModFlag(true);
}

void CMainWindow::OnCopyEntry()
{
CEntry &src=*currentEntry();
CEntry entry=src;
entry.sID=(*(db->Entries.end()-1)).sID+1;
while(1){
bool used=false;
getRandomBytes(&entry.ID,16,1,false);
  for(int j=0;j<db->Entries.size();j++){
	int k;
	for(k=0;k<16;k++){if(db->Entries[j].ID[k]!=entry.ID[k])k=0;break;}
	if(k==15)used=true;}
if(used==false)break;
}
entry.Creation.SetToNow();
entry.LastMod.SetToNow();
entry.LastAccess.SetToNow();
entry.Expire.Set(28,12,2999,0,0,0);
db->Entries.push_back(entry);
if(entry.BinaryDataLength>0){
CEntry &new_entry=(*(db->Entries.end()-1));
new_entry.pBinaryData=new unsigned char[new_entry.BinaryDataLength];
memcpy(new_entry.pBinaryData,src.pBinaryData,new_entry.BinaryDataLength);
}
updateEntryView();
setModFlag(true);
}

void CMainWindow::OnDeleteEntry()
{
db->deleteEntry(currentEntry());
updateEntryView();
setModFlag(true);
}

void CMainWindow::OnGlobalSearch()
{
Search();
}

void CMainWindow::OnGroupSearch()
{
Search(CurrentGroup->pGroup);
}

void CMainWindow::Search(CGroup* pGroup){
CSearchDlg dlg(pGroup,this,"SearchDialog",true);
if(!dlg.exec())return;
if(!dlg.hits.size()){
 QMessageBox::information(this,tr("Suche erfolglos"),trUtf8("Die Suche lieferte keine Ergebnisse."),trUtf8("OK"),0,0,2,1);
 return;}
if(db->SearchGroupID!=-1){
 db->deleteGroup(db->SearchGroupID);
 SearchResults.clear();
 db->SearchGroupID=-1;
}
CGroup* group=db->addGroup(NULL);
db->SearchGroupID=group->ID;
group->Name=tr("Suchergebnisse");
SearchResults.clear();
for(int i=0;i<dlg.hits.size();i++){
 SearchResults.push_back(dlg.hits[i]->sID);
}
updateGroupView();

}

void CMainWindow::OnEntryRightClicked(QListViewItem* item, const QPoint& pos,int column)
{
if(!FileOpen)return;
if(!CurrentGroup) return;
QPopupMenu menu;
connect(&menu, SIGNAL(activated(int)), this, SLOT(OnEntryCtxMenuClicked(int)));
menu.insertItem(trUtf8("Passwort in Zwischenablage kopieren"),1);
menu.insertItem(trUtf8("Benutzername in Zwischenablage kopieren"),2);
menu.insertItem(trUtf8("URL öffnen"),3);
menu.insertItem(trUtf8("Anhang speichern..."),4);
menu.insertSeparator();
menu.insertItem(trUtf8("Eintrag hinzufügen..."),5);
menu.insertItem(trUtf8("Eintrag anzeigen/bearbeiten"),6);
menu.insertItem(trUtf8("Eintrag duplizieren"),7);
menu.insertItem(trUtf8("Eintrag löschen"),8);

if(column==-1){
menu.setItemEnabled(1,false);
menu.setItemEnabled(2,false);
menu.setItemEnabled(3,false);
menu.setItemEnabled(4,false);
menu.setItemEnabled(6,false);
menu.setItemEnabled(7,false);
menu.setItemEnabled(8,false);}

if(CurrentGroup->pGroup->ID==db->SearchGroupID){
menu.setItemEnabled(5,false);
menu.setItemEnabled(7,false);
}

menu.exec(pos);

}

void CMainWindow::OnEntryCtxMenuClicked(int id){
switch(id){
case 1:OnPasswordToClipboard();
break;
case 2:OnUserNameToClipboard();
break;
case 3:OnOpenURL();
break;
case 4:OnSaveAttachment();
break;
case 5:OnAddEntry();
break;
case 6:OnEditEntry();
break;
case 7:OnCopyEntry();
break;
case 8:OnDeleteEntry();
break;
}
}

void CMainWindow::OnGroupRightClicked(QListViewItem* item, const QPoint& pos, int column)
{
if(!FileOpen)return;

QPopupMenu menu;
if(column!=-1){
 if(((GroupItem*)item)->pGroup->ID!=db->SearchGroupID){
  connect(&menu, SIGNAL(activated(int)), this, SLOT(OnGroupCtxMenuClicked(int)));
  //menu.insertItem(trUtf8("Gruppe hinzufügen..."),1);
  menu.insertItem(trUtf8("Untergruppe hinzufügen..."),2);
  menu.insertSeparator();
  menu.insertItem(trUtf8("Gruppe löschen"),3);
  menu.insertItem(trUtf8("Eigenschaften"),4);}
 else{
  connect(&menu, SIGNAL(activated(int)), this, SLOT(OnSearchGroupCtxMenuClicked(int)));
  menu.insertItem(trUtf8("Ausblenden"),1);}
}
else
{
connect(&menu, SIGNAL(activated(int)), this, SLOT(OnGroupCtxMenuClicked(int)));
  menu.insertItem(trUtf8("Gruppe hinzufügen..."),1);
}
menu.exec(pos);
}

void CMainWindow::OnSearchGroupCtxMenuClicked(int id){
/*if(db->SearchGroupID!=-1){
 db->DeleteGroup(db->GetGroupIterator(db->SearchGroupID));
 SearchResults.clear();
 delete GetGroupItem(db->SearchGroupID);
 DeleteGroupAssoc(db->SearchGroupID);
 db->SearchGroupID=-1;
 SetEditMenuState(STATE_NoGroupSelected);
 RefreshEntryView();
}*/
}

void CMainWindow::OnGroupCtxMenuClicked(int id){
switch(id){
case 1:OnAddGroup();
break;
case 2:OnAddSubGroup();
break;
case 3:OnDeleteGroup();
break;
case 4:OnEditGroup();
break;
}
}

void CMainWindow::OnDbSettings()
{
CDbSettingsDlg dlg(this,db,trUtf8("Einstellungen"),true);
dlg.exec();
}

void CMainWindow::OnChangeDbKey()
{
CChangeKeyDlg dlg(this,db);
dlg.exec();
setModFlag(true);
}

void CMainWindow::OnFileNew()
{
if(FileOpen==true) CloseDataBase();
db=new PwDatabase();
db->NewDataBase();
CChangeKeyDlg dlg(this,db);
if(dlg.exec()==1){
updateGroupView();
SetFileMenuState(STATE_FileOpen);
setModFlag(true);
FileOpen=true;
}
else delete db;
}

void CMainWindow::OnViewToolbarToggled(bool toggled)
{
config.Toolbar=toggled;
if(toggled){
Toolbar->show();
}
else
{
Toolbar->hide();
}
}

void CMainWindow::OnViewEntryDetailsToggled(bool toggled)
{
config.EntryDetails=toggled;
if(toggled){
SummaryField->show();
}
else
{
SummaryField->hide();
}
}

void CMainWindow::OnSettings()
{
CSettingsDlg dlg(this,0,true);
dlg.exec();

}

void CMainWindow::OnHelpAbout()
{
CAboutDialog* pDlg= new CAboutDialog(this,0,true);
pDlg->show();

}

void CMainWindow::OnView_ColumnExpireToggled(bool value)
{
config.Columns[5]=value;
SetupColumns();
updateEntryView();

}

void CMainWindow::OnView_ColumnAttachmentToggled(bool value)
{
config.Columns[9]=value;
SetupColumns();
updateEntryView();
}

void CMainWindow::OnView_ColumnUrlToggled(bool value)
{
config.Columns[2]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnTitleToggled(bool value)
{
config.Columns[0]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnCreationToggled(bool value)
{
config.Columns[6]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnLastAccessToggled(bool value)
{
config.Columns[8]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnLastModToggled(bool value)
{
config.Columns[7]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnCommentToggled(bool value)
{
config.Columns[4]=value;
SetupColumns();
updateEntryView();
}


void CMainWindow::OnView_ColumnPasswordToggled(bool value)
{
config.Columns[3]=value;
SetupColumns();
updateEntryView();
}

void CMainWindow::OnView_ColumnUsernameToggled(bool value)
{
config.Columns[1]=value;
SetupColumns();
updateEntryView();
}

void CMainWindow::OnExtrasLanguage()
{
CLanguageDlg dlg(this,0,true);
dlg.exec();
}

bool CMainWindow::isInSearchResults(CEntry* pEntry){
for(int i=0; i<SearchResults.size();i++){
 if(pEntry->sID==SearchResults[i])return true;}
return false;
}

void CMainWindow::OnQickSearch()
{
QMessageBox::information(this,trUtf8("Platzhalter"),trUtf8("< noch nicht implementiert >"),trUtf8("OK"),0,0,2,1);
}

void CMainWindow::setModFlag(bool flag){
modflag=flag;
toolButtonSave->setEnabled(flag);
}

void CMainWindow::OnGroupItemExpanded(QListViewItem* item){
((GroupItem*)item)->pGroup->UI_ItemIsExpanded=true;
}

void CMainWindow::OnGroupItemCollapsed(QListViewItem* item){
((GroupItem*)item)->pGroup->UI_ItemIsExpanded=false;
}

void CMainWindow::DEBUG_OnPrintDbStucture()
{

if(!FileOpen){cout << "db->IsOpen=false" << endl; return;}

cout << "NumEntries=" << db->Entries.size() << endl;
cout << "NumGroups=" << db->Groups.size() << endl;
cout << "------------------Group Vector-----------------" << endl;
if(db->Groups.size()){
//cout << "Start: 0x" << QString::number((unsigned int)&db->Groups.front(),16) << endl;
//cout << "End: 0x" << QString::number((unsigned int)&db->Groups.back(),16) << endl;
for(int i=0;i<db->Groups.size();i++){
cout << QString("(+%1) '%2', Level=%3, ID=%4").arg(i).arg(db->Groups[i].Name).arg(db->Groups[i].Level).arg(db->Groups[i].ID)
     << endl;
}
}
cout << "------------------Entry Vector-----------------" << endl;
if(db->Entries.size()){
//cout << "Start: 0x" << QString::number((unsigned int)&db->Entries.front(),16) << endl;
//cout << "End: 0x" << QString::number((unsigned int)&db->Entries.back(),16) << endl;
for(int i=0;i<db->Entries.size();i++){
QString groupname="???";
 for(int j=0;j<db->Groups.size();j++){
  if(db->Groups[j].ID==db->Entries[i].GroupID){
	groupname="'"+db->Groups[j].Name+"'";
	break;}
}
cout << QString("(+%1) '%2', GroupID=%3 [%4], sID=%5").arg(i).arg(db->Entries[i].Title).arg(db->Entries[i].GroupID).arg(groupname).arg(db->Entries[i].sID)
     << endl;
}
}
cout << endl;

}

GroupItem* CMainWindow::getLastSameLevelItem(int level){
for(int i=GroupItems.size()-1;i>=0;i--){
 if(GroupItems[i]->pGroup->Level==level){
return GroupItems[i];}
}
return GroupItems.back();

}

void CMainWindow::OnImportPwManagerFile()
{
bool merge=false;
if(FileOpen){
int ret=QMessageBox::question(this,tr("Frage"),
	trUtf8("Es ist noch eine andere Datenbank geöffnet. Soll sie mit der zu importierenden Datenbank zusammengeführt oder geschlossen werden werden?"),
	trUtf8("Zusammenführen"),trUtf8("Schließen"),tr("Abbrechen"),1,2);
switch(ret){
	case 0: merge=true;
		   break;
	case 1: CloseDataBase();
		   break;
	case 2: return;
		}
}
QString filename=QFileDialog::getOpenFileName(QDir::homeDirPath(),"*.pwm",this,trUtf8("Datenbank importieren"));
if(filename=="")return;
CSimplePasswordDialog dlg(!config.ShowPasswords,this,"SimplePasswordDlg",true);
if(!dlg.exec())return;
Import_PwManager importer;
QString err;

if(merge){
  PwDatabase* ImportDb=new PwDatabase();
  if(!importer.importFile(filename,dlg.password,ImportDb,err)){
	delete ImportDb;
  	QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei konnte nicht importiert werden.\n%1").arg(err),trUtf8("OK"),0,0,2,1);
  	return;}
  db->merge(ImportDb);
  delete ImportDb;
}
else{
  db=new PwDatabase();
  if(!importer.importFile(filename,dlg.password,db,err)){
	delete db;
  	QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei konnte nicht importiert werden.\n%1").arg(err),trUtf8("OK"),0,0,2,1);
  	return;}
  CChangeKeyDlg dlg(this,db);
  if(dlg.exec()!=1){
	delete db;
	return;
  }
}
updateGroupView();
SetFileMenuState(STATE_FileOpen);
FileOpen=true;
}

void CMainWindow::OnView_HideUsernamesToggled(bool state)
{
config.ListView_HideUsernames=state;
updateEntryView();
updateEntryDetails();
}


void CMainWindow::OnView_HidePasswordsToggled(bool state)
{
config.ListView_HidePasswords=state;
updateEntryView();
updateEntryDetails();
}

void CMainWindow::OnGroupViewDrop(QDropEvent* e)
{
}

void CMainWindow::OnHelpAboutQt()
{
QMessageBox::aboutQt(this);
}

void CMainWindow::OnImportKWalletXML()
{
bool merge=false;
if(FileOpen){
int ret=QMessageBox::question(this,tr("Frage"),
	trUtf8("Es ist noch eine andere Datenbank geöffnet. Soll sie mit der zu importierenden Datenbank zusammengeführt oder geschlossen werden werden?"),
	trUtf8("Zusammenführen"),trUtf8("Schließen"),tr("Abbrechen"),1,2);
switch(ret){
	case 0: merge=true;
		   break;
	case 1: CloseDataBase();
		   break;
	case 2: return;
		}
}
QString filename=QFileDialog::getOpenFileName(QDir::homeDirPath(),"*.xml",this,trUtf8("Datenbank importieren"));
if(filename=="")return;
Import_KWalletXml importer;
QString err;
if(merge){
  PwDatabase* ImportDb=new PwDatabase();
  if(!importer.importFile(filename,ImportDb,err)){
	delete ImportDb;
  	QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei konnte nicht importiert werden.\n%1").arg(err),trUtf8("OK"),0,0,2,1);
  	return;}
  db->merge(ImportDb);
  delete ImportDb;
}
else{
  db=new PwDatabase();
  if(!importer.importFile(filename,db,err)){
	delete db;
  	QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei konnte nicht importiert werden.\n%1").arg(err),trUtf8("OK"),0,0,2,1);
  	return;}
  CChangeKeyDlg dlg(this,db);
  if(dlg.exec()!=1){
	delete db;
	return;
  }
}
updateGroupView();
SetFileMenuState(STATE_FileOpen);
FileOpen=true;

}

void CMainWindow::updateEntryDetails(EntryItem* pItem){
if(pItem==NULL){
 SummaryField->setText("");
 return;}
CEntry& entry=*pItem->pEntry;
QString str=trUtf8("<B>Gruppe: </B>%1  <B>Titel: </B>%2  <B>Benutzername: </B>%3  <B>URL: </B>%4  <B>Passwort: </B>%5  <B>Erstellt: </B>%6  <B>letzte Änderung: </B>%7  <B>letzter Zugriff: </B>%8  <B>gültig bis: </B>%9");
str=str.arg(CurrentGroup->pGroup->Name).arg(entry.Title);

if(!config.ListView_HideUsernames)	str=str.arg(entry.UserName);
else str=str.arg("****");

str=str.arg(entry.URL);

if(!config.ListView_HidePasswords)	str=str.arg(entry.Password.getString());
else	str=str.arg("****");

str=str.arg(entry.Creation.GetString(0))
	  .arg(entry.LastMod.GetString(0))
	  .arg(entry.LastAccess.GetString(0))
	  .arg(entry.Expire.GetString(0));
SummaryField->setText(str);
entry.Password.delRef();
}


void CMainWindow::updateEntryDetails(){
updateEntryDetails( (EntryItem*)EntryView->currentItem() );
}



EntryItem* CMainWindow::currentEntryItem(){
return (EntryItem*)EntryView->currentItem();
}

CEntry* CMainWindow::currentEntry(){
return ((EntryItem*)EntryView->currentItem())->pEntry;
}