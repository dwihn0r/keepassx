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

#include "PasswordDlg.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <q3valuelist.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <q3filedialog.h>
#include <qmessagebox.h>



CPasswordDialog::CPasswordDialog(QWidget* parent, const char* name, bool modal, bool ShowExitButton, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
ButtonExit->setVisible(ShowExitButton);
createBanner(Banner,Icon_Key32x32,tr("Open Database"));
Label_select=new LinkLabel((QWidget*)groupframe,"Select",tr("Select File Manually..."),410,100);
connect( Combo_Dirs, SIGNAL( activated(int) ), this, SLOT( OnComboSelectionChanged(int) ) );
connect( ButtonBrowse, SIGNAL( clicked() ), this, SLOT( OnButtonBrowse() ) );
connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK() ) );
connect( ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
connect( Edit_Password, SIGNAL( textChanged(const QString&) ), this, SLOT( OnPasswordChanged(const QString&) ) );
connect( CheckBox_Both, SIGNAL( stateChanged(int) ), this, SLOT( OnCheckBox_BothChanged(int) ) );
connect( ButtonChangeEchoMode, SIGNAL( clicked() ), this, SLOT( ChangeEchoMode() ) );
connect( Edit_Password, SIGNAL( returnPressed() ), this, SLOT( OnOK() ) );
connect( ButtonExit, SIGNAL( clicked()),this,SLOT(OnButtonExit()));

QDir media("/media");
if(media.exists()){
Paths=media.entryList("*",QDir::Dirs);
Paths.erase(Paths.begin()); // delete "."
Paths.erase(Paths.begin()); // delete ".."

for(int i=0;i<Paths.count();i++){
Paths[i]="/media/"+Paths[i];
}
Paths.prepend(tr("< none >"));
}
for(int i=0;i<Paths.count();i++){
Combo_Dirs->insertItem(0,Paths[i]);
IsFile.append(false);
}

if(!config.ShowPasswords)ChangeEchoMode();

}

CPasswordDialog::~CPasswordDialog()
{
delete Label_select;
}

void CPasswordDialog::OnComboSelectionChanged(int i)
{
if(i==0){
keyfile="";
Edit_Password->setEnabled(true);
return;
}
QFile file;
if(IsFile[i]==true)file.setName(Paths[i]);
else file.setName(Paths[i]+"/pwsafe.key");

if(file.exists()){
keyfile=file.name();
if(!CheckBox_Both->isChecked()){
Edit_Password->setText("");
Edit_Password->setDisabled(true);}
return;
}
QMessageBox::warning(this,tr("No key file found"),tr(
"No key file could be found in the chosen directory.\n\
Make sure that the volume is mounted correctly.\n\
Please use the manual file selection for key files with a filename other than 'pwsafe.key'.")
,tr("OK"),"","",0,0);
Edit_Password->setEnabled(true);
Combo_Dirs->setCurrentItem(0);
return;
}


void CPasswordDialog::OnButtonBrowse()
{
QString dir=Q3FileDialog::getExistingDirectory(QDir::homeDirPath(),NULL,tr("Browse..."));
if(dir=="")return;
QFile file(dir+"/pwsafe.key");
if(file.exists()){
keyfile=dir+"/pwsafe.key";
Combo_Dirs->insertItem(dir);
Combo_Dirs->setCurrentItem(Combo_Dirs->count()-1);
if(!CheckBox_Both->isChecked()){
 Edit_Password->setDisabled(true);
 Edit_Password->setText("");
 password="";
}
Paths.append(dir);
IsFile.append(false);
return;}
QMessageBox::warning(this,tr("No key file found"),tr(
"No key file could be found in the chosen directory.\n\
Make sure that the volume is mounted correctly.\n\
Please use the manual file selection for key files with a filename other than 'pwsafe.key'.")
,tr("OK"),"","",0,0);
}

void CPasswordDialog::OnSelectClicked()
{
if(ButtonBrowse->isEnabled()){
keyfile=Q3FileDialog::getOpenFileName(QDir::homeDirPath(),"",this,tr("Open Key File"));
if(keyfile=="")return;
Combo_Dirs->insertItem(keyfile);
Combo_Dirs->setCurrentItem(Combo_Dirs->count()-1);
if(!CheckBox_Both->isChecked()){
 Edit_Password->setDisabled(true);
 Edit_Password->setText("");
 password="";
}
Paths.append(keyfile);
IsFile.append(true);
}
}

void CPasswordDialog::OnCancel()
{
done(0);
}

void CPasswordDialog::OnOK()
{

if(CheckBox_Both->isChecked()){
 if(password==""){QMessageBox::warning(this,tr("Error"),tr("Please enter a Password.")
                                      ,tr("OK"),"","",0,0);
			return;}
 if(keyfile==""){QMessageBox::warning(this,tr("Error"),tr("Please choose a key file.")
                                      ,tr("OK"),"","",0,0);
			return;}
}
else
{
 if(password=="" && keyfile==""){QMessageBox::warning(this,tr("Error")
									,tr("Please enter a Password or select a key file.")
                                    ,tr("OK"),"","",0,0);
			return;}
}
done(1);
}

void CPasswordDialog::OnPasswordChanged(const QString &txt)
{
password=Edit_Password->text();
if(txt!="" && !(CheckBox_Both->isChecked())){
Combo_Dirs->setDisabled(true);
ButtonBrowse->setDisabled(true);}
else{
Combo_Dirs->setEnabled(true);
ButtonBrowse->setEnabled(true);}

}

void CPasswordDialog::OnCheckBox_BothChanged(int state)
{
if(state==QCheckBox::On){
Combo_Dirs->setEnabled(true);
ButtonBrowse->setEnabled(true);
Edit_Password->setEnabled(true);}
else{
Edit_Password->setText("");
}
}

void CPasswordDialog::ChangeEchoMode()
{

if(Edit_Password->echoMode()==QLineEdit::Normal){
Edit_Password->setEchoMode(QLineEdit::Password);
}
else
{
Edit_Password->setEchoMode(QLineEdit::Normal);

}

}


void CPasswordDialog::OnButtonExit(){
	done(2);
}

