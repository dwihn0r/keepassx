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

#include <qpushbutton.h>
#include <qpalette.h>
#include <qfont.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qpen.h>
#include <qfiledialog.h>
#include <qmessagebox.h>


#include "PasswordGenDlg.h"
#include "EditEntryDlg.h"



CEditEntryDlg::CEditEntryDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
: EditEntryDialog(parent,name, modal,fl)
{

pw=((CMainWindow*)parentWidget())->db;
mainwnd=((CMainWindow*)parentWidget());
mainwnd->CreateBanner(Banner,mainwnd->Icon_Key32x32,trUtf8("Eintrag bearbeiten"));
ModFlag=false;
}

CEditEntryDlg::~CEditEntryDlg()
{


}


void CEditEntryDlg::showEvent(QShowEvent *event){

if(event->spontaneous()==false){
if(entry->pBinaryData==NULL){
ButtonSaveAttachment->setDisabled(true);
ButtonDeleteAttachment->setDisabled(true);
}
setCaption(entry->Title);
setIcon(mainwnd->EntryIcons[entry->ImageID]);
Edit_Title->setText(entry->Title);
Edit_UserName->setText(entry->UserName);
Edit_URL->setText(entry->URL);
Edit_Password->setText(entry->Password.getString());
Edit_Password_w->setText(entry->Password.getString());
entry->Password.delRef();
if(!mainwnd->config.ShowPasswords)ChangeEchoMode();
OnPasswordwLostFocus();
int bits=(entry->Password.length()*8);
Label_Bits->setText(QString::number(bits)+" Bit");
if(bits>128)bits=128;
Progress_Quali->setProgress(bits,128);
Progress_Quali->setPercentageVisible(false);
Edit_Attachment->setText(entry->BinaryDesc);
Edit_Comment->setText(entry->Additional);
InitGroupComboBox();
InitIconComboBox();
Edit_Expire_Date->setText((entry->Expire.GetString(0)).mid(0,10));
Edit_Expire_Time->setText((entry->Expire.GetString(0)).mid(11,8));
if(entry->BinaryDataLength==0){
Label_AttachmentSize->setText("");
}
else
{
QString unit;
int faktor;
int prec;
  if(entry->BinaryDataLength<1000){unit=" Byte";faktor=1;prec=0;}
  else {if(entry->BinaryDataLength<1000000){unit=" kB";faktor=1000;prec=1;}
  	else{unit=" MB";faktor=1000000;prec=1;}
	}
Label_AttachmentSize->setText(QString::number((float)entry->BinaryDataLength/(float)faktor,'f',prec)+unit);
}

}
}

void CEditEntryDlg::InitIconComboBox(){
for(int i=0;i<52;i++){
Combo_IconPicker->insertItem(((CMainWindow*)parentWidget())->EntryIcons[i],"",i);
}
Combo_IconPicker->setCurrentItem(entry->ImageID);
}


void CEditEntryDlg::InitGroupComboBox(){
QString tmp;
int i;
for(i=0;i!=pw->Groups.size();i++){
tmp="";
  for(int j=0;j<pw->Groups[i].Level;j++)tmp+="  ";
Combo_Group->insertItem(((CMainWindow*)parentWidget())->EntryIcons[pw->Groups[i].ImageID],
 			tmp+pw->Groups[i].Name,i);
}
Combo_Group->setCurrentItem(pw->getGroupIndex(entry->GroupID));
}

