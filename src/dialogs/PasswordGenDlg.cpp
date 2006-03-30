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

#include <fstream>
#include <qspinbox.h>
#include <qmessagebox.h>
#include "PasswordGenDlg.h"
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <QProgressBar>

CGenPwDialog::CGenPwDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
createBanner(Banner,Icon_Key32x32,QString::fromUtf8("Password Generator"));
Radio_1->setChecked(true);
Edit_chars->setDisabled(true);
connect(ButtonGenerate,SIGNAL(clicked()),this,SLOT(OnGeneratePw()));
connect(Radio_1,SIGNAL(toggled(bool)),this,SLOT(OnRadio1StateChanged(bool)));
connect(Radio_2,SIGNAL(toggled(bool)),this,SLOT(OnRadio2StateChanged(bool)));
connect(Button_Cancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
connect(ButtonOK,SIGNAL(clicked()),this,SLOT(OnAccept()));
}

CGenPwDialog::~CGenPwDialog()
{
}

void CGenPwDialog::OnRadio1StateChanged(bool state)
{
if(state){
 Radio_2->setChecked(false);
 checkBox1->setEnabled(true);
 checkBox2->setEnabled(true);
 checkBox3->setEnabled(true);
 checkBox4->setEnabled(true);
 checkBox5->setEnabled(true);
 checkBox6->setEnabled(true);
 checkBox7->setEnabled(true);
 checkBox8->setEnabled(true);
}else{
 if(Radio_2->isChecked()==false)Radio_2->setChecked(true);
 checkBox1->setDisabled(true);
 checkBox2->setDisabled(true);
 checkBox3->setDisabled(true);
 checkBox4->setDisabled(true);
 checkBox5->setDisabled(true);
 checkBox6->setDisabled(true);
 checkBox7->setDisabled(true);
 checkBox8->setDisabled(true);
}

}

void CGenPwDialog::OnRadio2StateChanged(bool state)
{
if(state){
 Radio_1->setChecked(false);
 Edit_chars->setEnabled(true);
}
else{
  if(Radio_1->isChecked()==false)Radio_1->setChecked(true);
 Edit_chars->setDisabled(true);
}

}

void CGenPwDialog::OnGeneratePw()
{
/*
-------
 ASCII
-------
"A...Z" 65...90
"a...z" 97...122
"0...9" 48...57
Special Charakters 33...47;58...64;91...96;123...126
"-" 45
"_" 95
ANSI >127
*/

int num=0;
char assoctable[255];

if(Radio_1->isChecked()){
if(checkBox1->isChecked()){
num+=AddToAssoctable(assoctable,65,90,num);
}

if(checkBox2->isChecked()){
num+=AddToAssoctable(assoctable,97,122,num);
}

if(checkBox3->isChecked()){
num+=AddToAssoctable(assoctable,48,57,num);
}

if(checkBox4->isChecked()){
num+=AddToAssoctable(assoctable,33,47,num);
num+=AddToAssoctable(assoctable,58,64,num);
num+=AddToAssoctable(assoctable,91,96,num);
num+=AddToAssoctable(assoctable,123,126,num);
}

if(checkBox5->isChecked()){
num+=AddToAssoctable(assoctable,32,32,num);
}


if(checkBox6->isChecked() && !checkBox4->isChecked()){
num+=AddToAssoctable(assoctable,45,45,num);
}

if(checkBox7->isChecked() && !checkBox4->isChecked()){
num+=AddToAssoctable(assoctable,95,95,num);
}

if(checkBox8->isChecked()){
num+=AddToAssoctable(assoctable,128,255,num);
}
}else
{
QString str=Edit_chars->text();
int i=0;
while(str.length()>0){
assoctable[i]=((QChar)str[0]).toAscii();
str.remove(str[0]);
i++;
num++;
}
}
if(num==0){
if(Radio_2->isChecked())QMessageBox::information(this,tr("Notice"),tr("You need to enter at least one character"),tr("OK"));
else QMessageBox::information(this,tr("Notice"),QString::fromUtf8("You need to select at least one character group."),"OK");
return;
}
int length=Spin_Num->value();
char* buffer=new char[length+1];
buffer[length]=0;
FILE *dev_random;
if(Check_strongrandom->isChecked()){
dev_random = fopen("/dev/random","r");}
else
{dev_random = fopen("/dev/urandom","r");}

if (dev_random==NULL){
QMessageBox::critical(this,tr("Error"),tr("Could not open '/dev/random' or '/dev/urandom'."),tr("OK"));
return;
}
unsigned char tmp;

for(int i=0;i<length;i++){

do{
fread(&tmp,1,1,dev_random);
}while(trim(tmp,num)==false);

buffer[i]=assoctable[tmp];
}

Edit_dest->setText(buffer);
delete [] buffer;
fclose(dev_random);

int bits;
if(checkBox8->isChecked())bits=length*8;
else bits=length*7;
Label_Bits->setText(QString::number(bits)+" Bit");
if(bits>128)bits=128;
Progress_Quali->setProgress(bits,128);
Progress_Quali->setPercentageVisible(false);
}

int CGenPwDialog::AddToAssoctable(char* table,int start,int end,int pos){
for(int i=start;i<=end;i++){
table[pos]=i;
pos++;
}
return (end-start)+1;
}


bool CGenPwDialog::trim(unsigned char &x, int r){
if(x<r)return true;
if(256%r!=0)return false;
x=x-(x/r)*r;
return true;
}

void CGenPwDialog::OnAccept()
{
((CEditEntryDlg*)parentWidget())->Edit_Password->setText(Edit_dest->text());
((CEditEntryDlg*)parentWidget())->Edit_Password_w->setText(Edit_dest->text());
((CEditEntryDlg*)parentWidget())->ModFlag=true;
close();
}

void CGenPwDialog::OnCancel()
{
close();
}




/*$SPECIALIZATION$*/


//#include "genpwdialog.moc"

