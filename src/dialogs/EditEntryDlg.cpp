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

#include "main.h"
#include "PwmConfig.h"
#include <qpushbutton.h>
#include <qpalette.h>
#include <qfont.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <q3progressbar.h>
#include <q3textedit.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qpen.h>
#include <QFileDialog>
#include <qmessagebox.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <QShowEvent>

#include "SelectIconDlg.h"
#include "PasswordGenDlg.h"
#include "EditEntryDlg.h"



CEditEntryDlg::CEditEntryDlg(Database* _db, CEntry* _entry,QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
Q_ASSERT(_db);
Q_ASSERT(_entry);
entry=_entry;
db=_db;
setupUi(this);
createBanner(Banner,Icon_Key32x32,tr("Edit Entry"));
ModFlag=false;
connect(Edit_Password_w, SIGNAL(editingFinished()), this, SLOT(OnPasswordwLostFocus()));
connect(Edit_Password_w, SIGNAL(textChanged(const QString&)), this, SLOT( OnPasswordwTextChanged(const QString&)));
connect(Edit_Password, SIGNAL(textChanged(const QString&)), this, SLOT( OnPasswordTextChanged(const QString&)));
connect(ButtonEchoMode, SIGNAL(clicked()), this, SLOT( ChangeEchoMode()));
connect(ButtonCancel, SIGNAL(clicked()), this, SLOT( OnButtonCancel()));
connect(ButtonOpenAttachment, SIGNAL(clicked()), this, SLOT( OnNewAttachment()));
connect(ButtonDeleteAttachment, SIGNAL(clicked()), this, SLOT( OnDeleteAttachment()));
connect(ButtonSaveAttachment, SIGNAL(clicked()), this, SLOT( OnSaveAttachment()));
connect(ButtonGenPw, SIGNAL(clicked()), this, SLOT( OnButtonGenPw()));
connect(ButtonOK, SIGNAL(clicked()),this,SLOT(OnButtonOK()));
connect(CheckBox_ExpiresNever,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxExpiresNeverChanged(int)));
connect(Button_CustomIcons,SIGNAL(clicked()),this,SLOT(OnCustomIcons()));

ButtonOpenAttachment->setIcon(*Icon_FileOpen);
ButtonDeleteAttachment->setIcon(*Icon_EditDelete);
ButtonSaveAttachment->setIcon(*Icon_FileSave);

if(entry->BinaryData.isNull()){
  ButtonSaveAttachment->setDisabled(true);
  ButtonDeleteAttachment->setDisabled(true);}
setCaption(entry->Title);
setIcon(db->icon(entry->ImageID));
Edit_Title->setText(entry->Title);
Edit_UserName->setText(entry->UserName);
Edit_URL->setText(entry->URL);
entry->Password.unlock();
Edit_Password->setText(entry->Password.string());
Edit_Password_w->setText(entry->Password.string());
entry->Password.lock();
if(!config.ShowPasswords)
  ChangeEchoMode();
OnPasswordwLostFocus();
int bits=(entry->Password.length()*8);
Label_Bits->setText(tr("%1 Bit").arg(QString::number(bits)));
if(bits>128)
  bits=128;
Progress_Quali->setValue(100*bits/128);
Edit_Attachment->setText(entry->BinaryDesc);
Edit_Comment->setText(entry->Additional);
InitGroupComboBox();
InitIconComboBox();
if(entry->BinaryData.length()==0)
  Label_AttachmentSize->setText("");
else{
  QString unit;
  int faktor;
  int prec;
  if(entry->BinaryData.length()<1000){unit=" Byte";faktor=1;prec=0;}
  else {if(entry->BinaryData.length()<1000000){unit=" kB";faktor=1000;prec=1;}
  	else{unit=" MB";faktor=1000000;prec=1;}
  }
  Label_AttachmentSize->setText(QString::number((float)entry->BinaryData.length()/(float)faktor,'f',prec)+unit);
}
if(entry->Expire==Date_Never){
  DateTime_Expire->setDisabled(true);
  CheckBox_ExpiresNever->setChecked(true);
}
else{
DateTime_Expire->setDateTime(entry->Expire);
}



}

