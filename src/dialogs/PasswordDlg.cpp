/***************************************************************************
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

#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QStringList>
#include <QPainter>
#include <QPalette>
#include <QMenu>

#include "main.h"
#include "KpxConfig.h"
#include "PasswordDlg.h"
#include "lib/FileDialogs.h"
#include "lib/bookmarks.h"


CPasswordDialog::CPasswordDialog(QWidget* parent,QString filename,IDatabase* DB,bool IsAuto,bool ChangeKeyMode)
: QDialog(parent)
{
	setupUi(this);
	createBanner(&BannerPixmap,getPixmap("key"),tr("Database Key"),width());
	Button_Bookmarks->setIcon(getIcon("bookmark"));
	db=DB;
	LastFile=filename;
	setWindowTitle(LastFile);
	QString mountDir=config->mountDir();
	QDir media(mountDir);
	if(media.exists()){
		QStringList Paths;
		Paths=media.entryList(QStringList()<<"*",QDir::Dirs | QDir::NoDotAndDotDot);
		for(int i=0;i<Paths.count();i++)
			Combo_Dirs->addItem(mountDir+Paths[i]);
	}

	Combo_Dirs->setEditText(QString());
	if(config->rememberLastKey() && !ChangeKeyMode){
		switch(config->lastKeyType()){
			case KEYFILE:
				setStateKeyFileOnly();
				Combo_Dirs->setEditText(QDir::cleanPath(QDir::current().absoluteFilePath(config->lastKeyLocation())));
				break;

			case BOTH:
				setStateBoth();
				CheckBox_Both->setChecked(true);
				Combo_Dirs->setEditText(QDir::cleanPath(QDir::current().absoluteFilePath(config->lastKeyLocation())));
		}
		// if(LastKeyType==Password){... is not required because it is already the default state.
	}
	
	// Bookmarks //
	QPalette palette=Button_Bookmarks->palette();
	palette.setColor(QPalette::Active,QPalette::Button,config->bannerColor1());
	palette.setColor(QPalette::Active,QPalette::Window,config->bannerColor2());
	Button_Bookmarks->setPalette(palette);
	palette=Label_Bookmark->palette();
	palette.setColor(QPalette::Active,QPalette::WindowText,config->bannerTextColor());
	Label_Bookmark->setPalette(palette);
	
	QMenu* BookmarkMenu=new QMenu(this);
	QAction* action=new QAction(this);
	action->setData(QString());
	action->setText(tr("Last File"));
	action->setIcon(getIcon("document"));
	BookmarkMenu->addAction(action);
	BookmarkMenu->addSeparator();	
	for(int i=0;i<KpxBookmarks::count();i++){
		QAction* action=new QAction(this);
		action->setData(KpxBookmarks::path(i));
		action->setText(KpxBookmarks::title(i));
		action->setIcon(getIcon("document"));
		BookmarkMenu->addAction(action);		
	}
	Button_Bookmarks->setMenu(BookmarkMenu);
	if(!IsAuto || !config->featureBookmarks()){
		Button_Bookmarks->hide();
		Label_Bookmark->hide();
	}
			
	connect(Combo_Dirs, SIGNAL( editTextChanged(const QString&) ),this, SLOT( OnComboTextChanged(const QString&)));
	connect(ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
	connect(Edit_Password, SIGNAL( textChanged(const QString&) ), this, SLOT( OnPasswordChanged(const QString&) ) );
	connect(CheckBox_Both, SIGNAL( stateChanged(int) ), this, SLOT( OnCheckBox_BothChanged(int) ) );
	connect(ButtonChangeEchoMode, SIGNAL( clicked() ), this, SLOT( ChangeEchoModeDatabaseKey() ) );
	connect(Edit_Password, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
	connect(Edit_PasswordRep, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
	connect(ButtonExit, SIGNAL( clicked()),this,SLOT(OnButtonExit()));
	connect(BookmarkMenu,SIGNAL(triggered(QAction*)),this,SLOT(OnBookmarkTriggered(QAction*)));

	ButtonExit->setVisible(IsAuto);
	Mode_Set=ChangeKeyMode;
	if(!ChangeKeyMode){
		Edit_PasswordRep->hide();
		Label_PasswordRep->hide();
		connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK() ) );
		connect( ButtonBrowse, SIGNAL( clicked() ), this, SLOT( OnButtonBrowse() ) );
	}else{
		connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK_Set() ) );
		connect( ButtonBrowse, SIGNAL( clicked() ), this, SLOT( OnButtonBrowse_Set() ) );
	}

	if(!config->showPasswordsPasswordDlg())ChangeEchoModeDatabaseKey();
}


void CPasswordDialog::setStatePasswordOnly(){
	Combo_Dirs->setEnabled(false);
	ButtonBrowse->setEnabled(false);
	Label_KeyFile->setEnabled(false);
	Label_Password->setEnabled(true);
	Label_PasswordRep->setEnabled(true);
	Edit_Password->setEnabled(true);
	Edit_PasswordRep->setEnabled(true);
	ButtonChangeEchoMode->setEnabled(true);
	KeyType=PASSWORD;
}


void CPasswordDialog::setStateKeyFileOnly(){
	Combo_Dirs->setEnabled(true);
	ButtonBrowse->setEnabled(true);
	Label_KeyFile->setEnabled(true);
	Label_Password->setEnabled(false);
	Label_PasswordRep->setEnabled(false);
	Edit_Password->setEnabled(false);
	Edit_PasswordRep->setEnabled(false);
	ButtonChangeEchoMode->setEnabled(false);
	KeyType=KEYFILE;
}


void CPasswordDialog::setStateBoth(){
	Combo_Dirs->setEnabled(true);
	ButtonBrowse->setEnabled(true);
	Label_KeyFile->setEnabled(true);
	Label_Password->setEnabled(true);
	Label_PasswordRep->setEnabled(true);
	Edit_Password->setEnabled(true);
	Edit_PasswordRep->setEnabled(true);
	ButtonChangeEchoMode->setEnabled(true);
	KeyType=BOTH;
}


void CPasswordDialog::OnButtonBrowse()
{
	QString filename=KpxFileDialogs::openExistingFile(this,"PasswordDlg",tr("Select a Key File"),
													  QStringList() << tr("All Files (*)") << tr("Key Files (*.key)"));
	if(filename!=QString()){
		Combo_Dirs->setEditText(filename);
	}
	return;
}

void CPasswordDialog::OnButtonBrowse_Set()
{
	QString filename=KpxFileDialogs::saveFile(this,"PasswordDlg",tr("Select a Key File"),
											  QStringList() << tr("All Files (*)") << tr("Key Files (*.key)"),
											  false);
	if(filename!=QString()){
		Combo_Dirs->setEditText(filename);
	}
	return;
}

void CPasswordDialog::OnCancel()
{
	done(0);
}

void CPasswordDialog::OnOK(){
	password=Edit_Password->text();
	keyfile=Combo_Dirs->currentText();

	if(password.isEmpty() && keyfile.isEmpty()){
		QMessageBox::warning(this,tr("Error"),tr("Please enter a Password or select a key file."),tr("OK"),"","",0,0);
		return;
	}

	if(KeyType==BOTH){
		if(password.isEmpty()){
			QMessageBox::warning(this,tr("Error"),tr("Please enter a Password."),tr("OK"),"","",0,0);
			return;}
		if(keyfile.isEmpty()){
			QMessageBox::warning(this,tr("Error"),tr("Please choose a key file."),tr("OK"),"","",0,0);
			return;}
	}

	if(KeyType==BOTH || KeyType==KEYFILE){
		QFileInfo fileinfo(keyfile);
		if(!fileinfo.exists()){
			QMessageBox::warning(this,tr("Error"),tr("The selected key file or directory does not exist."),tr("OK"),"","",0,0);
			return;
		}
		if(!fileinfo.isReadable()){
			QMessageBox::warning(this,tr("Error"),tr("The selected key file or directory is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
			return;
		}
		if(fileinfo.isDir()){
			if(keyfile.right(1)!="/")keyfile+="/";
			QFile file(keyfile+"pwsafe.key");
			if(!file.exists()){
				QDir dir(keyfile);
				QStringList files;
				files=dir.entryList(QStringList()<<"*.key",QDir::Files);
				if(!files.size()){
					QMessageBox::warning(this,tr("Error"),tr("The given directory does not contain any key files."),tr("OK"),"","",0,0);
					return;}
				if(files.size()>1){
					QMessageBox::warning(this,tr("Error"),tr("The given directory contains more then one key file.\nPlease specify the key file directly."),tr("OK"),"","",0,0);
					return;}
				QFile file(keyfile+files[0]);
				Q_ASSERT(file.exists());
				if(!QFileInfo(file).isReadable()){
					QMessageBox::warning(this,tr("Error"),tr("The key file found in the given directory is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
					return;}
				keyfile+=files[0];
				}
			else{ /* pwsafe.key exists */
				if(!QFileInfo(file).isReadable()){
					QMessageBox::warning(this,tr("Error"),tr("The key file found in the given directory is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
					return;}
				keyfile+="pwsafe.key";
				}
		}
		else{ /* not a directory */
			QFile file(keyfile);
			if(!file.exists()){
				QMessageBox::warning(this,tr("Error"),tr("Key file could not be found."),tr("OK"),"","",0,0);
				return;}
			if(!QFileInfo(file).isReadable()){
				QMessageBox::warning(this,tr("Error"),tr("Key file is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
				return;}
		}

	}
if(doAuth())done(1);
}