void CEditEntryDlg::OnButtonOK()
{

if(QString::compare(Edit_Password->text(),Edit_Password_w->text())!=0){
QMessageBox::warning(NULL,"Stopp",QString::fromUtf8("Passwort und Passwortwiederholung stimmen\nnicht überein."),"OK");
return;
}
QString str=Edit_Expire_Date->text();
if(CPwmTime::IsValidDate(str)==false){
QMessageBox::warning(NULL,"Stopp",QString::fromUtf8(str+" ist kein gültiges Datum."),"OK");
return;
}

str=Edit_Expire_Time->text();
if(CPwmTime::IsValidTime(str)==false){
QMessageBox::warning(NULL,"Stopp",QString::fromUtf8(str+" ist keine gültige Uhrzeit."),"OK");
return;
}

CPwmTime tmp_Expire;
tmp_Expire.SetDate(Edit_Expire_Date->text());
tmp_Expire.SetTime(Edit_Expire_Time->text());
if(tmp_Expire!=entry->Expire)
	ModFlag=true;
if(entry->Title!=Edit_Title->text())
	ModFlag=true;
if(entry->UserName!=Edit_UserName->text())
	ModFlag=true;
if(entry->URL!=Edit_URL->text())
	ModFlag=true;
if(entry->Additional!=Edit_Comment->text())
	ModFlag=true;
QString& passw=entry->Password.getString();
if(passw!=Edit_Password->text())
	ModFlag=true;
entry->Password.delRef();

entry->Expire.SetDate(Edit_Expire_Date->text());
entry->Expire.SetTime(Edit_Expire_Time->text());
entry->LastAccess.SetToNow();
if(ModFlag)entry->LastMod.SetToNow();
entry->Title=Edit_Title->text();
entry->UserName=Edit_UserName->text();
entry->URL=Edit_URL->text();
QString s=Edit_Password->text();
entry->Password.setString(s,true);
entry->Additional=Edit_Comment->text();
if(Combo_Group->currentItem()!=pw->getGroupIndex(entry->GroupID)){
pw->moveEntry(entry,&pw->Groups[Combo_Group->currentItem()]);
}
entry->ImageID=Combo_IconPicker->currentItem();
done(1);
}

void CEditEntryDlg::OnButtonCancel()
{
entry->LastAccess.SetToNow();
done(0);
}

void CEditEntryDlg::ChangeEchoMode()
{
if(Edit_Password->echoMode()==QLineEdit::Normal){
Edit_Password->setEchoMode(QLineEdit::Password);
Edit_Password_w->setEchoMode(QLineEdit::Password);
}
else
{
Edit_Password->setEchoMode(QLineEdit::Normal);
Edit_Password_w->setEchoMode(QLineEdit::Normal);
}


}

void CEditEntryDlg::OnPasswordTextChanged(const QString& txt)
{
Edit_Password_w->setText("");
int bits=(Edit_Password->text().length()*8);
Label_Bits->setText(QString::number(bits)+" Bit");
if(bits>128)bits=128;
Progress_Quali->setProgress(bits,128);
}

void CEditEntryDlg::OnPasswordwTextChanged(const QString& w)
{

if(QString::compare(Edit_Password_w->text(),Edit_Password->text().mid(0,(Edit_Password_w->text().length())))!=0){
Edit_Password_w->setPaletteBackgroundColor(QColor(255,125,125));
}else
{
Edit_Password_w->setPaletteBackgroundColor(QColor(255,255,255)); ///@FIXME Standart-Hintergrundfarbe nicht weiß
}



}

void CEditEntryDlg::OnPasswordwLostFocus()
{
if(QString::compare(Edit_Password_w->text(),Edit_Password->text())!=0){
Edit_Password_w->setPaletteBackgroundColor(QColor(255,125,125));
}
else
{
Edit_Password_w->setPaletteBackgroundColor(QColor(255,255,255)); ///@FIXME Standart-Hintergrundfarbe nicht weiß
}


}

void CEditEntryDlg::OnExpDateLostFocus()
{
QString str=Edit_Expire_Date->text();
if(CPwmTime::IsValidDate(str)==false){
Edit_Expire_Date->setPaletteBackgroundColor(QColor(255,125,125));
}
else
{
Edit_Expire_Date->setPaletteBackgroundColor(QColor(255,255,255));///@FIXME Standart-Hintergrundfarbe nicht weiß
}
}

void CEditEntryDlg::OnExpTimeLostFocus()
{
QString str=Edit_Expire_Time->text();
if(CPwmTime::IsValidTime(str)==false){
Edit_Expire_Time->setPaletteBackgroundColor(QColor(255,125,125));
}
else
{
Edit_Expire_Time->setPaletteBackgroundColor(QColor(255,255,255));///@FIXME Standart-Hintergrundfarbe nicht weiß
}
}