CEditEntryDlg::~CEditEntryDlg()
{


}


void CEditEntryDlg::showEvent(QShowEvent *event){

if(event->spontaneous()==false){


}
}

void CEditEntryDlg::InitIconComboBox(){
for(int i=0;i<db->numIcons();i++){
	Combo_IconPicker->insertItem(db->icon(i),"",i);
}
	Combo_IconPicker->setCurrentItem(entry->ImageID);
}


void CEditEntryDlg::InitGroupComboBox(){
QString tmp;
int i;
for(i=0;i!=db->numGroups();i++){
tmp="";
  for(int j=0;j<db->group(i).Level;j++)tmp+="  ";
Combo_Group->insertItem(db->icon(db->group(i).ImageID),
 			tmp+db->group(i).Name,i);
}
Combo_Group->setCurrentItem(db->getGroupIndex(entry->GroupID));
}

void CEditEntryDlg::OnButtonOK()
{
bool EntryMoved=false;
if(QString::compare(Edit_Password->text(),Edit_Password_w->text())!=0){
QMessageBox::warning(NULL,tr("Warning"),tr("Password and password repetition are not equal.\nPlease check your input."),tr("OK"));
return;
}

if(CheckBox_ExpiresNever->state()==Qt::Checked){
  DateTime_Expire->setDateTime(Date_Never);}

if(DateTime_Expire->dateTime()!=entry->Expire)
	ModFlag=true;
if(entry->Title!=Edit_Title->text())
	ModFlag=true;
if(entry->UserName!=Edit_UserName->text())
	ModFlag=true;
if(entry->URL!=Edit_URL->text())
	ModFlag=true;
if(entry->Additional!=Edit_Comment->text())
	ModFlag=true;
entry->Password.unlock();
if(entry->Password.string()!=Edit_Password->text())
	ModFlag=true;
entry->Password.lock();
if(entry->ImageID!=Combo_IconPicker->currentItem())
	ModFlag=true;

entry->Expire=DateTime_Expire->dateTime();
entry->LastAccess=QDateTime::currentDateTime();
if(ModFlag)entry->LastMod=QDateTime::currentDateTime();
entry->Title=Edit_Title->text();
entry->UserName=Edit_UserName->text();
entry->URL=Edit_URL->text();
QString s=Edit_Password->text();
entry->Password.setString(s,true);
entry->Additional=Edit_Comment->text();
if(Combo_Group->currentItem()!=db->getGroupIndex(entry->GroupID)){
	db->moveEntry(entry,&db->group(Combo_Group->currentItem()));
	EntryMoved=true; ModFlag=true;
}
if(entry->ImageID<BUILTIN_ICONS && Combo_IconPicker->currentItem()>=BUILTIN_ICONS)
	entry->OldImgID=entry->ImageID;
entry->ImageID=Combo_IconPicker->currentItem();

if(ModFlag&&EntryMoved)done(2);
else if(ModFlag)done(1);
else done(0);
}

void CEditEntryDlg::OnButtonCancel()
{
entry->LastAccess=QDateTime::currentDateTime();
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
Progress_Quali->setValue(100*bits/128);
}

void CEditEntryDlg::OnPasswordwTextChanged(const QString& w)
{

if(QString::compare(Edit_Password_w->text(),Edit_Password->text().mid(0,(Edit_Password_w->text().length())))!=0){
Edit_Password_w->setPaletteBackgroundColor(QColor(255,125,125));
}else
{
Edit_Password_w->setPaletteBackgroundColor(QColor(255,255,255)); ///@FIXME Standard-Hintergrundfarbe nicht weiß
}



}

void CEditEntryDlg::OnPasswordwLostFocus()
{
if(QString::compare(Edit_Password_w->text(),Edit_Password->text())!=0){
Edit_Password_w->setPaletteBackgroundColor(QColor(255,125,125));
}
else
{
Edit_Password_w->setPaletteBackgroundColor(QColor(255,255,255)); ///@FIXME Standard-Hintergrundfarbe nicht weiß
}


}

