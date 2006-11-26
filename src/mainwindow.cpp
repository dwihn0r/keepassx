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
#include "lib/FileDialogs.h"
#include "import/Import_PwManager.h"
#include "import/Import_KWalletXml.h"
#include "export/Export_Txt.h"

#include "dialogs/AboutDlg.h"
#include "dialogs/SearchDlg.h"
#include "dialogs/SettingsDlg.h"
#include "dialogs/DatabaseSettingsDlg.h"
#include "dialogs/PasswordDlg.h"
#include "dialogs/SimplePasswordDlg.h"
#include "dialogs/PasswordGenDlg.h"
#include "dialogs/CollectEntropyDlg.h"


KeepassMainWindow::KeepassMainWindow(const QString& ArgFile,QWidget *parent, Qt::WFlags flags):QMainWindow(parent,flags){
  Start=true;
  ShutingDown=false;
  setupUi(this);
  AutoType::MainWin=this;
  setGeometry(geometry().x(),geometry().y(),config.MainWinWidth,config.MainWinHeight);
  splitter->setSizes(QList<int>() << config.MainWinSplit1 << config.MainWinSplit2);
  SysTray=new QSystemTrayIcon(this);
  setupToolbar();
  setupIcons();
  setStateFileOpen(false);
  setupMenus();
  StatusBarGeneral=new QLabel(tr("Ready"),statusBar());
  StatusBarSelection=new QLabel(statusBar());
  statusBar()->addWidget(StatusBarGeneral,15);
  statusBar()->addWidget(StatusBarSelection,85);
  statusBar()->setVisible(config.ShowStatusbar);
//  Group::UI_ExpandByDefault=config.ExpandGroupTree;
  setupConnections();
  FileOpen=false;
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
	connect(FileNewAction, SIGNAL(triggered()), this, SLOT(OnFileNewKdb()));
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
	
	connect(EditNewGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnNewGroup()));
	connect(EditEditGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnEditGroup()));
	connect(EditDeleteGroupAction, SIGNAL(triggered()), GroupView, SLOT(OnDeleteGroup()));
	connect(EditNewEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnNewEntry()));
	connect(EditEditEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnEditEntry()));
	connect(EditCloneEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnCloneEntry()));
	connect(EditDeleteEntryAction, SIGNAL(triggered()), EntryView, SLOT(OnDeleteEntry()));
	connect(EditUsernameToClipboardAction, SIGNAL(triggered()), EntryView, SLOT(OnUsernameToClipboard()));
	connect(EditPasswordToClipboardAction, SIGNAL(triggered()), EntryView, SLOT(OnPasswordToClipboard()));
	connect(EditOpenUrlAction, SIGNAL(triggered()), this, SLOT(OnEditOpenUrl()));
	connect(EditSaveAttachmentAction, SIGNAL(triggered()),EntryView, SLOT(OnSaveAttachment()));
	connect(EditSearchAction, SIGNAL(triggered()), this, SLOT(OnSearch()));
	connect(EditGroupSearchAction, SIGNAL(triggered()), this, SLOT(OnGroupSearch()));
	connect(EditAutoTypeAction,SIGNAL(triggered()),EntryView,SLOT(OnAutoType()));
	
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
	
	connect(EntryView,SIGNAL(itemActivated(QTreeWidgetItem*,int)),EntryView,SLOT(OnEntryActivated(QTreeWidgetItem*,int)));
	connect(QuickSearchEdit,SIGNAL(returnPressed()), this, SLOT(OnQuickSearch()));
	connect(GroupView,SIGNAL(groupChanged(IGroupHandle*)),EntryView,SLOT(OnGroupChanged(IGroupHandle*)));
	connect(GroupView,SIGNAL(groupChanged(IGroupHandle*)),this,SLOT(OnGroupSelectionChanged(IGroupHandle*)));
	connect(GroupView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
	connect(EntryView,SIGNAL(fileModified()),this,SLOT(OnFileModified()));
	connect(EntryView,SIGNAL(selectionChanged(SelectionState)),this,SLOT(OnEntryChanged(SelectionState)));
	connect(GroupView,SIGNAL(searchResultsSelected()),EntryView,SLOT(OnShowSearchResults()));
	connect(GroupView,SIGNAL(searchResultsSelected()),this,SLOT(OnShowSearchResults()));
	connect(HideSearchResultsAction,SIGNAL(triggered()),GroupView,SLOT(OnHideSearchResults()));
	
	connect(SysTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(OnSysTrayActivated(QSystemTrayIcon::ActivationReason)));

}

