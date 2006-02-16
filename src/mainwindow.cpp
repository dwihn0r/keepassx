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

  QuickSearchEdit=new QLineEdit(toolBar);
  QuickSearchEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setupIcons();
  setupToolbar();
  setStateFileOpen(false);
  setupMenus();
  setupConnections();
  FileOpen=false;
  Clipboard=QApplication::clipboard();
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

   connect(EditNewGroupAction, SIGNAL(triggered()), this, SLOT(OnEditNewGroup()));
   connect(EditEditGroupAction, SIGNAL(triggered()), this, SLOT(OnEditEditGroup()));
   connect(EditDeleteGroupAction, SIGNAL(triggered()), this, SLOT(OnEditDeleteGroup()));
   connect(EditNewEntryAction, SIGNAL(triggered()), this, SLOT(OnEditNewEntry()));
   connect(EditEditEntryAction, SIGNAL(triggered()), this, SLOT(OnEditEditEntry()));
   connect(EditCloneEntryAction, SIGNAL(triggered()), this, SLOT(OnEditCloneEntry()));
   connect(EditDeleteEntryAction, SIGNAL(triggered()), this, SLOT(OnEditDeleteEntry()));
   connect(EditUsernameToClipboardAction, SIGNAL(triggered()), this, SLOT(OnEditUsernameToClipboard()));
   connect(EditPasswordToClipboardAction, SIGNAL(triggered()), this, SLOT(OnEditPasswordToClipboard()));
   connect(EditOpenUrlAction, SIGNAL(triggered()), this, SLOT(OnEditOpenUrl()));
   connect(EditSaveAttachmentAction, SIGNAL(triggered()), this, SLOT(OnEditSaveAttachment()));
   connect(EditSearchAction, SIGNAL(triggered()), this, SLOT(OnEditSearch()));
   connect(EditGroupSearchAction, SIGNAL(triggered()), this, SLOT(OnEditGroupSearch()));
  
   connect(ViewShowToolbarAction,SIGNAL(toggled(bool)),this,SLOT(OnViewShowToolbar(bool)));
   connect(ViewShowEntryDetailsAction,SIGNAL(toggled(bool)),this,SLOT(OnViewShowEntryDetails(bool)));
   connect(ViewHidePasswordsAction,SIGNAL(toggled(bool)), this, SLOT(OnUsernPasswVisibilityChanged(bool)));
   connect(ViewHideUsernamesAction,SIGNAL(toggled(bool)), this, SLOT(OnUsernPasswVisibilityChanged(bool)));
   connect(ViewColumnsTitleAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsUsernameAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsUrlAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsPasswordAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsCommentAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsExpireAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsCreationAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsLastChangeAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsLastAccessAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));
   connect(ViewColumnsAttachmentAction,SIGNAL(toggled(bool)), this, SLOT(OnColumnVisibilityChanged(bool)));

   connect(ExtrasSettingsAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasSettings()));

   connect(&ClipboardTimer, SIGNAL(timeout()), this, SLOT(OnClipboardTimeOut()));
   connect(GroupView,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,
		   SLOT(OnCurrentGroupChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
   connect(EntryView,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,
		   SLOT(OnEntryItemDoubleClicked(QTreeWidgetItem*,int)));
   connect(EntryView,SIGNAL(itemSelectionChanged()), this, SLOT(OnEntrySelectionChanged()));
   connect(GroupView,SIGNAL(itemSelectionChanged()), this, SLOT(OnGroupSelectionChanged()));
   connect(GroupView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
   connect(QuickSearchEdit,SIGNAL(returnPressed()), this, SLOT(OnQuickSearch()));

}

void KeepassMainWindow::setupToolbar(){
  toolBar->addAction(FileNewAction);
  toolBar->addAction(FileOpenAction);
  toolBar->addAction(FileSaveAction);
  toolBar->addSeparator();
  toolBar->addAction(EditNewEntryAction);
  toolBar->addAction(EditEditEntryAction);
  toolBar->addAction(EditDeleteEntryAction);
  toolBar->addSeparator();
  toolBar->addAction(EditPasswordToClipboardAction);
  toolBar->addAction(EditUsernameToClipboardAction);
  toolBar->addSeparator();
  toolBar->addWidget(QuickSearchEdit);
}

void KeepassMainWindow::setupIcons(){
FileNewAction->setIcon(*Icon_FileNew);
FileOpenAction->setIcon(*Icon_FileOpen);
FileSaveAction->setIcon(*Icon_FileSave);
FileSaveAsAction->setIcon(*Icon_FileSaveAs);
FileCloseAction->setIcon(*Icon_FileClose);
FileSettingsAction->setIcon(*Icon_Configure);
FileExitAction->setIcon(*Icon_Exit);
EditNewEntryAction->setIcon(*Icon_EditAdd);
EditEditEntryAction->setIcon(*Icon_EditEdit);
EditDeleteEntryAction->setIcon(*Icon_EditDelete);
EditPasswordToClipboardAction->setIcon(*Icon_EditPasswordToCb);
EditUsernameToClipboardAction->setIcon(*Icon_EditUsernameToCb);
EditCloneEntryAction->setIcon(*Icon_EditClone);
EditOpenUrlAction->setIcon(*Icon_EditOpenUrl);
EditSaveAttachmentAction->setIcon(*Icon_FileSave);
EditNewGroupAction->setIcon(*Icon_EditAdd);
EditEditGroupAction->setIcon(*Icon_EditEdit);
EditDeleteGroupAction->setIcon(*Icon_EditDelete);
EditSearchAction->setIcon(*Icon_EditSearch);
EditGroupSearchAction->setIcon(*Icon_EditSearch);
ExtrasSettingsAction->setIcon(*Icon_Configure);
}

void KeepassMainWindow::setupMenus(){
  GroupView->ContextMenu->addAction(EditNewGroupAction);
  GroupView->ContextMenu->addAction(EditEditGroupAction);
  GroupView->ContextMenu->addAction(EditDeleteGroupAction);
  GroupView->ContextMenu->addSeparator();
  GroupView->ContextMenu->addAction(EditNewEntryAction);
  GroupView->ContextMenu->addSeparator();
  GroupView->ContextMenu->addAction(EditGroupSearchAction);

  EntryView->ContextMenu->addAction(EditPasswordToClipboardAction);
  EntryView->ContextMenu->addAction(EditUsernameToClipboardAction);
  EntryView->ContextMenu->addAction(EditOpenUrlAction);
  EntryView->ContextMenu->addAction(EditSaveAttachmentAction);
  EntryView->ContextMenu->addSeparator();
  EntryView->ContextMenu->addAction(EditNewEntryAction);
  EntryView->ContextMenu->addAction(EditEditEntryAction);
  EntryView->ContextMenu->addAction(EditCloneEntryAction);
  EntryView->ContextMenu->addAction(EditDeleteEntryAction);

  ViewShowToolbarAction->setChecked(config.Toolbar);
  ViewShowEntryDetailsAction->setChecked(config.EntryDetails);
  ViewHidePasswordsAction->setChecked(config.ListView_HidePasswords);
  ViewHideUsernamesAction->setChecked(config.ListView_HideUsernames);
  ViewColumnsTitleAction->setChecked(config.Columns[0]);
  ViewColumnsUsernameAction->setChecked(config.Columns[1]);
  ViewColumnsUrlAction->setChecked(config.Columns[2]);
  ViewColumnsPasswordAction->setChecked(config.Columns[3]);
  ViewColumnsCommentAction->setChecked(config.Columns[4]);
  ViewColumnsExpireAction->setChecked(config.Columns[5]);
  ViewColumnsCreationAction->setChecked(config.Columns[6]);
  ViewColumnsLastChangeAction->setChecked(config.Columns[7]);
  ViewColumnsLastAccessAction->setChecked(config.Columns[8]);
  ViewColumnsAttachmentAction->setChecked(config.Columns[9]);

  FileNewAction->setShortcut(tr("Ctrl+N"));
  FileOpenAction->setShortcut(tr("Ctrl+O"));
  FileSaveAction->setShortcut(tr("Ctrl+S"));
  EditNewGroupAction->setShortcut(tr("Ctrl+G"));
  EditPasswordToClipboardAction->setShortcut(tr("Ctrl+C"));
  EditUsernameToClipboardAction->setShortcut(tr("Ctrl+B"));
  EditOpenUrlAction->setShortcut(tr("Ctrl+U"));
  EditNewEntryAction->setShortcut(tr("Ctrl+Y"));
  EditEditEntryAction->setShortcut(tr("Ctrl+E"));
  EditDeleteEntryAction->setShortcut(tr("Ctrl+D"));
  EditCloneEntryAction->setShortcut(tr("Ctrl+K"));
  EditSearchAction->setShortcut(tr("Ctrl+F"));
#ifdef Q_WS_MAC
  FileCloseAction->setShortcut(tr("Ctrl+W"));
  FileSaveAsAction->setShortcut(tr("Shift+Ctrl+S"));
  EditGroupSearchAction->setShortcut(tr("Shift+Ctrl+F"));
#endif
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
				  trUtf8("Die aktuell geöffnete Datei wurde verändert. Sollen die Änderungen\nvor dem Schließen gespeichert werden?"),tr("Ja"),tr("Nein"),tr("Abbrechen"),2,2);
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
SearchResults.clear();
GroupView->ShowSearchGroup=false;
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
EditSearchAction->setEnabled(IsOpen);
GroupView->setEnabled(IsOpen);
EntryView->setEnabled(IsOpen);
DetailView->setEnabled(IsOpen);
QuickSearchEdit->setEnabled(IsOpen);
if(!IsOpen){
    EditNewGroupAction->setEnabled(false);
    EditEditGroupAction->setEnabled(false);
    EditDeleteGroupAction->setEnabled(false);
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditNewEntryAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(false);
    EditDeleteEntryAction->setEnabled(false);
    EditGroupSearchAction->setEnabled(false);
}
else{
    OnGroupSelectionChanged();
    OnEntrySelectionChanged();
}

}

void KeepassMainWindow::editEntry(CEntry* pEntry){
CEditEntryDlg dlg(db,pEntry,this,"EditEntryDialog",true);
dlg.exec();
EntryView->refreshVisibleItems();
if(dlg.ModFlag)setStateFileModified(true);
}

void KeepassMainWindow::setStateFileModified(bool mod){
if(!FileOpen)return;
ModFlag=mod;
FileSaveAction->setEnabled(mod);
}

void KeepassMainWindow::setStateGroupSelected(SelectionState s){
GroupSelection=s;
switch(GroupSelection){
 case NONE:
    EditNewGroupAction->setEnabled(true);
    EditEditGroupAction->setEnabled(false);
    EditDeleteGroupAction->setEnabled(false);
    EditGroupSearchAction->setEnabled(false);
    EditNewEntryAction->setEnabled(false);
    break;
 case SINGLE:
    EditNewGroupAction->setEnabled(true);
    EditEditGroupAction->setEnabled(true);
    EditDeleteGroupAction->setEnabled(true);
    EditGroupSearchAction->setEnabled(true);
    EditNewEntryAction->setEnabled(true);
    break;
 case SEARCHGROUP:
    EditNewGroupAction->setEnabled(false);
    EditEditGroupAction->setEnabled(false);
    EditDeleteGroupAction->setEnabled(false);
    EditGroupSearchAction->setEnabled(false);
    EditNewEntryAction->setEnabled(false);
    break;
   
	
default: Q_ASSERT(false);
}
}

void KeepassMainWindow::updateDetailView(){
if(EntryView->selectedItems().size()!=1){
 DetailView->setText("");
 return;}

CEntry& entry=*((EntryViewItem*)(EntryView->selectedItems()[0]))->pEntry;
QString str=trUtf8("<B>Gruppe: </B>%1  <B>Titel: </B>%2  <B>Benutzername: </B>%3  <B>URL: </B><a href=%4>%4</a>  <B>Passwort: </B>%5  <B>Erstellt: </B>%6  <B>letzte Änderung: </B>%7  <B>letzter Zugriff: </B>%8  <B>gültig bis: </B>%9");
str=str.arg(currentGroup()->Name).arg(entry.Title);

if(!config.ListView_HideUsernames)	str=str.arg(entry.UserName);
else str=str.arg("****");

str=str.arg(entry.URL);

if(!config.ListView_HidePasswords)	str=str.arg(entry.Password.getString());
else	str=str.arg("****");

str=str.arg(entry.Creation.toString(Qt::LocalDate))
	  .arg(entry.LastMod.toString(Qt::LocalDate))
	  .arg(entry.LastAccess.toString(Qt::LocalDate))
	  .arg(entry.Expire.toString(Qt::LocalDate));
DetailView->setHtml(str);
entry.Password.delRef();

}


void KeepassMainWindow::setStateEntrySelected(SelectionState s){
EntrySelection=s;
if(GroupSelection == NONE || GroupSelection == SINGLE)
switch(EntrySelection){
 case NONE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(trUtf8("Eintrag duplizieren"));
    EditDeleteEntryAction->setEnabled(false);
    EditDeleteEntryAction->setText(trUtf8("Eintrag löschen"));
    break;
 case SINGLE:
    EditPasswordToClipboardAction->setEnabled(true);
    EditUsernameToClipboardAction->setEnabled(true);
    EditOpenUrlAction->setEnabled(true);
    EditSaveAttachmentAction->setEnabled(true);
    EditEditEntryAction->setEnabled(true);
    EditCloneEntryAction->setEnabled(true);
    EditCloneEntryAction->setText(trUtf8("Eintrag duplizieren"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(trUtf8("Eintrag löschen"));
    break;
 case MULTIPLE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(true);
    EditCloneEntryAction->setText(trUtf8("Einträge duplizieren"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(trUtf8("Einträge löschen"));
    break;
 default: Q_ASSERT(false);
}
else if(GroupSelection == SEARCHGROUP)
switch(EntrySelection){
 case NONE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(trUtf8("Eintrag duplizieren"));
    EditDeleteEntryAction->setEnabled(false);
    EditDeleteEntryAction->setText(trUtf8("Eintrag löschen"));
    break;
 case SINGLE:
    EditPasswordToClipboardAction->setEnabled(true);
    EditUsernameToClipboardAction->setEnabled(true);
    EditOpenUrlAction->setEnabled(true);
    EditSaveAttachmentAction->setEnabled(true);
    EditEditEntryAction->setEnabled(true);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(trUtf8("Eintrag duplizieren"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(trUtf8("Eintrag löschen"));
    break;
 case MULTIPLE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(trUtf8("Einträge duplizieren"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(trUtf8("Einträge löschen"));
    break;
 default: Q_ASSERT(false);
}
else Q_ASSERT(false);
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
 if(GroupView->isSearchResultGroup((GroupViewItem*)cur)){
	EntryView->showSearchResults(SearchResults);
 }
 else EntryView->setCurrentGroup(((GroupViewItem*)cur)->pGroup->ID);
}
}

void KeepassMainWindow::OnEntryItemDoubleClicked(QTreeWidgetItem* item,int column){
if(column) return;
if(!config.Columns[0]) return;
editEntry(static_cast<EntryViewItem*>(item)->pEntry);
}

void KeepassMainWindow::OnEntrySelectionChanged(){
updateDetailView();
if(EntryView->selectedItems().size()==0)
  setStateEntrySelected(NONE);
if(EntryView->selectedItems().size()==1)
  setStateEntrySelected(SINGLE);
if(EntryView->selectedItems().size()>1)
  setStateEntrySelected(MULTIPLE);
}

void KeepassMainWindow::OnGroupSelectionChanged(){
Q_ASSERT(GroupView->selectedItems().size()<=1);
if(GroupView->selectedItems().size()==0)
  setStateGroupSelected(NONE);
if(GroupView->selectedItems().size()==1){
	if(GroupView->isSearchResultGroup((GroupViewItem*)GroupView->selectedItems()[0]))
	 setStateGroupSelected(SEARCHGROUP);
	else
	 setStateGroupSelected(SINGLE);
}
}

void KeepassMainWindow::OnEditNewGroup(){
CGroup *pNew=NULL;
if(GroupView->selectedItems().size())
 pNew=db->addGroup(static_cast<GroupViewItem*>(GroupView->selectedItems()[0])->pGroup);
else
 pNew=db->addGroup(NULL);
CEditGroupDialog dlg(this,"EditGroupDlg",true);
if(!dlg.exec()){
 db->deleteGroup(pNew);
 return;
}
pNew->Name=dlg.GroupName;
pNew->ImageID=dlg.IconID;
setStateFileModified(true);
GroupView->updateItems();
}

void KeepassMainWindow::OnEditEditGroup(){
Q_ASSERT(GroupView->selectedItems().size());
CGroup *pGroup=static_cast<GroupViewItem*>(GroupView->selectedItems()[0])->pGroup;
CEditGroupDialog dlg(this,"EditGroupDlg",true);
dlg.GroupName=pGroup->Name;
dlg.IconID=pGroup->ImageID;
if(!dlg.exec())return;
if((pGroup->Name != dlg.GroupName) || (pGroup->ImageID != dlg.IconID)){
  setStateFileModified(true);
  pGroup->Name = dlg.GroupName;
  pGroup->ImageID = dlg.IconID;
  GroupView->updateItems();
}
}

void KeepassMainWindow::OnEditDeleteGroup(){
Q_ASSERT(GroupView->selectedItems().size());
CGroup *pGroup=static_cast<GroupViewItem*>(GroupView->selectedItems()[0])->pGroup;
db->deleteGroup(pGroup);
GroupView->updateItems();
setStateFileModified(true);
}

void KeepassMainWindow::OnEditNewEntry(){
CEntry NewEntry;
NewEntry.GroupID=currentGroup()->ID;
CEditEntryDlg dlg(db,&NewEntry,this,"EditEntryDialog",true);
if(dlg.exec()){
 db->addEntry(&NewEntry);
 EntryView->updateItems();
 setStateFileModified(true);
}
}

void KeepassMainWindow::OnEditEditEntry(){
Q_ASSERT(EntryView->selectedItems().size()==1);
editEntry(static_cast<EntryViewItem*>(EntryView->selectedItems()[0])->pEntry);
}

void KeepassMainWindow::OnEditCloneEntry(){
Q_ASSERT(EntryView->selectedItems().size()>0);
QList<QTreeWidgetItem*> entries=EntryView->selectedItems();
for(int i=0; i<entries.size();i++){
	db->cloneEntry(((EntryViewItem*)entries[i])->pEntry);
}
setStateFileModified(true);
EntryView->updateItems();
}

void KeepassMainWindow::OnEditDeleteEntry(){
Q_ASSERT(EntryView->selectedItems().size()>0);
QList<QTreeWidgetItem*> entries=EntryView->selectedItems();
for(int i=0; i<entries.size();i++){
	removeFromSearchResults(((EntryViewItem*)entries[i])->pEntry->sID);
	db->deleteEntry(((EntryViewItem*)entries[i])->pEntry);
}
setStateFileModified(true);
EntryView->updateItems();
}

void KeepassMainWindow::removeFromSearchResults(int id){
for(int i=0; i<SearchResults.size();i++){
 if(SearchResults[i]==id){
	SearchResults.removeAt(i);
	return;
 }
}

}


void KeepassMainWindow::OnEditUsernameToClipboard(){
Clipboard->setText(currentEntry()->UserName,  QClipboard::Clipboard);
ClipboardTimer.start(config.ClipboardTimeOut*1000,true);
}

void KeepassMainWindow::OnEditPasswordToClipboard(){
Clipboard->setText(currentEntry()->Password.getString(),QClipboard::Clipboard);
ClipboardTimer.start(config.ClipboardTimeOut*1000,true);
currentEntry()->Password.delRef();

}

void KeepassMainWindow::OnClipboardTimeOut(){
Clipboard->clear(QClipboard::Clipboard);
}

void KeepassMainWindow::OnEditSaveAttachment(){
CEditEntryDlg::saveAttachment(currentEntry(),this);
}

void KeepassMainWindow::OnEditOpenUrl(){
openBrowser(currentEntry()->URL);
}

void KeepassMainWindow::search(CGroup* group){
CSearchDlg dlg(db,group,this,"SearchDialog",false);
if(dlg.exec()){
 SearchResults=dlg.Hits;
 GroupView->ShowSearchGroup=true;
 GroupView->updateItems();
 GroupView->selectSearchGroup();
 EntryView->showSearchResults(SearchResults);
}
else
{

}
}

void KeepassMainWindow::OnEditSearch(){
search(NULL);
}

void KeepassMainWindow::OnEditGroupSearch(){
search(currentGroup());
}

void KeepassMainWindow::OnQuickSearch(){
// TODO: solution without a hidden CSearchDlg window-----
	CSearchDlg dlg(db,NULL,this,"SearchDialog",false);
	dlg.Edit_Search->setText(QuickSearchEdit->text());
	dlg.OnButtonSearch();
	SearchResults=dlg.Hits;
//-------------------------------------------------------
GroupView->ShowSearchGroup=true;
GroupView->updateItems();
GroupView->selectSearchGroup();
EntryView->showSearchResults(SearchResults);
}

CGroup* KeepassMainWindow::currentGroup(){
Q_ASSERT(GroupView->selectedItems().size());
return static_cast<GroupViewItem*>(GroupView->selectedItems()[0])->pGroup;
}

CEntry* KeepassMainWindow::currentEntry(){
Q_ASSERT(EntryView->selectedItems().size()==1);
return static_cast<EntryViewItem*>(EntryView->selectedItems()[0])->pEntry;
}

void KeepassMainWindow::OnColumnVisibilityChanged(bool value){
config.Columns[0]=ViewColumnsTitleAction->isChecked();
config.Columns[1]=ViewColumnsUsernameAction->isChecked();
config.Columns[2]=ViewColumnsUrlAction->isChecked();
config.Columns[3]=ViewColumnsPasswordAction->isChecked();
config.Columns[4]=ViewColumnsCommentAction->isChecked();
config.Columns[5]=ViewColumnsExpireAction->isChecked();
config.Columns[6]=ViewColumnsCreationAction->isChecked();
config.Columns[7]=ViewColumnsLastChangeAction->isChecked();
config.Columns[8]=ViewColumnsLastAccessAction->isChecked();
config.Columns[9]=ViewColumnsAttachmentAction->isChecked();
EntryView->updateColumns();
if(FileOpen) EntryView->updateItems();
}

void KeepassMainWindow::OnUsernPasswVisibilityChanged(bool value){
config.ListView_HidePasswords=ViewHidePasswordsAction->isChecked();
config.ListView_HideUsernames=ViewHideUsernamesAction->isChecked();
EntryView->updateItems();
}

void KeepassMainWindow::OnFileModified(){
setStateFileModified(true);
}

void KeepassMainWindow::closeEvent(QCloseEvent* e){
if(FileOpen){
 if(!closeDatabase())
 	e->ignore();
 else
	e->accept();
}
else
 e->accept();
}


void KeepassMainWindow::OnExtrasSettings(){
CSettingsDlg dlg(this,"SettingsDlg");
dlg.exec();

}

void KeepassMainWindow::OnViewShowToolbar(bool show){
config.Toolbar=show;
toolBar->setVisible(config.Toolbar);
}

void KeepassMainWindow::OnViewShowEntryDetails(bool show){
config.EntryDetails=show;
DetailView->setVisible(config.EntryDetails);
}
