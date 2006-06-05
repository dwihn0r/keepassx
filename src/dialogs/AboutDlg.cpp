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

#include <qmessagebox.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qfile.h>

#include "main.h"
#include "AboutDlg.h"

CAboutDialog::CAboutDialog(QWidget* parent,bool modal, Qt::WFlags fl)
: QDialog(parent,fl)
{
setupUi(this);
createBanner(Banner,Icon_Key32x32,tr("KeePassX %1").arg(KEEPASS_VERSION));
loadLicFromFile();

QString AboutTr=tr("<b>Current Translation: None</b><br><br>","Please replace 'None' with the language of your translation");
if(TrActive){
	AboutTr+=tr("<b>Author:</b> %1<br>").arg(tr("$TRANSLATION_AUTHOR"));
	QString mail=tr("$TRANSLATION_AUTHOR_EMAIL","Here you can enter your email or homepage if you want.");
	if(mail!=QString()){
		AboutTr+=mail+"<br>";
	}
	AboutTr+="<br>";
}
Edit_Translation->setHtml(AboutTr+tr("\
Information on how to translate KeePassX can be found under:\n\
http://keepass.berlios.de/"));
QString str;
str+="<b>"+tr("Team")+"</b><br>";
str+="<div style='margin-left:10px;'>";
str+="<u>"+tr("Tarek Saidi")+"</u><br>"+tr("Developer, Project Admin")+"<br>"+tr("tariq@users.berlios.de")+"<br>";
str+="<br>";
str+="<u>"+tr("Eugen Gorschenin")+"</u><br>"+tr("Web Designer")+"<br>"+tr("geugen@users.berlios.de")+"<br>";
str+="</div><br><div style='margin-left:0px;'>";
str+="<b>"+tr("Thanks To")+"</b>";
str+="</div><div style='margin-left:10px;'>";
str+="<u>"+tr("Matthias Miller")+"</u><br>"+tr("Patches for better MacOS X support")+"<br>"+tr("www.outofhanwell.com")+"<br></div>";
Edit_Thanks->setHtml(str);
}

CAboutDialog::~CAboutDialog()
{

}

void CAboutDialog::OnClose()
{
close();
}

void CAboutDialog::loadLicFromFile(){

QFile gpl(AppDir+"/../share/keepass/license.html");
if(!gpl.exists()){
QMessageBox::critical(this,tr("Error"),tr("File '%1' could not be found.")
			  .arg("'license.html'")+"\n"+tr("Make sure that the program is installed correctly.")
			  ,tr("OK"),0,0,2,1);
return;
}

if(!gpl.open(QIODevice::ReadOnly)){
QMessageBox::critical(this,tr("Error"),tr("Could not open file '%1'")
			  .arg("'license.txt'")+tr("The following error occured:\n%1").arg(gpl.errorString())			 
			  ,tr("OK"),0,0,2,1);
return;
}

char* buffer=new char[gpl.size()];
long l=gpl.read(buffer,gpl.size());
gpl.close();
Edit_License->setHtml(QString::fromUtf8(buffer,l));
delete buffer;
}

void CAboutDialog::OnHomepageClicked(){
openBrowser(tr("http://keepass.berlios.de/index.php"));
}

void CAboutDialog::OnEMailClicked(){
openBrowser("mailto:tarek.saidi@arcor.de");
}

