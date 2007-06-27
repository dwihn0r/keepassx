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
#include "KpxConfig.h"
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcolordialog.h>
#include <qlineedit.h>
#include <QFileDialog>
#include <QDir>
#include <QPainter>
#include "SettingsDlg.h"
#include "CustomizeDetailViewDlg.h"

bool CSettingsDlg::PluginsModified=false;


CSettingsDlg::CSettingsDlg(QWidget* parent):QDialog(parent,Qt::Dialog)
{
	setupUi(this);
	connect(DialogButtons, SIGNAL( accepted() ), this, SLOT( OnOK() ) );
	connect(DialogButtons, SIGNAL( rejected() ), this, SLOT( OnCancel() ) );
	connect(DialogButtons, SIGNAL( clicked(QAbstractButton*)), this, SLOT(OnOtherButton(QAbstractButton*)));

	connect(ButtonColor1, SIGNAL( clicked() ), this, SLOT( OnColor1() ) );
	connect(ButtonColor2, SIGNAL( clicked() ), this, SLOT( OnColor2() ) );
	connect(ButtonTextColor, SIGNAL( clicked() ), this, SLOT( OnTextColor() ) );
	connect(CheckBox_OpenLast,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxOpenLastChanged(int)));
	connect(Button_MountDirBrowse,SIGNAL(clicked()),this,SLOT(OnMountDirBrowse()));

	connect(Radio_IntPlugin_None,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginNone(bool)));
	connect(Radio_IntPlugin_Gnome,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginGnome(bool)));
	connect(Radio_IntPlugin_Kde,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginKde(bool)));

	connect(CheckBox_BrowserDefault,SIGNAL(stateChanged(int)),this,SLOT(OnCheckBoxBrowserDefaultChanged(int)));
	connect(Button_CustomizeEntryDetails,SIGNAL(clicked()),this,SLOT(OnCustomizeEntryDetails()));


	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width());

	//General
	CheckBox_OpenLast->setChecked(config->openLastFile());
	CheckBox_RememberLastKey->setChecked(config->rememberLastKey());
	checkBox_ShowSysTrayIcon->setChecked(config->showSysTrayIcon());
	checkBox_MinimizeToTray->setChecked(config->minimizeToTray());
	checkBox_SaveFileDlgHistory->setChecked(config->saveFileDlgHistory());
	checkBox_AskBeforeDelete->setChecked(config->askBeforeDelete());

	switch(config->groupTreeState()){
		case KpxConfig::RestoreLast:
			Radio_GroupTreeRestore->setChecked(true);
			break;
		case KpxConfig::ExpandAll:
			Radio_GroupTreeExpand->setChecked(true);
			break;
		default:
			Radio_GroupTreeDoNothing->setChecked(true);
	}

	//Appearance
	QPixmap *pxt=new QPixmap(pixmTextColor->width(),pixmTextColor->height());
	pxt->fill(config->bannerTextColor());
	pixmTextColor->clear();
	pixmTextColor->setPixmap(*pxt);

	QPixmap *px1=new QPixmap(pixmColor1->width(),pixmColor1->height());
	px1->fill(config->bannerColor1());
	pixmColor1->clear();
	pixmColor1->setPixmap(*px1);

	QPixmap *px2=new QPixmap(pixmColor2->width(),pixmColor2->height());
	px2->fill(config->bannerColor2());
	pixmColor2->clear();
	pixmColor2->setPixmap(*px2);

	color1=config->bannerColor1();
	color2=config->bannerColor2();
	textcolor=config->bannerTextColor();
	CheckBox_AlternatingRowColors->setChecked(config->alternatingRowColors());


	//Security
	SpinBox_ClipboardTime->setValue(config->clipboardTimeOut());
	CheckBox_ShowPasswords->setChecked(config->showPasswords());
	CheckBox_ShowPasswords_PasswordDlg->setChecked(config->showPasswordsPasswordDlg());


	//Desktop Integration
	if(PluginLoadError==QString())
		Label_IntPlugin_Error->hide();
	else
		Label_IntPlugin_Error->setText(QString("<html><p style='font-weight:600; color:#8b0000;'>%1</p></body></html>")
				.arg(tr("Error: %1")).arg(PluginLoadError));

	switch(config->integrPlugin()){
		case KpxConfig::KDE:
			Radio_IntPlugin_Kde->setChecked(true);
			break;
		case KpxConfig::Gnome:
			Radio_IntPlugin_Gnome->setChecked(true);
			break;
		default:
			Radio_IntPlugin_None->setChecked(true);
	}
	if(!PluginsModified)
		Label_IntPlugin_Info->hide();


	//Advanced
	QString BrowserCmd=config->urlCmd();
	if(BrowserCmd.isEmpty()){
		CheckBox_BrowserDefault->setChecked(true);
		Edit_BrowserCmd->setDisabled(true);
	}
	else{
		Edit_BrowserCmd->setText(BrowserCmd);
		CheckBox_BrowserDefault->setChecked(false);
	}

	Edit_MountDir->setText(config->mountDir());
	CheckBox_SaveRelativePaths->setChecked(config->saveRelativePaths());

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CSettingsDlg::OnCheckBoxBrowserDefaultChanged(int state){
	if(state==Qt::Checked){
		Edit_BrowserCmd->setDisabled(true);
		Edit_BrowserCmd->setText(QString());
	}
	else{
		Edit_BrowserCmd->setDisabled(false);
	}
}