void CPasswordDialog::OnOK_Set(){
	password=Edit_Password->text();
	if(password!=Edit_PasswordRep->text()){
		QMessageBox::warning(this,tr("Warning"),tr("Password an password repetition are not equal.\nPlease check your input."),tr("OK"),"","",0,0);
		return;
	}
	keyfile=Combo_Dirs->currentText();
	if(password.isEmpty() && keyfile.isEmpty()){
		QMessageBox::warning(this,tr("Error"),tr("Please enter a password or select a key file."),tr("OK"),"","",0,0);
		return;
	}

	if(!keyfile.isEmpty()){
		QFile file(keyfile);
		if(QFileInfo(file).isDir()){
			if(keyfile.right(1)!="/")keyfile+="/";
			keyfile+="pwsafe.key";
		}
		if(file.exists()){
			switch(QMessageBox::question(this,tr("File exists."),tr("A file with the selected name already exists, should this file be used as key file or do you want to overwrite it with a newly generated one?"),
					tr("Use"),tr("Overwrite"),tr("Cancel"),0,2)){
							case 0:
								OverwriteKeyFile=false;
								break;
							case 1:
								OverwriteKeyFile=true;
								break;
							case 2:
								return;
			}
		}
		IFilePasswordAuth* DbAuth=dynamic_cast<IFilePasswordAuth*>(db);
		if(OverwriteKeyFile){
			if(!DbAuth->createKeyFile(keyfile,32,true)){
				QMessageBox::warning(this,tr("Error"),tr("Key file could not be created.\n%1").arg(db->getError()),tr("OK"),"","",0,0);
				return;
			}
		}
	}
	if(doAuth())done(1);
}