void KeepassMainWindow::setupToolbar(){
	toolBar=new QToolBar(this);
	addToolBar(toolBar);
	toolBar->setIconSize(QSize(config.ToolbarIconSize,config.ToolbarIconSize));
	ViewShowToolbarAction=toolBar->toggleViewAction();
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
	SysTray->setIcon(QIcon(AppDir+"/../share/keepass/icons/keepassx_large.png"));
	SysTray->show();
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
	
	ViewShowToolbarAction->setText(tr("Show Toolbar"));
	ViewMenu->insertAction(ViewShowEntryDetailsAction,ViewShowToolbarAction);
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
	
	//FileNewMenu->setShortcut(tr("Ctrl+N"));
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

void KeepassMainWindow::setupDatabaseConnections(IDatabase* DB){
	ICustomIcons* CustomIconsDb=dynamic_cast<ICustomIcons*>(DB);
	if(CustomIconsDb){
		connect(CustomIconsDb,SIGNAL(iconsModified()),this,SLOT(OnFileModified()));
		connect(CustomIconsDb,SIGNAL(iconsModified()),EntryView,SLOT(updateIcons()));
		connect(CustomIconsDb,SIGNAL(iconsModified()),GroupView,SLOT(updateIcons()));
	}
}


void KeepassMainWindow::openDatabase(QString filename,bool IsAuto){
	Q_ASSERT(!FileOpen);
	if(!IsAuto){
		config.LastKeyLocation=QString();
		config.LastKeyType=PASSWORD;}
	CPasswordDialog PasswordDlg(this,true,IsAuto);
	PasswordDlg.setWindowTitle(filename);
	int r=PasswordDlg.exec();
	if(r==0) return;
	if(r==2) {Start=false; return;}
	Q_ASSERT(r==1);
	db = new StandardDatabase();
	GroupView->db=db;
	EntryView->db=db;
	setupDatabaseConnections(db);
	IFilePasswordAuth* auth=dynamic_cast<IFilePasswordAuth*>(db);
	if(PasswordDlg.password!="" && PasswordDlg.keyfile=="")
		auth->authByPwd(PasswordDlg.password);
	if(PasswordDlg.password=="" && PasswordDlg.keyfile!=""){
		QFile keyfile(PasswordDlg.keyfile);
		if(!keyfile.open(QIODevice::ReadOnly)){
			delete db;
			QMessageBox::critical(this,tr("Error"),tr("Could not open key file."),tr("OK"));
			return;		
		}
		if(!auth->authByFile(keyfile)){
			QMessageBox::critical(this,tr("Error"),db->getError(),tr("OK"));
			delete db;
			return;			
		}	
	}
	if(PasswordDlg.password!="" && PasswordDlg.keyfile!=""){
		QFile keyfile(PasswordDlg.keyfile);
		if(!keyfile.open(QIODevice::ReadOnly)){
			delete db;
			QMessageBox::critical(this,tr("Error"),tr("Could not open key file."),tr("OK"));
			return;		
		}
		if(!auth->authByFileAndPwd(PasswordDlg.password,keyfile)){
			QMessageBox::critical(this,tr("Error"),db->getError(),tr("OK"));
			delete db;
			return;			
		}
	}		
	QString err;
	StatusBarGeneral->setText(tr("Loading Database..."));
	if(db->load(filename)==true){
		if(config.OpenLast)config.LastFile=filename;
		setWindowTitle(tr("%1 - KeePassX").arg(filename));
		GroupView->createItems();
		EntryView->showGroup(NULL);
		setStateFileOpen(true);
		setStateFileModified(false);
	}
	else{
		StatusBarGeneral->setText(tr("Loading Failed"));
		QString error=db->getError();
		bool KeyError=auth->isKeyError();
		delete db;
		if(error==QString())error=tr("Unknown error while loading database.");
		QMessageBox::critical(this,tr("Error")
								,tr("The following error occured while opening the database:\n%1")
								.arg(error),tr("OK"));
		if(KeyError)
			openDatabase(filename,IsAuto);
	}
	StatusBarGeneral->setText(tr("Ready"));
}


bool KeepassMainWindow::closeDatabase(){
	Q_ASSERT(FileOpen);
	Q_ASSERT(db!=NULL);
	if(ModFlag){
	int r=QMessageBox::question(this,tr("Save modified file?"),
					tr("The current file was modified. Do you want\nto save the changes?"),tr("Yes"),tr("No"),tr("Cancel"),2,2);
	if(r==2)return false;	//Abbrechen
	if(r==0)				//Ja (Datei speichern)
	if(!OnFileSave())return false;
	}
	db->close();
	delete db;
	db=NULL;
	EntryView->db=NULL;
	EntryView->clear();
	EntryView->Items.clear();
	GroupView->db=NULL;
	GroupView->clear();
	GroupView->Items.clear();
	SearchResults.clear();
	setStateFileOpen(false);
	setWindowTitle("KeePassX Password Manager");
	return true;
}


void KeepassMainWindow::OnFileNewKdb(){
	CPasswordDialog dlg(this,true,false,true);
	dlg.setWindowTitle("New Database");
	if(dlg.exec()==1){
		if(FileOpen)
			if(!closeDatabase())return;
		db=new StandardDatabase();
		db->create();
		IFilePasswordAuth* DbAuth=dynamic_cast<IFilePasswordAuth*>(db);
		if(dlg.KeyType==BOTH || dlg.KeyType==KEYFILE){
			bool KeyFileExists=QFileInfo(dlg.keyfile).exists();
			if((KeyFileExists && dlg.OverwriteKeyFile) || !KeyFileExists){
				if(!DbAuth->createKeyFile(dlg.keyfile)){					
					QMessageBox::warning(this,tr("Error"),tr("Could not create key file. The following error occured:\n%1").arg(db->getError()),tr("OK"),"","",0,0);
					delete db; db=NULL;
					return;
				}
			}
		}	
		if(dlg.password!="" && dlg.keyfile=="")
			DbAuth->authByPwd(dlg.password);
		if(dlg.password=="" && dlg.keyfile!=""){
			QFile keyfile(dlg.keyfile);
			if(!keyfile.open(QIODevice::ReadOnly)){
				delete db;
				QMessageBox::critical(this,tr("Error"),tr("Could not open key file."),tr("OK"));
				return;		
			}
			if(!DbAuth->authByFile(keyfile)){
				QMessageBox::critical(this,tr("Error"),db->getError(),tr("OK"));
				delete db;
				return;			
			}	
		}
		if(dlg.password!="" && dlg.keyfile!=""){
			QFile keyfile(dlg.keyfile);
			if(!keyfile.open(QIODevice::ReadOnly)){
				delete db;
				QMessageBox::critical(this,tr("Error"),tr("Could not open key file."),tr("OK"));
				return;		
			}
			if(!DbAuth->authByFileAndPwd(dlg.password,keyfile)){
				QMessageBox::critical(this,tr("Error"),db->getError(),tr("OK"));
				delete db;
				return;			
			}
		}		
		setWindowTitle(tr("%1 - KeePassX").arg(tr("[new]")));
		GroupView->db=db;
		EntryView->db=db;		
		GroupView->createItems();
		EntryView->showGroup(NULL);
		setStateFileOpen(true);
		setStateFileModified(true);
		FileOpen=true;
		setupDatabaseConnections(db);
		setStateGroupSelected(NONE);
		setStateEntrySelected(NONE);
	}	
	}

void KeepassMainWindow::OnFileNewKxdb(){
	
	
	
}


void KeepassMainWindow::OnFileOpen(){
	/*QFileDialog FileDlg(this,tr("Open Database..."),QDir::homePath());
	FileDlg.setFilters(QStringList()<< tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	FileDlg.setFileMode(QFileDialog::ExistingFile);
	FileDlg.setAcceptMode(QFileDialog::AcceptOpen);
	if(!FileDlg.exec())return;
	if(!FileDlg.selectedFiles().size())return;*/
	QString filename=KpxFileDialogs::openExistingFile(this,"MainWindow_FileOpen",
			tr("Open Database..."),QStringList()<<tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	if(filename==QString())return;
	if(FileOpen)
		if(!closeDatabase())return;
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
    //OnGroupSelectionChanged();
    //OnEntrySelectionChanged();
}

}


void KeepassMainWindow::setStateFileModified(bool mod){
	if(!FileOpen){
		FileSaveAction->setIcon(*Icon_FileSave);
		return;
	}
	ModFlag=mod;
	if(mod)
		FileSaveAction->setIcon(*Icon_FileSave);
	else
		FileSaveAction->setIcon(*Icon_FileSaveDisabled);
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
		DetailView->setPlainText("");
		return;
	}
	
	IEntryHandle* entry=((EntryViewItem*)(EntryView->selectedItems()[0]))->EntryHandle;
	QString str=tr("<B>Group: </B>%1 <B>Title: </B>%2  <B>Username: </B>%3  <B>URL: </B><a href=%4>%4</a>  <B>Password: </B>%5  <B>Creation: </B>%6  <B>Last Change: </B>%7  <B>LastAccess: </B>%8  <B>Expires: </B>%9");
	
	str=str.arg(entry->group()->title()).arg(entry->title());
	
	if(!config.ListView_HideUsernames)	str=str.arg(entry->username());
	else str=str.arg("****");
	
	str=str.arg(entry->url());
	
	if(!config.ListView_HidePasswords){	
		SecString password=entry->password();
		password.unlock();
		str=str.arg(password.string());
	}
	else str=str.arg("****");
	
	str=str	.arg(entry->creation().toString(Qt::LocalDate))
			.arg(entry->lastMod().toString(Qt::LocalDate))
			.arg(entry->lastAccess().toString(Qt::LocalDate))
			.arg(entry->expire().toString(Qt::LocalDate));
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
if(!db->file())
  return OnFileSaveAs();
config.LastFile=db->file()->fileName();
if(db->save())
  setStateFileModified(false);
else{
  showErrMsg(tr("File could not be saved.\n%1").arg(db->getError()));
  return false;  
}
return true;
}

