/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
#include <QPixmap>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QPainter>
#include "SettingsDlg.h"
#include "CustomizeDetailViewDlg.h"
#include "FileDialogs.h"

bool CSettingsDlg::PluginsModified=false;


CSettingsDlg::CSettingsDlg(QWidget* parent):QDialog(parent,Qt::Dialog)
{
	setupUi(this);
	connect(DialogButtons, SIGNAL( accepted() ), this, SLOT( OnOK() ) );
	connect(DialogButtons, SIGNAL( rejected() ), this, SLOT( OnCancel() ) );
	connect(DialogButtons, SIGNAL( clicked(QAbstractButton*)), this, SLOT(OnOtherButton(QAbstractButton*)));
	
	connect(CheckBox_ShowSysTrayIcon, SIGNAL( toggled(bool) ), CheckBox_CloseToTray, SLOT( setEnabled(bool) ) );
	connect(CheckBox_ShowSysTrayIcon, SIGNAL( toggled(bool) ), CheckBox_MinimizeTray, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_RememberLastKey, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_StartMinimized, SLOT( setEnabled(bool) ) );
	connect(CheckBox_OpenLast, SIGNAL( toggled(bool) ), CheckBox_StartLocked, SLOT( setEnabled(bool) ) );
	
	connect(Button_ClearFileDlgHistory, SIGNAL(clicked()), &fileDlgHistory, SLOT(clear()));
	connect(ButtonColor1, SIGNAL( clicked() ), this, SLOT( OnColor1() ) );
	connect(ButtonColor2, SIGNAL( clicked() ), this, SLOT( OnColor2() ) );
	connect(ButtonTextColor, SIGNAL( clicked() ), this, SLOT( OnTextColor() ) );
	connect(Button_MountDirBrowse,SIGNAL(clicked()),this,SLOT(OnMountDirBrowse()));
	connect(Button_BrowserCmdBrowse,SIGNAL(clicked()),this,SLOT(OnBrowserCmdBrowse()));

	connect(Radio_IntPlugin_None,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginNone()));
	connect(Radio_IntPlugin_Gnome,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginGnome()));
	connect(Radio_IntPlugin_Kde,SIGNAL(toggled(bool)),this,SLOT(OnIntPluginKde()));

	connect(Button_CustomizeEntryDetails,SIGNAL(clicked()),this,SLOT(OnCustomizeEntryDetails()));
	
#if !defined(AUTOTYPE)
	Box_AutoType->setVisible(false);
#endif
#if !defined(GLOBAL_AUTOTYPE)
	Label_GlobalShortcut->setVisible(false);
	Edit_GlobalShortcut->setVisible(false);
#endif
	
#ifdef GLOBAL_AUTOTYPE
	pShortcut = AutoType::shortcut;
	connect(this,SIGNAL(rejected()),SLOT(resetGlobalShortcut()));
#endif

	createBanner(&BannerPixmap,getPixmap("appsettings"),tr("Settings"),width());

	//General
	CheckBox_OpenLast->setChecked(config->openLastFile());
	CheckBox_RememberLastKey->setChecked(config->rememberLastKey());
	CheckBox_ShowSysTrayIcon->setChecked(config->showSysTrayIcon());
	CheckBox_CloseToTray->setChecked(config->minimizeToTray());
	CheckBox_MinimizeTray->setChecked(config->minimizeTray());
	CheckBox_StartMinimized->setChecked(config->startMinimized());
	CheckBox_StartLocked->setChecked(config->startLocked());
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
	CheckBox_LockMinimize->setChecked(config->lockOnMinimize());
	
	//Features
	CheckBox_FeatureBookmarks->setChecked(config->featureBookmarks());


	// TODO Desktop Integration
	tabWidgetSettings->removeTab(tabWidgetSettings->indexOf(tabIntegration));
	/*if(PluginLoadError==QString())
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
		Label_IntPlugin_Info->hide();*/


	//Advanced
	Box_BrowserCmd->setChecked(!config->urlCmdDef());
	Edit_BrowserCmd->setText(config->urlCmd());

	Edit_MountDir->setText(config->mountDir());
	CheckBox_SaveRelativePaths->setChecked(config->saveRelativePaths());