void CEditEntryDlg::OnNewAttachment()
{
QString filename=QFileDialog::getOpenFileName(QDir::homeDirPath(),"",this,QString::fromUtf8("Anhang hinzufügen..."));
if(filename=="")return;
QFile file(filename);
if(file.open(IO_ReadOnly)==false){
file.close();
QMessageBox::warning(NULL,trUtf8("Fehler"),trUtf8("Datei konnte nicht geöffnet werden."),"OK");
return;
}
ModFlag=true;
if(entry->pBinaryData)delete [] entry->pBinaryData;
entry->pBinaryData = new UINT8 [file.size()];

if(entry->pBinaryData==NULL){
file.close();
QMessageBox::critical(NULL,"Fehler",QString::fromUtf8("Es ist nicht genügend Arbeitsspeicher für diesen Vorgang vorhanden."),"OK");
return;
}
entry->BinaryDataLength=file.size();
file.readBlock((char*)entry->pBinaryData,file.size());
file.close();
QFileInfo info(filename);
entry->BinaryDesc=info.fileName();
file.close();
Edit_Attachment->setText(entry->BinaryDesc);
QString unit;
int faktor;
int prec;
  if(entry->BinaryDataLength<1000){unit=" Byte";faktor=1;prec=0;}
  else {if(entry->BinaryDataLength<1000000){unit=" kB";faktor=1000;prec=1;}
  	else{unit=" MB";faktor=1000000;prec=1;}
	}
Label_AttachmentSize->setText(QString::number((float)entry->BinaryDataLength/(float)faktor,'f',prec)+unit);
ButtonOpenAttachment->setEnabled(true);
ButtonSaveAttachment->setEnabled(true);
ButtonDeleteAttachment->setEnabled(true);
}

void CEditEntryDlg::OnSaveAttachment()
{
QString filename=QFileDialog::getSaveFileName(QDir::homeDirPath(),"",this,trUtf8("Anhang speichern..."));
if(filename=="")return;
QFile file(filename);
if(file.exists()){
int r=QMessageBox::warning(this,QString::fromUtf8("Vorhandene Datei überschreiben?"),QString::fromUtf8("Unter dem gewählten Dateinamen existiert bereits eine Datei.\nSoll sie überschrieben werden?"),"Ja","Nein",NULL,1,1);
if(r==1)return;
if(file.remove()==false){
QMessageBox::critical(NULL,"Fehler",QString::fromUtf8("Datei konnte nicht überschrieben werden."),"OK");
return;}
}
if(file.open(IO_WriteOnly)==false){
QMessageBox::critical(NULL,"Fehler",QString::fromUtf8("Datei konnte nicht erstellt werden."),"OK");
return;
}

int r=file.writeBlock((char*)entry->pBinaryData,entry->BinaryDataLength);
if(r==-1){
file.close();
QMessageBox::critical(NULL,"Fehler",QString::fromUtf8("Beim schreiben in der Datei ist ein Fehler aufgetreten."),"OK");
return;
}
if(r!=entry->BinaryDataLength){
file.close();
QMessageBox::critical(NULL,"Fehler",QString::fromUtf8("Die Datei konnte nicht vollständig geschrieben werden."),"OK");
return;
}
file.close();
}

void CEditEntryDlg::OnDeleteAttachment()
{
int r=QMessageBox::warning(this,trUtf8("Anhang löschen?"),trUtf8("Sie sind dabei den Anhang zu löschen.\nSind Sie sicher, dass Sie dies tun wollen?"),trUtf8("Ja"),trUtf8("Nein"),NULL,1,1);
if(r==0){
ModFlag=true;
delete[]entry->pBinaryData;
entry->pBinaryData=NULL;
entry->BinaryDataLength=0;
entry->BinaryDesc="";
Edit_Attachment->setText("");
Label_AttachmentSize->setText("");
ButtonOpenAttachment->setEnabled(true);
ButtonSaveAttachment->setDisabled(true);
ButtonDeleteAttachment->setDisabled(true);
}
}

void CEditEntryDlg::OnButtonGenPw()
{
CGenPwDialog* pDlg=new CGenPwDialog(this,0,true);
pDlg->show();
}














/*$SPECIALIZATION$*/


//#include "editentrydlg.moc"