bool KeepassMainWindow::OnFileSaveAs(){
	QString filename=KpxFileDialogs::saveFile(this,"MainWindow_FileSave",
			tr("Save Database..."),QStringList()<<tr("KeePass Databases (*.kdb)")<< tr("All Files (*)"));
	if(filename==QString())return false;
	if(!db->changeFile(filename)){
		showErrMsg(tr("File could not be saved.\n%1").arg(db->getError()));
		db->changeFile(QString());
		setWindowTitle(tr("KeePassX - [unsaved]").arg(filename));
		return false;		
	}
	setWindowTitle(tr("%1 - KeePassX").arg(filename));
	return OnFileSave();
}

void KeepassMainWindow::OnFileSettings(){
CDbSettingsDlg dlg(this,db);
if(dlg.exec())setStateFileModified(true);
}

void KeepassMainWindow::OnFileChangeKey(){
	/*
CPasswordDialog dlg(this,true,false,true);
dlg.setWindowTitle(db->file->fileName());
if(dlg.exec()==1){
	if(dlg.KeyType==BOTH || dlg.KeyType==KEYFILE){
		if(!db->createKeyFile(dlg.keyfile)){
			QMessageBox::warning(this,tr("Error"),tr("Could not create key file. The following error occured:\n%1").arg(db->getError()),tr("OK"),"","",0,0);
			return;
		}
	}
	IFilePasswordAuth* auth=dynamic_cast<IFilePasswordAuth*>db;
	if(dlg.password!="" && dlg.keyfile=="")
		auth->authByPwd(dlg.password);
	if(dlg.password=="" && dlg.keyfile!="")
		auth->authByFile(dlg.keyfile);
	if(dlg.password!="" && dlg.keyfile!="")
		auth->authByFileAndPw(dlg.keyfile,dlg.password);
	setStateFileModified(true);
}
	*/

}