#ifdef AUTOTYPE
	SpinBox_AutoTypePreGap->setValue(config->autoTypePreGap());
	SpinBox_AutoTypeKeyStrokeDelay->setValue(config->autoTypeKeyStrokeDelay());
#endif
#ifdef GLOBAL_AUTOTYPE
	Edit_GlobalShortcut->setShortcut(config->globalShortcut());
	CheckBox_EntryTitlesMatch->setChecked(config->entryTitlesMatch());
#endif
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
	config->setShowSysTrayIcon(CheckBox_ShowSysTrayIcon->isChecked());
	config->setMinimizeToTray(CheckBox_CloseToTray->isChecked());
	config->setMinimizeTray(CheckBox_MinimizeTray->isChecked());
	config->setStartMinimized(CheckBox_StartMinimized->isChecked());
	config->setStartLocked(CheckBox_StartLocked->isChecked());
	config->setSaveFileDlgHistory(checkBox_SaveFileDlgHistory->isChecked());
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
	config->setLockOnMinimize(CheckBox_LockMinimize->isChecked());
	
	//Features
	config->setFeatureBookmarks(CheckBox_FeatureBookmarks->isChecked());

	//TODO Desktop Integration
	/*PluginsModified=Label_IntPlugin_Info->isVisible();
	if(Radio_IntPlugin_Kde->isChecked())config->setIntegrPlugin(KpxConfig::KDE);
	else if(Radio_IntPlugin_Gnome->isChecked())config->setIntegrPlugin(KpxConfig::Gnome);
	else config->setIntegrPlugin(KpxConfig::NoIntegr);*/

	//Advanced
	config->setUrlCmdDef(!Box_BrowserCmd->isChecked());
	config->setUrlCmd(Edit_BrowserCmd->text());
	config->setMountDir(Edit_MountDir->text());
	if(!config->mountDir().isEmpty() && config->mountDir().right(1)!="/")
		config->setMountDir(config->mountDir()+"/");
	config->setSaveRelativePaths(CheckBox_SaveRelativePaths->isChecked());
#ifdef AUTOTYPE
	config->setAutoTypePreGap(SpinBox_AutoTypePreGap->value());
	config->setAutoTypeKeyStrokeDelay(SpinBox_AutoTypeKeyStrokeDelay->value());
#endif
#ifdef GLOBAL_AUTOTYPE
	config->setGlobalShortcut(Edit_GlobalShortcut->shortcut());
	config->setEntryTitlesMatch(CheckBox_EntryTitlesMatch->isChecked());
#endif
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

void CSettingsDlg::OnMountDirBrowse(){
	QString dir=QFileDialog::getExistingDirectory(this,tr("Select a directory..."));
	if(!dir.isEmpty()){
		Edit_MountDir->setText(dir);
	}
}

void CSettingsDlg::OnBrowserCmdBrowse(){
	QString filename=QFileDialog::getOpenFileName(this,tr("Select an executable..."));
	if(!filename.isEmpty()){
		Edit_BrowserCmd->setText(filename);
	}
}

void CSettingsDlg::OnIntPluginNone(){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginGnome(){
	Label_IntPlugin_Info->show();
}

void CSettingsDlg::OnIntPluginKde(){
	Label_IntPlugin_Info->show();
}


void CSettingsDlg::OnCustomizeEntryDetails(){
	CustomizeDetailViewDialog dlg(this);
	dlg.exec();
}

#ifdef GLOBAL_AUTOTYPE
void CSettingsDlg::resetGlobalShortcut(){
	AutoType::unregisterGlobalShortcut();
	AutoType::registerGlobalShortcut(pShortcut);
}
#endif
