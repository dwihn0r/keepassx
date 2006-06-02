/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
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
#include <QStatusBar>

#include "lib/random.h"
#include "lib/IniReader.h"
#include "lib/AutoType.h"
#include "import/Import_PwManager.h"
#include "import/Import_KWalletXml.h"
#include "export/Export_Txt.h"

#include "dialogs/AboutDlg.h"
#include "dialogs/EditGroupDlg.h"
#include "dialogs/SearchDlg.h"
#include "dialogs/SettingsDlg.h"
#include "dialogs/DatabaseSettingsDlg.h"
#include "dialogs/PasswordDlg.h"
#include "dialogs/SimplePasswordDlg.h"
#include "dialogs/EditEntryDlg.h"
#include "dialogs/PasswordGenDlg.h"


KeepassMainWindow::KeepassMainWindow(const QString& ArgFile,QWidget *parent, Qt::WFlags flags):QMainWindow(parent,flags){
  Start=true;
  setupUi(this);
  AutoType::MainWin=this;
  setGeometry(geometry().x(),geometry().y(),config.MainWinWidth,config.MainWinHeight);
  splitter->setSizes(QList<int>() << config.MainWinSplit1 << config.MainWinSplit2);
  setupIcons();
  setupToolbar();
  setStateFileOpen(false);
  setupMenus();
  setStatusBar(new QStatusBar(this));
  StatusBarGeneral=new QLabel(tr("Ready"),statusBar());
  StatusBarSelection=new QLabel(statusBar());
  statusBar()->addWidget(StatusBarGeneral,15);
  statusBar()->addWidget(StatusBarSelection,85);
  statusBar()->setVisible(config.ShowStatusbar);
  CGroup::UI_ExpandByDefault=config.ExpandGroupTree;
  setupConnections();
  FileOpen=false;
  Clipboard=QApplication::clipboard();
  if(ArgFile!=QString())
	openDatabase(ArgFile,false);
  else if(config.OpenLast && (config.LastFile!=QString()) ){
		QFileInfo file(config.LastFile);
		if(file.exists())
			openDatabase(config.LastFile,true);
		else
			config.LastFile=QString();	
  		}
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
	connect(FileExpPlainTextAction,SIGNAL(triggered()),this,SLOT(OnExportToTxt()));
	
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
	connect(EditAutoTypeAction,SIGNAL(triggered()),this,SLOT(OnEditAutoType()));
	
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
	connect(ViewToolButtonSize16Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize16(bool)));
	connect(ViewToolButtonSize22Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize22(bool)));
	connect(ViewToolButtonSize28Action,SIGNAL(toggled(bool)), this, SLOT(OnViewToolbarIconSize28(bool)));
	connect(ViewShowStatusbarAction,SIGNAL(toggled(bool)),statusBar(),SLOT(setVisible(bool)));
	
	connect(ExtrasSettingsAction,SIGNAL(triggered(bool)),this,SLOT(OnExtrasSettings()));
	
	connect(HelpHandbookAction,SIGNAL(triggered()),this,SLOT(OnHelpHandbook()));
	connect(HelpAboutAction,SIGNAL(triggered()),this,SLOT(OnHelpAbout()));
	
	connect(GroupView,SIGNAL(entryDropped()),EntryView,SLOT(updateItems()));
	connect(this,SIGNAL(entryChanged()),EntryView,SLOT(updateItems()),Qt::QueuedConnection);
	connect(&ClipboardTimer, SIGNAL(timeout()), this, SLOT(OnClipboardTimeOut()));
	connect(GroupView,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,
			SLOT(OnCurrentGroupChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(GroupView,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(OnItemExpanded(QTreeWidgetItem*)));
	connect(GroupView,SIGNAL(itemCollapsed(QTreeWidgetItem*)),this,SLOT(OnItemCollaped(QTreeWidgetItem*)));
	connect(EntryView,SIGNAL(itemActivated(QTreeWidgetItem*,int)),this,
			SLOT(OnEntryItemDoubleClicked(QTreeWidgetItem*,int)));
	connect(EntryView,SIGNAL(itemSelectionChanged()), this, SLOT(OnEntrySelectionChanged()));
	connect(GroupView,SIGNAL(itemSelectionChanged()), this, SLOT(OnGroupSelectionChanged()));
	connect(GroupView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
	connect(QuickSearchEdit,SIGNAL(returnPressed()), this, SLOT(OnQuickSearch()));
	connect(HideSearchResultsAction,SIGNAL(triggered()),this, SLOT(OnHideSearchGroup()));

}

void KeepassMainWindow::setupToolbar(){
  toolBar=new QToolBar(this);
  addToolBar(toolBar);
  toolBar->setIconSize(QSize(config.ToolbarIconSize,config.ToolbarIconSize));
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
  QuickSearchEdit=new QLineEdit(toolBar);
  QuickSearchEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  toolBar->addWidget(QuickSearchEdit);
}

void KeepassMainWindow::setupIcons(){
setWindowIcon(QIcon(AppDir+"/../share/keepass/icons/keepassx.png"));
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
EditAutoTypeAction->setIcon(*Icon_AutoType);
HelpHandbookAction->setIcon(*Icon_Help);
}

void KeepassMainWindow::setupMenus(){
  GroupView->ContextMenu->addAction(EditNewGroupAction);
  GroupView->ContextMenu->addAction(EditEditGroupAction);
  GroupView->ContextMenu->addAction(EditDeleteGroupAction);
  GroupView->ContextMenu->addSeparator();
  GroupView->ContextMenu->addAction(EditNewEntryAction);
  GroupView->ContextMenu->addSeparator();
  GroupView->ContextMenu->addAction(EditGroupSearchAction);
  GroupView->ContextMenuSearchGroup->addAction(HideSearchResultsAction);

  EntryView->ContextMenu->addAction(EditPasswordToClipboardAction);
  EntryView->ContextMenu->addAction(EditUsernameToClipboardAction);
  EntryView->ContextMenu->addAction(EditOpenUrlAction);
  EntryView->ContextMenu->addAction(EditSaveAttachmentAction);
  EntryView->ContextMenu->addAction(EditAutoTypeAction);
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
  ViewShowStatusbarAction->setChecked(config.ShowStatusbar);

  switch(config.ToolbarIconSize){
	case 16: ViewToolButtonSize16Action->setChecked(true); break;
	case 22: ViewToolButtonSize22Action->setChecked(true); break;
	case 28: ViewToolButtonSize28Action->setChecked(true); break;
  }

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
  EditAutoTypeAction->setShortcut(tr("Ctrl+V"));
#ifdef Q_WS_MAC
  FileCloseAction->setShortcut(tr("Ctrl+W"));
  FileSaveAsAction->setShortcut(tr("Shift+Ctrl+S"));
  EditGroupSearchAction->setShortcut(tr("Shift+Ctrl+F"));
#endif
}

void KeepassMainWindow::setupDatabaseConnections(Database* DB){
connect(DB,SIGNAL(iconsModified()),this,SLOT(OnFileModified()));
connect(DB,SIGNAL(iconsModified()),EntryView,SLOT(updateItems()));
connect(DB,SIGNAL(iconsModified()),GroupView,SLOT(updateItems()));
}


void KeepassMainWindow::openDatabase(QString filename,bool IsAuto){
Q_ASSERT(!FileOpen);
if(!IsAuto){
	config.LastKeyLocation=QString();
	config.LastKeyType=PASSWORD;}
CPasswordDialog PasswordDlg(this,"Password Dialog",true,IsAuto);
PasswordDlg.setCaption(filename);
int r=PasswordDlg.exec();
if(r==0) return;
if(r==2) {Start=false; return;}
Q_ASSERT(r==1);
db = new PwDatabase();
GroupView->db=db;
EntryView->db=db;
setupDatabaseConnections(db);
if(PasswordDlg.password!="" && PasswordDlg.keyfile=="")
	db->CalcMasterKeyByPassword(PasswordDlg.password);
if(PasswordDlg.password=="" && PasswordDlg.keyfile!="")
	db->CalcMasterKeyByFile(PasswordDlg.keyfile);
if(PasswordDlg.password!="" && PasswordDlg.keyfile!="")
	db->CalcMasterKeyByFileAndPw(PasswordDlg.keyfile,PasswordDlg.password);
QString err;
StatusBarGeneral->setText(tr("Loading Database..."));
if(db->openDatabase(filename,err)==true){
//SUCCESS
if(config.OpenLast)config.LastFile=filename;
setCaption(tr("KeePassX - %1").arg(filename));
GroupView->updateItems();
EntryView->updateItems(0);
setStateFileOpen(true);
setStateFileModified(false);
}
else{
//ERROR
delete db;
StatusBarGeneral->setText(tr("Loading Failed"));
if(err=="")err=tr("Unknown error in PwDatabase::openDatabase()");
QMessageBox::critical(this,tr("Error")
						  ,tr("The following error occured while opening the database:\n%1")
				 		  .arg(err),tr("OK"));
}
StatusBarGeneral->setText(tr("Ready"));
}

bool KeepassMainWindow::closeDatabase(){
Q_ASSERT(FileOpen);
Q_ASSERT(db!=NULL);
if(ModFlag){
 int r=QMessageBox::question(this,tr("Save modified file?"),
				  tr("The current file was modified. Do you want\nto save the changes?"),tr("Yes"),tr("No"),tr("Cancel"),2,2);
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
setCaption("KeePassX Password Manager");
return true;
}


void KeepassMainWindow::OnFileNew(){
CPasswordDialog dlg(this,"PasswordDlg",true,false,true);
dlg.setCaption("New Database");
if(dlg.exec()==1){
	if(FileOpen)
		if(!closeDatabase())return;
	db=new PwDatabase();
	db->newDatabase();
	if(dlg.KeyType==BOTH || dlg.KeyType==KEYFILE){
		if(!db->createKeyFile(dlg.keyfile)){
			QMessageBox::warning(this,tr("Error"),tr("Could not create key file. The following error occured:\n%1").arg(db->getError()),tr("OK"),"","",0,0);
			delete db; db=NULL;
			return;
		}
	}	
	if(dlg.password!="" && dlg.keyfile=="")
		db->CalcMasterKeyByPassword(dlg.password);
	if(dlg.password=="" && dlg.keyfile!="")
		db->CalcMasterKeyByFile(dlg.keyfile);
	if(dlg.password!="" && dlg.keyfile!="")
		db->CalcMasterKeyByFileAndPw(dlg.keyfile,dlg.password);
	setCaption(tr("KeePassX - %1").arg(tr("[new]")));
	GroupView->db=db;
	EntryView->db=db;
	GroupView->updateItems();
	EntryView->updateItems(0);
	setStateFileOpen(true);
	setStateFileModified(true);
	FileOpen=true;
	setupDatabaseConnections(db);
}
}

void KeepassMainWindow::OnFileOpen(){
QString filename=QFileDialog::getOpenFileName(this,tr("Open Database..."),QDir::homePath(),"*.kdb");
if(filename!=QString()){
	if(FileOpen)
 		if(!closeDatabase())return;
	openDatabase(filename);
}
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
FileExpPlainTextAction->setEnabled(IsOpen);


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
	EditAutoTypeAction->setEnabled(false);
}
else{
    OnGroupSelectionChanged();
    OnEntrySelectionChanged();
}

}

void KeepassMainWindow::editEntry(CEntry* pEntry){
CEditEntryDlg dlg(db,pEntry,this,"EditEntryDialog",true);
switch(dlg.exec()){
	case 0: //canceled or no changes
			break;
	case 1: //modifications but same group
			EntryView->refreshItems();
			setStateFileModified(true);
			break;
	case 2: //entry moved to another group
			emit entryChanged(); //a direct call of updateItems() would cause a SegFault because of the TreeView base class slots
			setStateFileModified(true);
			break;
}
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
QString str=tr("<B>Group: </B>%1  <B>Title: </B>%2  <B>Username: </B>%3  <B>URL: </B><a href=%4>%4</a>  <B>Password: </B>%5  <B>Creation: </B>%6  <B>Last Change: </B>%7  <B>LastAccess: </B>%8  <B>Expires: </B>%9");
//todo: a "CGroup* PwDatabase::getGroup(CEntry*)" method would be a good idea
str=str.arg(db->group(db->getGroupIndex(entry.GroupID)).Name).arg(entry.Title);

if(!config.ListView_HideUsernames)	str=str.arg(entry.UserName);
else str=str.arg("****");

str=str.arg(entry.URL);

entry.Password.unlock();
if(!config.ListView_HidePasswords)	str=str.arg(entry.Password.string());
else	str=str.arg("****");

entry.Password.lock();

str=str.arg(entry.Creation.toString(Qt::LocalDate))
	  .arg(entry.LastMod.toString(Qt::LocalDate))
	  .arg(entry.LastAccess.toString(Qt::LocalDate))
	  .arg(entry.Expire.toString(Qt::LocalDate));
DetailView->setHtml(str);


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
    EditCloneEntryAction->setText(tr("Clone Entry"));
    EditDeleteEntryAction->setEnabled(false);
    EditDeleteEntryAction->setText(tr("Delete Entry"));
	EditAutoTypeAction->setEnabled(false);
    break;
 case SINGLE:
    EditPasswordToClipboardAction->setEnabled(true);
    EditUsernameToClipboardAction->setEnabled(true);
    EditOpenUrlAction->setEnabled(true);
    EditSaveAttachmentAction->setEnabled(true);
    EditEditEntryAction->setEnabled(true);
    EditCloneEntryAction->setEnabled(true);
    EditCloneEntryAction->setText(tr("Clone Entry"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(tr("Delete Entry"));
	EditAutoTypeAction->setEnabled(true);
    break;
 case MULTIPLE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(true);
    EditCloneEntryAction->setText(tr("Clone Entries"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(tr("Delete Entries"));
	EditAutoTypeAction->setEnabled(false);
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
    EditCloneEntryAction->setText(tr("Clone Entry"));
    EditDeleteEntryAction->setEnabled(false);
    EditDeleteEntryAction->setText(tr("Delete Entry"));
	EditAutoTypeAction->setEnabled(false);
    break;
 case SINGLE:
    EditPasswordToClipboardAction->setEnabled(true);
    EditUsernameToClipboardAction->setEnabled(true);
    EditOpenUrlAction->setEnabled(true);
    EditSaveAttachmentAction->setEnabled(true);
    EditEditEntryAction->setEnabled(true);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(tr("Clone Entry"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(tr("Delete Entry"));
	EditAutoTypeAction->setEnabled(true);
    break;
 case MULTIPLE:
    EditPasswordToClipboardAction->setEnabled(false);
    EditUsernameToClipboardAction->setEnabled(false);
    EditOpenUrlAction->setEnabled(false);
    EditSaveAttachmentAction->setEnabled(false);
    EditEditEntryAction->setEnabled(false);
    EditCloneEntryAction->setEnabled(false);
    EditCloneEntryAction->setText(tr("Clone Entries"));
    EditDeleteEntryAction->setEnabled(true);
    EditDeleteEntryAction->setText(tr("Delete Entries"));
	EditAutoTypeAction->setEnabled(false);
    break;
 default: Q_ASSERT(false);
}
else Q_ASSERT(false);
}

bool KeepassMainWindow::OnFileSave(){
if(db->file->fileName()==QString())
 return OnFileSaveAs();
config.LastFile=db->file->fileName();
if(db->saveDatabase())
  setStateFileModified(false);
else{
  showErrMsg(tr("File could not be saved.\n%1").arg(db->getError()));
  return false;  
}
return true;
}

bool KeepassMainWindow::OnFileSaveAs(){
QString filename=QFileDialog::getSaveFileName(this,tr("Save Database As..."),QDir::homePath(),"*.kdb"); 
if(filename==QString()) return false;
Q_ASSERT(db->file);
if(db->file->isOpen()) db->file->close();

db->file->setFileName(filename);
setCaption(tr("KeePassX - %1").arg(filename));
return OnFileSave();
}

void KeepassMainWindow::OnFileSettings(){
CDbSettingsDlg dlg(this,db,"DatabaseSettingsDlg");
if(dlg.exec())setStateFileModified(true);
}

void KeepassMainWindow::OnFileChangeKey(){
CPasswordDialog dlg(this,"PasswordDlg",true,false,true);
dlg.setCaption(db->file->fileName());
if(dlg.exec()==1){
	if(dlg.KeyType==BOTH || dlg.KeyType==KEYFILE){
		if(!db->createKeyFile(dlg.keyfile)){
			QMessageBox::warning(this,tr("Error"),tr("Could not create key file. The following error occured:\n%1").arg(db->getError()),tr("OK"),"","",0,0);
			return;
		}
	}	
	if(dlg.password!="" && dlg.keyfile=="")
		db->CalcMasterKeyByPassword(dlg.password);
	if(dlg.password=="" && dlg.keyfile!="")
		db->CalcMasterKeyByFile(dlg.keyfile);
	if(dlg.password!="" && dlg.keyfile!="")
		db->CalcMasterKeyByFileAndPw(dlg.keyfile,dlg.password);
	setStateFileModified(true);
}

}

void KeepassMainWindow::OnFileExit(){
close();
}

void KeepassMainWindow::OnExportToTxt(){
QString filename=QFileDialog::getSaveFileName(this,tr("Export To..."),QDir::homePath(),"*.txt");
if(filename==QString())return;
Export_Txt exp;
QString err;
exp.exportFile(filename,db,err);
}

void KeepassMainWindow::OnImportFromPwm(){
if(FileOpen)
 if(!closeDatabase())return;
QString filename=QFileDialog::getOpenFileName(this,tr("Open Database..."),QDir::homePath(),"*.pwm");
if(filename!=QString::null){
	Q_ASSERT(!FileOpen);
	db = new PwDatabase();
	db->newDatabase();
	CSimplePasswordDialog dlg(this,"SimplePasswordDlg",true);
	if(!dlg.exec()){
		delete db;
		db=NULL;
		StatusBarGeneral->setText(tr("Ready"));
	}
	GroupView->db=db;
	EntryView->db=db;
	QString err;
	StatusBarGeneral->setText(tr("Loading Database..."));
	Import_PwManager import;
	if(import.importFile(filename,dlg.password,db,err)==true){
		//SUCCESS
		setCaption(tr("KeePassX [new]"));
		GroupView->updateItems();
		EntryView->updateItems(0);
		setStateFileOpen(true);
		setStateFileModified(true);
	}
	else{
		//ERROR
		delete db;
		StatusBarGeneral->setText(tr("Loading Failed"));
		if(err=="")err=tr("Unknown error in Import_PwManager::importFile()()");
		QMessageBox::critical(this,tr("Error")
								,tr("The following error occured while opening the database:\n%1")
								.arg(err),tr("OK"));
	}
	StatusBarGeneral->setText(tr("Ready"));
	}
}

void KeepassMainWindow::OnImportFromKWalletXml(){
if(FileOpen)
 if(!closeDatabase())return;
QString filename=QFileDialog::getOpenFileName(this,tr("Open Database..."),QDir::homePath(),"*.pwm");
if(filename!=QString::null){
	Q_ASSERT(!FileOpen);
	db = new PwDatabase();
	db->newDatabase();
	GroupView->db=db;
	EntryView->db=db;
	QString err;
	StatusBarGeneral->setText(tr("Loading Database..."));
	Import_KWalletXml import;
	if(import.importFile(filename,db,err)==true){
		//SUCCESS
		setCaption(tr("KeePassX [new]"));
		GroupView->updateItems();
		EntryView->updateItems(0);
		setStateFileOpen(true);
		setStateFileModified(true);
	}
	else{
		//ERROR
		delete db;
		StatusBarGeneral->setText(tr("Loading Failed"));
		if(err=="")err=tr("Unknown error in Import_KWalletXml::importFile()");
		QMessageBox::critical(this,tr("Error")
								,tr("The following error occured while opening the database:\n%1")
								.arg(err),tr("OK"));
	}
	StatusBarGeneral->setText(tr("Ready"));
	}
}

void KeepassMainWindow::OnCurrentGroupChanged(QTreeWidgetItem* cur,QTreeWidgetItem* prev){
if(cur){
 if(GroupView->isSearchResultGroup((GroupViewItem*)cur)){
	EntryView->showSearchResults(SearchResults);
 }
 else EntryView->updateItems(((GroupViewItem*)cur)->pGroup->ID);
}
}

void KeepassMainWindow::OnEntryItemDoubleClicked(QTreeWidgetItem* item,int column){
int i=0;
int c=-1;
for(i;i<NUM_COLUMNS;i++){
	if(config.Columns[i])c++;
	if(c==column)break;
}
if(c==-1)return;
CEntry* entry=static_cast<EntryViewItem*>(item)->pEntry;
switch(i){
	case 0:	editEntry(entry);
			break;
	case 1: OnEditUsernameToClipboard();
			break;
	case 2: OnEditOpenUrl();
			break;
	case 3:	OnEditPasswordToClipboard();
			break;
}
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

while(EntryView->selectedItems().size()){
	EntryView->setItemSelected(EntryView->selectedItems()[0],false);}

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
CEditGroupDialog dlg(db,this,"EditGroupDlg",true);
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
CEditGroupDialog dlg(db,this,"EditGroupDlg",true);
dlg.GroupName=pGroup->Name;
dlg.IconID=pGroup->ImageID;
if(!dlg.exec())return;
if((pGroup->Name != dlg.GroupName) || (pGroup->ImageID != dlg.IconID) || dlg.ModFlag){
  setStateFileModified(true);
  pGroup->Name = dlg.GroupName;
  if(pGroup->ImageID<BUILTIN_ICONS && dlg.IconID>=BUILTIN_ICONS)
	pGroup->OldImgID=pGroup->ImageID;
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
 EntryView->updateItems(currentGroup()->ID);
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
EntryView->updateItems(currentGroup()->ID);
}

void KeepassMainWindow::OnEditDeleteEntry(){
Q_ASSERT(EntryView->selectedItems().size()>0);
QList<QTreeWidgetItem*> entries=EntryView->selectedItems();
for(int i=0; i<entries.size();i++){
	removeFromSearchResults(((EntryViewItem*)entries[i])->pEntry->sID);
	db->deleteEntry(((EntryViewItem*)entries[i])->pEntry);
}
setStateFileModified(true);
if(GroupView->isSearchResultGroup((GroupViewItem*)(GroupView->selectedItems()[0])))
	EntryView->showSearchResults(SearchResults);
else
	EntryView->updateItems(currentGroup()->ID);
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
currentEntry()->Password.unlock();
Clipboard->setText(currentEntry()->Password.string(),QClipboard::Clipboard);
ClipboardTimer.start(config.ClipboardTimeOut*1000,true);
currentEntry()->Password.lock();

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
return ((GroupViewItem*)(GroupView->selectedItems()[0]))->pGroup;
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
EntryView->refreshItems();
}

void KeepassMainWindow::OnFileModified(){
setStateFileModified(true);
}

void KeepassMainWindow::closeEvent(QCloseEvent* e){
config.MainWinHeight=geometry().height();
config.MainWinWidth=geometry().width();
config.MainWinSplit1=splitter->sizes()[0];
config.MainWinSplit2=splitter->sizes()[1];
config.ShowStatusbar=statusBar()->isVisible();

if(FileOpen){
 if(!closeDatabase())
 	e->ignore();
 else
	e->accept();
}
else
 e->accept();
}

void KeepassMainWindow::OnHideSearchGroup(){
GroupView->ShowSearchGroup=false;
GroupView->updateItems();
EntryView->updateItems(0);
SearchResults.clear();
}


void KeepassMainWindow::OnExtrasSettings(){
CSettingsDlg dlg(this);
dlg.exec();
EntryView->setAlternatingRowColors(config.AlternatingRowColors);
CGroup::UI_ExpandByDefault=config.ExpandGroupTree;
}

void KeepassMainWindow::OnHelpAbout(){
CAboutDialog dlg(this,"AboutDlg");
dlg.exec();
}

void KeepassMainWindow::OnHelpHandbook(){
openBrowser(AppDir+"/../share/doc/keepass/index.html");
}

void KeepassMainWindow::OnViewShowToolbar(bool show){
config.Toolbar=show;
toolBar->setVisible(config.Toolbar);
}

void KeepassMainWindow::OnViewShowEntryDetails(bool show){
config.EntryDetails=show;
DetailView->setVisible(config.EntryDetails);
}

void KeepassMainWindow::OnItemExpanded(QTreeWidgetItem* item){
((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=true;
}

void KeepassMainWindow::OnItemCollaped(QTreeWidgetItem* item){
((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=false;
}

void KeepassMainWindow::OnEditAutoType(){
Q_ASSERT(EntryView->selectedItems().size()==1);
QString error;
AutoType::perform(((EntryViewItem*)(EntryView->selectedItems()[0]))->pEntry,error);
}

void KeepassMainWindow::OnViewToolbarIconSize16(bool state){
if(!state)return;
ViewToolButtonSize22Action->setChecked(false);
ViewToolButtonSize28Action->setChecked(false);
config.ToolbarIconSize=16;
toolBar->setIconSize(QSize(config.ToolbarIconSize,config.ToolbarIconSize));
}

void KeepassMainWindow::OnViewToolbarIconSize22(bool state){
if(!state)return;
ViewToolButtonSize16Action->setChecked(false);
ViewToolButtonSize28Action->setChecked(false);
config.ToolbarIconSize=22;
toolBar->setIconSize(QSize(config.ToolbarIconSize,config.ToolbarIconSize));
}

void KeepassMainWindow::OnViewToolbarIconSize28(bool state){
if(!state)return;
ViewToolButtonSize16Action->setChecked(false);
ViewToolButtonSize22Action->setChecked(false);
config.ToolbarIconSize=28;
toolBar->setIconSize(QSize(config.ToolbarIconSize,config.ToolbarIconSize));
}