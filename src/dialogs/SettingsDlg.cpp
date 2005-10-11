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
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcolordialog.h>
#include <qlineedit.h>
#include "SettingsDlg.h"


CSettingsDlg::CSettingsDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
: SettingsDialog(parent,name, modal,fl)
{
pw=((CMainWindow*)parentWidget())->db;
mainwnd=((CMainWindow*)parentWidget());
mainwnd->CreateBanner(Banner,mainwnd->Icon_Settings32x32,trUtf8("Einstellungen"));

CheckBox_OpenLast->setChecked(mainwnd->config.OpenLast);
SpinBox_ClipboardTime->setValue(mainwnd->config.ClipboardTimeOut);
pixmTextColor->setPixmap(*(new QPixmap(pixmTextColor->width(),pixmTextColor->height())));
pixmTextColor->pixmap()->fill(mainwnd->config.BannerTextColor);

pixmColor1->setPixmap(*(new QPixmap(pixmColor1->width(),pixmColor1->height())));
pixmColor1->pixmap()->fill(mainwnd->config.BannerColor1);

pixmColor2->setPixmap(*(new QPixmap(pixmColor2->width(),pixmColor2->height())));
pixmColor2->pixmap()->fill(mainwnd->config.BannerColor2);

color1=mainwnd->config.BannerColor1;
color2=mainwnd->config.BannerColor2;
textcolor=mainwnd->config.BannerTextColor;

CheckBox_ShowPasswords->setChecked(mainwnd->config.ShowPasswords);
Edit_BrowserCmd->setText(mainwnd->config.OpenUrlCommand);
CheckBox_ExpandGroupTree->setChecked(mainwnd->config.ExpandGroupTree);

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::OnOK()
{
mainwnd->config.OpenLast=CheckBox_OpenLast->isChecked();
mainwnd->config.ClipboardTimeOut=SpinBox_ClipboardTime->value();
mainwnd->config.BannerColor1=color1;
mainwnd->config.BannerColor2=color2;
mainwnd->config.BannerTextColor=textcolor;
mainwnd->config.ShowPasswords=CheckBox_ShowPasswords->isChecked();
mainwnd->config.OpenUrlCommand=Edit_BrowserCmd->text();
mainwnd->config.ExpandGroupTree=CheckBox_ExpandGroupTree->isChecked();
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
mainwnd->CreateBanner(Banner,mainwnd->Icon_Settings32x32,trUtf8("Einstellungen"),color1,color2,textcolor);}
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
mainwnd->CreateBanner(Banner,mainwnd->Icon_Settings32x32,trUtf8("Einstellungen"),color1,color2,textcolor);}
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
mainwnd->CreateBanner(Banner,mainwnd->Icon_Settings32x32,trUtf8("Einstellungen"),color1,color2,textcolor);
}
}




/*$SPECIALIZATION$*/


//#include "settingsdlg.moc"

