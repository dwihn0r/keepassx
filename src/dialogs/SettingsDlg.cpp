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
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcolordialog.h>
#include <qlineedit.h>
#include "SettingsDlg.h"


CSettingsDlg::CSettingsDlg(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK() ) );
connect( ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
connect( ButtonColor1, SIGNAL( clicked() ), this, SLOT( OnColor1() ) );
connect( ButtonColor2, SIGNAL( clicked() ), this, SLOT( OnColor2() ) );
connect( ButtonTextColor, SIGNAL( clicked() ), this, SLOT( OnTextColor() ) );
createBanner(Banner,Icon_Settings32x32,tr("Settings"));
CheckBox_OpenLast->setChecked(config.OpenLast);
SpinBox_ClipboardTime->setValue(config.ClipboardTimeOut);

QPixmap *pxt=new QPixmap(pixmTextColor->width(),pixmTextColor->height());
pxt->fill(config.BannerTextColor);
pixmTextColor->clear();
pixmTextColor->setPixmap(*pxt);

QPixmap *px1=new QPixmap(pixmColor1->width(),pixmColor1->height());
px1->fill(config.BannerColor1);
pixmColor1->clear();
pixmColor1->setPixmap(*px1);

QPixmap *px2=new QPixmap(pixmColor2->width(),pixmColor2->height());
px2->fill(config.BannerColor2);
pixmColor2->clear();
pixmColor2->setPixmap(*px2);

color1=config.BannerColor1;
color2=config.BannerColor2;
textcolor=config.BannerTextColor;

CheckBox_ShowPasswords->setChecked(config.ShowPasswords);
Edit_BrowserCmd->setText(config.OpenUrlCommand);
CheckBox_ExpandGroupTree->setChecked(config.ExpandGroupTree);

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::OnOK()
{
config.OpenLast=CheckBox_OpenLast->isChecked();
config.ClipboardTimeOut=SpinBox_ClipboardTime->value();
config.BannerColor1=color1;
config.BannerColor2=color2;
config.BannerTextColor=textcolor;
config.ShowPasswords=CheckBox_ShowPasswords->isChecked();
config.OpenUrlCommand=Edit_BrowserCmd->text();
config.ExpandGroupTree=CheckBox_ExpandGroupTree->isChecked();
close();
}

void CSettingsDlg::OnCancel()
{
close();
}

void CSettingsDlg::OnTextColor()
{
QColor c=QColorDialog::getColor(textcolor,this);
if(c.isValid()){
textcolor=c;
QPixmap *px=new QPixmap(pixmTextColor->width(),pixmTextColor->height());
px->fill(c);
pixmTextColor->clear();
pixmTextColor->setPixmap(*px);
createBanner(Banner,Icon_Settings32x32,tr("Settings"),color1,color2,textcolor);}
}


void CSettingsDlg::OnColor2()
{
QColor c=QColorDialog::getColor(color2,this);
if(c.isValid()){
color2=c;
QPixmap *px=new QPixmap(pixmColor2->width(),pixmColor2->height());
px->fill(c);
pixmColor2->clear();
pixmColor2->setPixmap(*px);
createBanner(Banner,Icon_Settings32x32,tr("Settings"),color1,color2,textcolor);}
}


void CSettingsDlg::OnColor1()
{
QColor c=QColorDialog::getColor(color1,this);
if(c.isValid()){
color1=c;
QPixmap *px=new QPixmap(pixmColor1->width(),pixmColor1->height());
px->fill(c);
pixmColor1->clear();
pixmColor1->setPixmap(*px);
createBanner(Banner,Icon_Settings32x32,tr("Settings"),color1,color2,textcolor);
}
}




/*$SPECIALIZATION$*/


//#include "settingsdlg.moc"