void KeepassMainWindow::OnFileExit(){
	ShutingDown=true;
	close();
}

void KeepassMainWindow::OnExportToTxt(){
	/*
QString filename=QFileDialog::getSaveFileName(this,tr("Export To..."),QDir::homePath(),"*.txt");
if(filename==QString())return;
Export_Txt exp;
QString err;
exp.exportFile(filename,db,err);
	*/
}

void KeepassMainWindow::OnImportFromPwm(){
	/*
if(FileOpen)
 if(!closeDatabase())return;
QString filename=QFileDialog::getOpenFileName(this,tr("Open Database..."),QDir::homePath(),"*.pwm");
if(filename!=QString::null){
	Q_ASSERT(!FileOpen);
	db = new PwDatabase();
	db->newDatabase();
	CSimplePasswordDialog dlg(this,true);
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
		setWindowTitle(tr("KeePassX [new]"));
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
	*/
}

void KeepassMainWindow::OnImportFromKWalletXml(){
	/*
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
		setWindowTitle(tr("KeePassX [new]"));
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
	*/
}

/*
void KeepassMainWindow::OnEntrySelectionChanged(){
updateDetailView();
if(EntryView->selectedItems().size()==0)
  setStateEntrySelected(NONE);
if(EntryView->selectedItems().size()==1)
  setStateEntrySelected(SINGLE);
if(EntryView->selectedItems().size()>1)
  setStateEntrySelected(MULTIPLE);
}
*/






