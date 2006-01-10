/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek.saidi@arcor.de                                                  *
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

#include "mainwindow.h"

#include <QToolButton>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QImage>
#include <QClipboard>
#include <QApplication>
#include <QColor>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QDropEvent>
#include <QLabel>
#include <QShowEvent>
#include <QWidget>
#include <QFileDialog>

#include "lib/random.h"
#include "lib/IniReader.h"
#include "import/Import_PwManager.h"
#include "import/Import_KWalletXml.h"

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





KeepassMainWindow::KeepassMainWindow(QWidget *parent, Qt::WFlags flags):QMainWindow(parent,flags){
  setupUi(this);  
  setupConnections();
  setupIcons();
  setupToolbar();
  setStateFileOpen(false);
  FileOpen=false;
}

void KeepassMainWindow::setupConnections(){
   connect(FileNewAction, SIGNAL(triggered()), this, SLOT(OnFileNew()));
   connect(FileOpenAction, SIGNAL(triggered()), this, SLOT(OnFileOpen()));
   connect(FileCloseAction, SIGNAL(triggered()), this, SLOT(OnFileClose()));
   connect(FileSaveAction, SIGNAL(triggered()), this, SLOT(OnFileSave()));
   connect(FileSaveAsAction, SIGNAL(triggered()), this, SLOT(OnFileSaveAs()));
   connect(FileSettingsAction, SIGNAL(triggered()), this, SLOT(OnFileSettings()));
   connect(FileChangeKeyAction, SIGNAL(triggered()), this, SLOT(OnFileChangeKey()));
   connect(FileExitAction, SIGNAL(triggered()), this, SLOT(OnFileExit()));
   connect(FileImpPwmAction, SIGNAL(triggered()), this, SLOT(OnImportFromPwm()));
   connect(FileImpKWalletXmlAction, SIGNAL(triggered()), this,SLOT(OnImportFromKWalletXml()));
   connect(GroupView,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,
	   SLOT(OnCurrentGroupChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
   connect(EntryView,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,
	   SLOT(OnEntryItemDoubleClicked(QTreeWidgetItem*,int)));
}

void KeepassMainWindow::setupToolbar(){
toolBar->addAction(FileNewAction);
toolBar->addAction(FileOpenAction);
toolBar->addAction(FileSaveAction);
toolBar->addSeparator();

}

void KeepassMainWindow::setupIcons(){
FileNewAction->setIcon(*Icon_FileNew);
FileOpenAction->setIcon(*Icon_FileOpen);
FileSaveAction->setIcon(*Icon_FileSave);
FileSaveAsAction->setIcon(*Icon_FileSaveAs);
FileCloseAction->setIcon(*Icon_FileClose);
FileExitAction->setIcon(*Icon_Exit);
}


void KeepassMainWindow::openDatabase(QString filename){
Q_ASSERT(!FileOpen);
CPasswordDialog PasswordDlg(this,"Password Dialog",true);
PasswordDlg.setCaption(filename);
if(!PasswordDlg.exec()) return;
db = new PwDatabase();
GroupView->db=db;
EntryView->db=db;
if(PasswordDlg.password!="" && PasswordDlg.keyfile==""){
db->CalcMasterKeyByPassword(PasswordDlg.password);
}
if(PasswordDlg.password=="" && PasswordDlg.keyfile!=""){
db->CalcMasterKeyByFile(PasswordDlg.keyfile);
}
if(PasswordDlg.password!="" && PasswordDlg.keyfile!=""){
db->CalcMasterKeyByFileAndPw(PasswordDlg.keyfile,PasswordDlg.password);
}
QString err;
if(db->loadDatabase(filename,err)==true){
//SUCCESS
setCaption(tr("Keepass - %1").arg(filename));
GroupView->updateItems();
EntryView->updateItems();
EntryView->setCurrentGroup(0);
setStateFileOpen(true);
setStateFileModified(false);
}
else{
//ERROR
delete db;
if(err=="")err=trUtf8("unbekannter Fehler in PwDatabase::loadDatabase()");
QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Beim öffnen der Datenbank ist ein Fehler aufgetreten:\n%1")
				 .arg(err),trUtf8("OK"));
}
}

bool KeepassMainWindow::closeDatabase(){
Q_ASSERT(FileOpen);
Q_ASSERT(db!=NULL);
if(ModFlag){
 int r=QMessageBox::question(this,trUtf8("Geänderte Datei speichern?"),
				  trUtf8("Die aktuell geöffnete Datei wurde verändert. Sollen die Änderungen vor dem Schließen gespeichert werden?"),tr("Ja"),tr("Nein"),tr("Abbrechen"),2,2);
 if(r==2)return false;			//Abbrechen
 if(r==0)				//Ja (Datei speichern)
  if(!OnFileSave())return false;
}
delete db;
db=NULL;
EntryView->db=NULL;
EntryView->clear();
EntryView->Items.clear();
GroupView->db=NULL;
GroupView->clear();
GroupView->Items.clear();
setStateFileOpen(false);
setCaption("Keepass Passwort-Manager");
return true;
}


void KeepassMainWindow::OnFileNew(){
if(FileOpen)
 if(!closeDatabase())return;
db=new PwDatabase();
CChangeKeyDlg dlg(this,db);
if(dlg.exec()==1){
setCaption(tr("Keepass - %1").arg(tr("[neu]")));
GroupView->db=db;
EntryView->db=db;
GroupView->updateItems();
EntryView->updateItems();
EntryView->setCurrentGroup(0);
setStateFileOpen(true);
setStateFileModified(true);
FileOpen=true;
}
else delete db;
}

void KeepassMainWindow::OnFileOpen(){
if(FileOpen)
 if(!closeDatabase())return;
QString filename=QFileDialog::getOpenFileName(this,trUtf8("Databank öffnen..."),QDir::homePath(),"*.kdb");
if(filename!=QString::null)
 openDatabase(filename);
}

void KeepassMainWindow::OnFileClose(){
closeDatabase();
}

void KeepassMainWindow::setStateFileOpen(bool IsOpen){
FileOpen=IsOpen;
FileSaveAction->setEnabled(IsOpen);
FileSaveAsAction->setEnabled(IsOpen);
FileCloseAction->setEnabled(IsOpen);
FileSettingsAction->setEnabled(IsOpen);
FileChangeKeyAction->setEnabled(IsOpen);
GroupView->setEnabled(IsOpen);
EntryView->setEnabled(IsOpen);
}

void KeepassMainWindow::editEntry(CEntry* pEntry){
CEditEntryDlg dlg(db,pEntry,this,"EditEntryDialog",true);
dlg.exec();
if(dlg.ModFlag)setStateFileModified(true);
}

void KeepassMainWindow::setStateFileModified(bool mod){
if(!FileOpen)return;
ModFlag=mod;
FileSaveAction->setEnabled(mod);
}


bool KeepassMainWindow::OnFileSave(){
if(db->filename==QString())
 return OnFileSaveAs();
if(db->saveDatabase())
  setStateFileModified(false);
else{
  showErrMsg(trUtf8("Die Datei konnte nicht gespeichert werden.\n%1").arg(db->getError()));
  return false;  
}
return true;
}

bool KeepassMainWindow::OnFileSaveAs(){
QString filename=QFileDialog::getSaveFileName(this,trUtf8("Datenbank speichern unter..."),QDir::homePath(),"*.kdb"); 
if(filename==QString()) return false;
db->filename=filename;
setCaption(tr("Keepass - %1").arg(db->filename));
return OnFileSave();
}

void KeepassMainWindow::OnFileSettings(){
CDbSettingsDlg dlg(this,db,"DatabaseSettingsDlg");
if(dlg.exec())setStateFileModified(true);
}

void KeepassMainWindow::OnFileChangeKey(){
CChangeKeyDlg dlg(this,db,"ChangeKeyDialog");
if(dlg.exec()) setStateFileModified(true);

}

void KeepassMainWindow::OnFileExit(){
if(FileOpen)
 if(!closeDatabase())return;
close();
}

void KeepassMainWindow::OnImportFromPwm(){}

void KeepassMainWindow::OnImportFromKWalletXml(){}

void KeepassMainWindow::OnCurrentGroupChanged(QTreeWidgetItem* cur,QTreeWidgetItem* prev){
if(cur){
 EntryView->setCurrentGroup(((GroupViewItem*)cur)->pGroup->ID);
}
}

void KeepassMainWindow::OnEntryItemDoubleClicked(QTreeWidgetItem* item,int column){
if(column) return;
if(!config.Columns[0]) return;
editEntry(static_cast<EntryViewItem*>(item)->pEntry);
}