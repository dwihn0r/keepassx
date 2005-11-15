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
#include "PwManager.h"
#include "../lib/random.h"
#include "ChangeKeyDlg.h"
//QT
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmessagebox.h>



CChangeKeyDlg::CChangeKeyDlg(QWidget* parent,PwDatabase* _db,const char* name, bool modal, WFlags fl)
: ChangeKeyDialog(parent,name, modal,fl)
{
db=_db;
parentwnd=((CMainWindow*)parentWidget());
parentwnd->CreateBanner(Banner,parentwnd->Icon_Key32x32,trUtf8("Hauptschlüssel ändern"));
if(!parentwnd->config->ShowPasswords)ChangeEchoMode();
///@PlatformSpecific
QDir media("/media");
if(media.exists()){
Paths=media.entryList("*",QDir::Dirs);
Paths.erase(Paths.begin()); // delete "."
Paths.erase(Paths.begin()); // delete ".."

for(int i=0;i<Paths.count();i++){
Paths[i]="/media/"+Paths[i];
}
Paths.prepend("< none >");
}
for(int i=0;i<Paths.count();i++){
Combo_Dirs->insertItem(0,Paths[i]);
IsFile.append(false);
}
}

CChangeKeyDlg::~CChangeKeyDlg()
{
}

void CChangeKeyDlg::OnOK()
{
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

UINT8 file_key[32]={0};
UINT8 pw_key[32]={0};

if(keyfile!=""){
QFile file(keyfile);
if(file.exists()){
int r=QMessageBox::warning(this,trUtf8("Vorhandene Datei überschreiben?"),trUtf8("Unter dem gewählten Dateinamen existiert bereits eine Datei.\nSoll sie überschrieben werden?"),"Ja","Nein",NULL,1,1);
if(r==1)return;}
getRandomBytes(file_key,1,32,true);
if(file.open(IO_WriteOnly | IO_Truncate)==false){
QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Schlüsseldatei konnte nicht geöffnet werden."),"OK",0,0,2,1);
return;
}
if(file.writeBlock((char*)file_key,32)!=32){
file.close();
QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Das Schreiben der Schlüsseldatei ist fehlgeschlagen."),"OK",0,0,2,1);
return;
}
file.close();
}

if(CheckBox_Both->isChecked() || keyfile==""){
	if(password!=Edit_Password_2->text()){
		QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Passwort und Passwortwiederholung stimmen nicht überein.\nBitte prüfen Sie Ihre Eingabe."),"OK",0,0,2,1);
		return;}}

if(CheckBox_Both->isChecked())db->CalcMasterKeyByFileAndPw(keyfile, password);
 else if(password=="")db->CalcMasterKeyByFile(keyfile);
   else if(keyfile==""){db->CalcMasterKeyByPassword(password);}

done(1);
}

void CChangeKeyDlg::OnSelect()
{
if(Button_Browse->isEnabled()){
keyfile=QFileDialog::getSaveFileName(QDir::homeDirPath(),"",this,trUtf8("Schlüsseldatei öffnen"));
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


void CChangeKeyDlg::OnBrowse()
{
QString dir=QFileDialog::getExistingDirectory(QDir::homeDirPath(),NULL,trUtf8("Verzeichnis wählen"));
if(dir=="")return;
keyfile=dir+"/pwsafe.key";
Combo_Dirs->insertItem(dir);
Combo_Dirs->setCurrentItem(Combo_Dirs->count()-1);
if(!CheckBox_Both->isChecked()){
 Edit_Password->setDisabled(true);
 Edit_Password_2->setDisabled(true);
 Edit_Password->setText("");
 Edit_Password_2->setText("");
 password="";
}
Paths.append(dir);
IsFile.append(false);
}


void CChangeKeyDlg::OnCancel()
{
done(0);
}

void CChangeKeyDlg::OnPasswordChanged(const QString& str)
{
if(str!="" && !(CheckBox_Both->isChecked())){
Combo_Dirs->setDisabled(true);
Button_Browse->setDisabled(true);}
else{
Combo_Dirs->setEnabled(true);
Button_Browse->setEnabled(true);}
Edit_Password_2->setText("");
password=str;
}

void CChangeKeyDlg::OnPassword2Changed(const QString& str)
{
}

void CChangeKeyDlg::OnComboChanged(int i)
{
if(i==0){
keyfile="";
Edit_Password->setEnabled(true);
Edit_Password_2->setEnabled(true);
return;
}
if(IsFile[i]==true)keyfile=Paths[i];
else keyfile=Paths[i]+"/pwsafe.key";

if(!CheckBox_Both->isChecked()){
Edit_Password->setText("");
Edit_Password->setDisabled(true);
Edit_Password_2->setText("");
Edit_Password_2->setDisabled(true);}

}

void CChangeKeyDlg::OnCheckBoxChanged(int i)
{
if(i==QButton::NoChange)return;
if(i==QButton::On){
Combo_Dirs->setEnabled(true);
Button_Browse->setEnabled(true);
Edit_Password->setEnabled(true);
Edit_Password_2->setEnabled(true);
}
}

void CChangeKeyDlg::ChangeEchoMode()
{
if(Edit_Password->echoMode()==QLineEdit::Normal){
Edit_Password->setEchoMode(QLineEdit::Password);
Edit_Password_2->setEchoMode(QLineEdit::Password);
}
else
{
Edit_Password->setEchoMode(QLineEdit::Normal);
Edit_Password_2->setEchoMode(QLineEdit::Normal);
}

}






/*$SPECIALIZATION$*/


//#include "changekeydlg.moc"