void KeepassMainWindow::removeFromSearchResults(int id){
	/*
for(int i=0; i<SearchResults.size();i++){
 if(SearchResults[i]==id){
	SearchResults.removeAt(i);
	return;
 }
}
	*/
}


void KeepassMainWindow::OnEditOpenUrl(){
//openBrowser(currentEntry()->URL);
}

void KeepassMainWindow::search(IGroupHandle* group){
	CSearchDlg dlg(db,group,this,"SearchDialog",false);
	if(dlg.exec()){
		EntryView->SearchResults=dlg.Result;
		GroupView->showSearchResults();
	}
}

void KeepassMainWindow::OnSearch(){
	search(NULL);
}

void KeepassMainWindow::OnGroupSearch(){
	Q_ASSERT(GroupView->currentItem());
	search(((GroupViewItem*)GroupView->currentItem())->GroupHandle);
}

void KeepassMainWindow::OnQuickSearch(){
	EntryView->SearchResults=db->search(NULL,QuickSearchEdit->text(),false,false,false,NULL);
	GroupView->showSearchResults();	
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
//EntryView->updateColumns();
//if(FileOpen) EntryView->updateItems();
}

void KeepassMainWindow::OnUsernPasswVisibilityChanged(bool value){
config.ListView_HidePasswords=ViewHidePasswordsAction->isChecked();
config.ListView_HideUsernames=ViewHideUsernamesAction->isChecked();
//EntryView->refreshItems();
}

void KeepassMainWindow::OnFileModified(){
setStateFileModified(true);
}

void KeepassMainWindow::closeEvent(QCloseEvent* e){
	if(!ShutingDown){
		e->ignore();
		hide();
		return;
	}
	
	config.MainWinHeight=geometry().height();
	config.MainWinWidth=geometry().width();
	config.MainWinSplit1=splitter->sizes()[0];
	config.MainWinSplit2=splitter->sizes()[1];
	config.ShowStatusbar=statusBar()->isVisible();
	
	if(FileOpen){
		if(!closeDatabase()){		
			ShutingDown=false;
			e->ignore();
			return;}
		else
			e->accept();
	}
	else
		e->accept();
	delete SysTray;
}


void KeepassMainWindow::OnExtrasSettings(){
CSettingsDlg dlg(this);
dlg.exec();
EntryView->setAlternatingRowColors(config.AlternatingRowColors);
//Group::UI_ExpandByDefault=config.ExpandGroupTree;
}

void KeepassMainWindow::OnHelpAbout(){
CAboutDialog dlg(this);
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
//((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=true;
}

void KeepassMainWindow::OnItemCollaped(QTreeWidgetItem* item){
//((GroupViewItem*)item)->pGroup->UI_ItemIsExpanded=false;
}

void KeepassMainWindow::OnGroupSelectionChanged(IGroupHandle* group){
	if(group)
		setStateGroupSelected(SINGLE);
	else
		setStateGroupSelected(NONE);	
}

void KeepassMainWindow::OnEntryChanged(SelectionState Selection){
	updateDetailView();
	setStateEntrySelected(Selection);
}

void KeepassMainWindow::OnShowSearchResults(){
	setStateGroupSelected(SEARCHGROUP);	
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

void KeepassMainWindow::OnSysTrayActivated(QSystemTrayIcon::ActivationReason reason){
	this->show();
}
