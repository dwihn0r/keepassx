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

#include "mainwindow.h"

#include "PasswordDlg.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qvaluelist.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>



CPasswordDialog::CPasswordDialog(QWidget* parent, const char* name, bool modal, WFlags fl)
: PasswordDlg(parent,name, modal,fl)
{
parentwnd=((CMainWindow*)parentWidget());
parentwnd->CreateBanner(Banner,parentwnd->Icon_Key32x32,trUtf8("Datenbank öffnen"));
Label_select=new LinkLabel((QWidget*)groupframe,"Select",trUtf8("Datei manuell wählen..."),410,100);
connect(Label_select,SIGNAL(clicked()),this,SLOT(OnSelectClicked()));
///@PlatformSpecific
QDir media("/media");
if(media.exists()){
Paths=media.entryList("*",QDir::Dirs);
Paths.erase(Paths.begin()); // delete "."
Paths.erase(Paths.begin()); // delete ".."

for(int i=0;i<Paths.count();i++){
Paths[i]="/media/"+Paths[i];
}
Paths.prepend(trUtf8("< keiner >"));
}
for(int i=0;i<Paths.count();i++){
Combo_Dirs->insertItem(0,Paths[i]);
IsFile.append(false);
}

if(!parentwnd->config->ShowPasswords)ChangeEchoMode();

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
QMessageBox::warning(this,"Datei nicht gefunden",QString::fromUtf8("Im gewählten Verzeichnis konnte keine Schlüsseldatei gefunden werden.\nStellen Sie sicher, dass der Schlüssel-Datenträger ordnungsgemäß eingehängt (mounted) ist.\nSollte die Schlüsseldatei nicht den Dateinamen 'pwsafe.key' haben, nutzen Sie bitte die manuelle Dateiauswahl."),"OK","","",0,0);
Edit_Password->setEnabled(true);
Combo_Dirs->setCurrentItem(0);

return;
}


void CPasswordDialog::OnButtonBrowse()
{
///@PlatformSpecific
QString dir=QFileDialog::getExistingDirectory(QDir::homeDirPath(),NULL,QString::fromUtf8("Verzeichnis wählen"));
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
QMessageBox::warning(this,"Datei nicht gefunden",QString::fromUtf8("Im gewählten Verzeichnis konnte keine Schlüsseldatei gefunden werden.\nStellen Sie sicher, dass der Schlüssel-Datenträger ordnungsgemäß eingehängt (mounted) ist.\nSollte die Schlüsseldatei nicht den Dateinamen 'pwsafe.key' haben, nutzen Sie bitte die manuelle Dateiauswahl."),"OK","","",0,0);
}

void CPasswordDialog::OnSelectClicked()
{
if(Button_Browse->isEnabled()){
keyfile=QFileDialog::getOpenFileName(QDir::homeDirPath(),"",this,QString::fromUtf8("Schlüsseldatei öffnen"));
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
canceled=true;
close();
}

void CPasswordDialog::OnOK()
{
canceled=false;

if(CheckBox_Both->isChecked()){
 if(password==""){QMessageBox::warning(this,trUtf8("Fehler"),trUtf8("Bitte geben Sie ein Passwort ein.")
                                      ,trUtf8("OK"),"","",0,0);
			return;}
 if(keyfile==""){QMessageBox::warning(this,trUtf8("Fehler"),trUtf8("Bitte wählen Sie eine Schlüsseldatei.")
                                      ,trUtf8("OK"),"","",0,0);
			return;}
}
else
{
 if(password=="" && keyfile==""){QMessageBox::warning(this,trUtf8("Fehler"),trUtf8("Geben Sie bitte ein Passwort ein oder wählen Sie eine Schlüsseldatei.")
                                      ,trUtf8("OK"),"","",0,0);
			return;}
}
close();
}

void CPasswordDialog::OnPasswordChanged(const QString &txt)
{
password=Edit_Password->text();
if(txt!="" && !(CheckBox_Both->isChecked())){
Combo_Dirs->setDisabled(true);
Button_Browse->setDisabled(true);}
else{
Combo_Dirs->setEnabled(true);
Button_Browse->setEnabled(true);}

}

void CPasswordDialog::OnCheckBox_BothChanged(int state)
{
if(state==QButton::On){
Combo_Dirs->setEnabled(true);
Button_Browse->setEnabled(true);
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





/*$SPECIALIZATION$*/


//#include "passworddialog.moc"

