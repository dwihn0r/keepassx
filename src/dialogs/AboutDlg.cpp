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
#include <qmessagebox.h>
#include <q3scrollview.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qfile.h>

#include "AboutDlg.h"

CAboutDialog::CAboutDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: AboutDlg(parent,name, modal,fl)
{
mainwnd=((CMainWindow*)parentWidget());
mainwnd->CreateBanner(Banner,mainwnd->Icon_Key32x32,trUtf8("Keepass für Linux"));
Link_Homepage=new LinkLabel(this,"Link_Homepage",trUtf8("http://keepass.de.vu"),80,143);
Link_EMail=new LinkLabel(this,"Link_EMail",trUtf8("tarek.saidi@arcor.de"),80,163);
Link_License=new LinkLabel(this,"Link_License",trUtf8("Lizenz"),80,183);
connect(Link_License,SIGNAL(clicked()),this,SLOT(OnLicenseClicked()));
connect(Link_EMail,SIGNAL(clicked()),this,SLOT(OnEMailClicked()));
connect(Link_Homepage,SIGNAL(clicked()),this,SLOT(OnHomepageClicked()));
Label0->setText(Label0->text().arg(KEEPASS_VERSION));

}

CAboutDialog::~CAboutDialog()
{
delete Link_Homepage;
delete Link_EMail;
delete Link_License;
}

void CAboutDialog::OnClose()
{
close();
}

void CAboutDialog::OnLicenseClicked(){

QDialog dlg(this,NULL,true);
Q3ScrollView scroll(&dlg);
QLabel label(&scroll,"License-Scroll");
scroll.addChild(&label);
QFile gpl(((CMainWindow*)parentWidget())->appdir+"/../share/keepass/license.txt");
if(!gpl.exists()){
QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei '%1' konnte nicht gefunden werden.")
			  .arg("'license.txt'")+"\n"+trUtf8("Die Anwendung wurde möglicherweiße nicht korrekt installiert.")
			  ,trUtf8("OK"),0,0,2,1);
return;
}

if(!gpl.open(QIODevice::ReadOnly)){
QMessageBox::critical(this,trUtf8("Fehler"),trUtf8("Die Datei '%1' konnte nicht geöffnet werden.")
			  .arg("'license.txt'")+trUtf8("Es trat folgender Fehler auf:\n%1").arg(gpl.errorString())			 
			  ,trUtf8("OK"),0,0,2,1);
return;
}
char* buffer=new char[gpl.size()];
long l=gpl.readBlock(buffer,gpl.size());
gpl.close();
label.setText(QString::fromUtf8(buffer,l));
label.setGeometry(0,0,500,800);
dlg.setFixedWidth(500);
dlg.setFixedHeight(400);
dlg.setCaption(trUtf8("Lizenz"));
scroll.setGeometry(0,0,500,400);
dlg.exec();
delete buffer;

}

void CAboutDialog::OnHomepageClicked(){
mainwnd->OpenURL("http://keepass.de.vu");
}

void CAboutDialog::OnEMailClicked(){
mainwnd->OpenURL("mailto:tarek.saidi@arcor.de");
}