bool CPasswordDialog::doAuth(){
	IFilePasswordAuth* DbAuth=dynamic_cast<IFilePasswordAuth*>(db);
	if(!password.isEmpty() && keyfile.isEmpty()){
		DbAuth->authByPwd(password);
	}
	if(password.isEmpty() && !keyfile.isEmpty()){
		if(!DbAuth->authByFile(keyfile))return false;
	}
	if(!password.isEmpty() && !keyfile.isEmpty()){
		if(!DbAuth->authByFile(keyfile))return false;
	}

	if(config->rememberLastKey()){
		QString KeyLocation=keyfile;
		if(config->saveRelativePaths()){
			KeyLocation=KeyLocation.left(KeyLocation.lastIndexOf("/"));
			KeyLocation=makePathRelative(KeyLocation,QDir::currentPath())+keyfile.right(keyfile.length()-keyfile.lastIndexOf("/")-1);
		}
		config->setLastKeyLocation(KeyLocation);
		config->setLastKeyType(KeyType);
	}
	return true;

}

void CPasswordDialog::OnPasswordChanged(const QString &txt){
Edit_PasswordRep->setText(QString());
if(CheckBox_Both->isChecked() || txt.isEmpty())
	setStateBoth();
else
	setStatePasswordOnly();
}

void CPasswordDialog::OnComboTextChanged(const QString& txt){
if(CheckBox_Both->isChecked() || txt.isEmpty())
	setStateBoth();
else
	setStateKeyFileOnly();
}



void CPasswordDialog::OnCheckBox_BothChanged(int state){
if(state==Qt::Checked)
	setStateBoth();
if(state==Qt::Unchecked){
	if(!Edit_Password->text().isEmpty() && !Combo_Dirs->currentText().isEmpty()){
		Combo_Dirs->setEditText(QString());
		setStatePasswordOnly();
	}
	else{
		if(Edit_Password->text().isEmpty())
			setStateKeyFileOnly();
		else
			setStatePasswordOnly();
	}
}

}

void CPasswordDialog::ChangeEchoModeDatabaseKey(){
if(Edit_Password->echoMode()==QLineEdit::Normal){
	Edit_Password->setEchoMode(QLineEdit::Password);
	Edit_PasswordRep->setEchoMode(QLineEdit::Password);}
else{
	Edit_Password->setEchoMode(QLineEdit::Normal);
	Edit_PasswordRep->setEchoMode(QLineEdit::Normal);}
}


void CPasswordDialog::OnButtonExit(){
	done(2);
}

void CPasswordDialog::paintEvent(QPaintEvent* event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);	
}

void CPasswordDialog::OnBookmarkTriggered(QAction* action){
	BookmarkFilename=action->data().toString();
	if(action->data().toString()==QString())
		setWindowTitle(LastFile);
	else
		setWindowTitle(action->data().toString());
	Label_Bookmark->setText(action->text());
}
