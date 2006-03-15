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

#include "main.h"
#include "PwmConfig.h"
#include "PasswordDlg.h"
#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>


CPasswordDialog::CPasswordDialog(QWidget* parent, const char* name, bool modal, bool ShowExitButton,bool ChangeKeyMode, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
createBanner(Banner,Icon_Key32x32,tr("Database Key"));
connect( Combo_Dirs, SIGNAL( editTextChanged(const QString&) ),this, SLOT( OnComboTextChanged(const QString&)));
connect( ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
connect( Edit_Password, SIGNAL( textChanged(const QString&) ), this, SLOT( OnPasswordChanged(const QString&) ) );
connect( CheckBox_Both, SIGNAL( stateChanged(int) ), this, SLOT( OnCheckBox_BothChanged(int) ) );
connect( ButtonChangeEchoMode, SIGNAL( clicked() ), this, SLOT( ChangeEchoMode() ) );
connect( Edit_Password, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
connect( ButtonExit, SIGNAL( clicked()),this,SLOT(OnButtonExit()));

ButtonExit->setVisible(ShowExitButton);
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



QDir media(config.MountDir);
if(media.exists()){
	QStringList Paths;
	Paths=media.entryList("*",QDir::Dirs);
	Paths.erase(Paths.begin()); // delete "."
	Paths.erase(Paths.begin()); // delete ".."
	for(int i=0;i<Paths.count();i++)
		Combo_Dirs->addItem(config.MountDir+Paths[i]);
}

Combo_Dirs->setEditText(QString());
if(!config.ShowPasswords)ChangeEchoMode();
}


void CPasswordDialog::setStatePasswordOnly(){
Combo_Dirs->setEnabled(false);
ButtonBrowse->setEnabled(false);
Label_KeyFile->setEnabled(false);
Label_Password->setEnabled(true);
Edit_Password->setEnabled(true);
ButtonChangeEchoMode->setEnabled(true);
KeyType=PASSWORD;
}


void CPasswordDialog::setStateKeyFileOnly(){
Combo_Dirs->setEnabled(true);
ButtonBrowse->setEnabled(true);
Label_KeyFile->setEnabled(true);
Label_Password->setEnabled(false);
Edit_Password->setEnabled(false);
ButtonChangeEchoMode->setEnabled(false);
KeyType=KEYFILE;
}


void CPasswordDialog::setStateBoth(){
Combo_Dirs->setEnabled(true);
ButtonBrowse->setEnabled(true);
Label_KeyFile->setEnabled(true);
Label_Password->setEnabled(true);
Edit_Password->setEnabled(true);
ButtonChangeEchoMode->setEnabled(true);
KeyType=BOTH;
}


void CPasswordDialog::OnButtonBrowse()
{
QString filename=QFileDialog::getOpenFileName(this,tr("Select a Key File"),QDir::homeDirPath(),tr("*.key"));
if(filename=="")return;
QFile file(filename);
if(file.exists()){
	Combo_Dirs->setEditText(filename);
	return;
}
QMessageBox::warning(this,tr("Error"),tr("Unexpected Error: File does not exist."),tr("OK"),"","",0,0);
}

void CPasswordDialog::OnButtonBrowse_Set()
{
QString filename=QFileDialog::getSaveFileName(this,tr("Select a Key File"),QDir::homeDirPath(),tr("*.key"));
if(filename=="")return;
Combo_Dirs->setEditText(filename);
}

void CPasswordDialog::OnCancel()
{
done(0);
}

void CPasswordDialog::OnOK(){
password=Edit_Password->text();
keyfile=Combo_Dirs->currentText();

if(password=="" && keyfile==""){
	QMessageBox::warning(this,tr("Error"),tr("Please enter a Password or select a key file."),tr("OK"),"","",0,0);
	return;}

if(KeyType==BOTH){
	if(password==""){
		QMessageBox::warning(this,tr("Error"),tr("Please enter a Password."),tr("OK"),"","",0,0);
		return;}
	if(keyfile==""){
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
		QMessageBox::warning(this,tr("Error"),tr("The selected key file or directory is not readable\n.Please check your permissions."),tr("OK"),"","",0,0);
		return;
	}
	if(fileinfo.isDir()){
		if(keyfile.right(1)!="/")keyfile+="/";
		QFile file(keyfile+"pwsafe.key");
		if(!file.exists()){				
			QDir dir(keyfile);
			QStringList files;
			files=dir.entryList("*.key",QDir::Files);
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
		}else{
			if(!QFileInfo(file).isReadable()){
				QMessageBox::warning(this,tr("Error"),tr("The key file found in the given directory is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
				return;}			
			keyfile+="pwsafe.key";
		}
	}else{
	QFile file(keyfile);
	if(!file.exists()){
		QMessageBox::warning(this,tr("Error"),tr("Key file could not be found."),tr("OK"),"","",0,0);
		return;}
	if(!QFileInfo(file).isReadable()){
		QMessageBox::warning(this,tr("Error"),tr("Key file is not readable.\nPlease check your permissions."),tr("OK"),"","",0,0);
		return;}
	}
}
done(1);
}

void CPasswordDialog::OnOK_Set(){



}

void CPasswordDialog::OnPasswordChanged(const QString &txt){
if(CheckBox_Both->isChecked() || txt==QString())
	setStateBoth();
else
	setStatePasswordOnly();
}

void CPasswordDialog::OnComboTextChanged(const QString& txt){
if(CheckBox_Both->isChecked() || txt==QString())
	setStateBoth();
else
	setStateKeyFileOnly();
}



void CPasswordDialog::OnCheckBox_BothChanged(int state){
if(state==Qt::Checked)
	setStateBoth();
if(state==Qt::Unchecked){
	if(Edit_Password->text()!=QString() && Combo_Dirs->currentText()!=QString()){
		Combo_Dirs->setEditText(QString());
		setStatePasswordOnly();
	}
	else{
		if(Edit_Password->text()==QString())
			setStateKeyFileOnly();
		else
			setStatePasswordOnly();
	}

}

}

void CPasswordDialog::ChangeEchoMode(){
if(Edit_Password->echoMode()==QLineEdit::Normal)
	Edit_Password->setEchoMode(QLineEdit::Password);
else
	Edit_Password->setEchoMode(QLineEdit::Normal);
}


void CPasswordDialog::OnButtonExit(){
	done(2);
}