void CSettingsDlg::OnOK()
{
	apply();
	accept();
}

void CSettingsDlg::OnCancel()
{
	reject();
}

void CSettingsDlg::OnOtherButton(QAbstractButton* button){
	if(DialogButtons->buttonRole(button)==QDialogButtonBox::ApplyRole)
		apply();
}

void CSettingsDlg::apply(){

	//General
	config->setShowSysTrayIcon(checkBox_ShowSysTrayIcon->isChecked());
	config->setMinimizeToTray(checkBox_MinimizeToTray->isChecked());
	config->setSaveFileDlgHistory(checkBox_SaveFileDlgHistory->isChecked());
	config->setEnableBookmarkMenu(checkBox_EnableBookmarkMenu->isChecked());
	if(Radio_GroupTreeRestore->isChecked())config->setGroupTreeState(KpxConfig::RestoreLast);
	else if(Radio_GroupTreeExpand->isChecked())config->setGroupTreeState(KpxConfig::ExpandAll);
	else config->setGroupTreeState(KpxConfig::DoNothing);
	config->setOpenLastFile(CheckBox_OpenLast->isChecked());
	config->setRememberLastKey(CheckBox_RememberLastKey->isChecked());
	config->setAskBeforeDelete(checkBox_AskBeforeDelete->isChecked());

	//Appearence
	config->setBannerColor1(color1);
	config->setBannerColor2(color2);
	config->setBannerTextColor(textcolor);
	config->setAlternatingRowColors(CheckBox_AlternatingRowColors->isChecked());

	//Security
	config->setClipboardTimeOut(SpinBox_ClipboardTime->value());
	config->setShowPasswords(CheckBox_ShowPasswords->isChecked());
	config->setShowPasswordsPasswordDlg(CheckBox_ShowPasswords_PasswordDlg->isChecked());

	//Desktop Integration
	PluginsModified=Label_IntPlugin_Info->isVisible();
	if(Radio_IntPlugin_Kde->isChecked())config->setIntegrPlugin(KpxConfig::KDE);
	else if(Radio_IntPlugin_Gnome->isChecked())config->setIntegrPlugin(KpxConfig::Gnome);
	else config->setIntegrPlugin(KpxConfig::NoIntegr);

	//Advanced
	config->setUrlCmd(Edit_BrowserCmd->text());
	config->setMountDir(Edit_MountDir->text());
	if(!config->mountDir().isEmpty() && config->mountDir().right(1)!="/")
		config->setMountDir(config->mountDir()+"/");
	if(CheckBox_BrowserDefault->isChecked())config->setUrlCmd(QString());
	else config->setUrlCmd(Edit_BrowserCmd->text());
	config->setSaveRelativePaths(CheckBox_SaveRelativePaths->isChecked());

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
	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
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
		createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
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
		createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width(),color1,color2,textcolor);
	}
}

void CSettingsDlg::OnCheckBoxOpenLastChanged(int state){
if(state==Qt::Checked){
	CheckBox_RememberLastKey->setEnabled(true);
}else{
	CheckBox_RememberLastKey->setEnabled(false);
	CheckBox_RememberLastKey->setChecked(false);
}
}

void CSettingsDlg::OnMountDirBrowse(){
QString dir=QFileDialog::getExistingDirectory(this,tr("Select a directory..."),"/");
if(dir!=QString()){
	Edit_MountDir->setText(dir);
}
}

void CSettingsDlg::OnIntPluginNone(bool toggled){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginGnome(bool toggled){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginKde(bool toggled){
	Label_IntPlugin_Info->show();
}


void CSettingsDlg::OnCustomizeEntryDetails(){
	CustomizeDetailViewDialog dlg(this);
	dlg.exec();
}