void CEditEntryDlg::OnNewAttachment()
{
QString filename=QFileDialog::getOpenFileName(this,tr("Add Attachment..."),QDir::homeDirPath());
if(filename=="")return;
QFile file(filename);
if(file.open(QIODevice::ReadOnly)==false){
file.close();
QMessageBox::warning(NULL,tr("Error"),tr("Could not open file."),tr("OK"));
return;
}
ModFlag=true;
entry->BinaryData=file.readAll();
file.close();
QFileInfo info(filename);
entry->BinaryDesc=info.fileName();
file.close();
Edit_Attachment->setText(entry->BinaryDesc);
QString unit;
int faktor;
int prec;
  if(entry->BinaryData.length()<1000){unit=" Byte";faktor=1;prec=0;}
  else {if(entry->BinaryData.length()<1000000){unit=" kB";faktor=1000;prec=1;}
  	else{unit=" MB";faktor=1000000;prec=1;}
	}
Label_AttachmentSize->setText(QString::number((float)entry->BinaryData.length()/(float)faktor,'f',prec)+unit);
ButtonOpenAttachment->setEnabled(true);
ButtonSaveAttachment->setEnabled(true);
ButtonDeleteAttachment->setEnabled(true);
}

void CEditEntryDlg::OnSaveAttachment(){
saveAttachment(entry,this);
}

void CEditEntryDlg::saveAttachment(CEntry* pEntry, QWidget* ParentWidget)
{
QFileDialog FileDlg(ParentWidget,tr("Save Attachment..."),QDir::homeDirPath());
FileDlg.selectFile(pEntry->BinaryDesc);
FileDlg.setAcceptMode(QFileDialog::AcceptSave);
if(!FileDlg.exec())return;
QString filename=FileDlg.selectedFiles()[0];
QFile file(filename);
if(file.exists()){
int r=QMessageBox::warning(ParentWidget,tr("Overwrite?"),tr("A file with this name already exists.\nDo you want to replace it?"),tr("Yes"),tr("No"),NULL,1,1);
if(r==1)return;
if(file.remove()==false){
QMessageBox::critical(NULL,tr("Error"),tr("Could not remove old file."),tr("OK"));
return;}
}
if(file.open(QIODevice::WriteOnly)==false){
QMessageBox::critical(NULL,tr("Error"),tr("Could not create new file."),tr("OK"));
return;
}

int r=file.write(pEntry->BinaryData);
if(r==-1){
file.close();
QMessageBox::critical(NULL,tr("Error"),tr("Error while writing the file."),"OK");
return;
}
if(r!=pEntry->BinaryData.length()){
file.close();
QMessageBox::critical(NULL,tr("Error"),tr("Error while writing the file."),tr("OK"));
return;
}
file.close();
}

void CEditEntryDlg::OnDeleteAttachment()
{
int r=QMessageBox::warning(this,tr("Delete Attachment?"),tr("You are about to delete the attachment of this entry.\nAre you sure?"),tr("Yes"),tr("No, Cancel"),NULL,1,1);
if(r==0){
ModFlag=true;
entry->BinaryData.clear();
entry->BinaryData=QByteArray();
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


void CEditEntryDlg::OnCheckBoxExpiresNeverChanged(int state){
if(state==Qt::Unchecked){
 DateTime_Expire->setDisabled(false);
}
else
{
 DateTime_Expire->setDisabled(true);
}
}

void CEditEntryDlg::OnCustomIcons(){
CSelectIconDlg dlg(db,Combo_IconPicker->currentItem(),this);
int r=dlg.exec();
if(dlg.ModFlag)ModFlag=true;
if(r!=-1){
	Combo_IconPicker->clear();
	for(int i=0;i<db->numIcons();i++)
		Combo_IconPicker->insertItem(db->icon(i),"",i);
	Combo_IconPicker->setCurrentItem(r);
}
}



/*$SPECIALIZATION$*/


//#include "editentrydlg.moc"

