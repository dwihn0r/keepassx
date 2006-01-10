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
#include <qcombobox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QShowEvent>
#include "main.h"
#include "DatabaseSettingsDlg.h"


CDbSettingsDlg::CDbSettingsDlg(QWidget* parent,Database* db, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
database=db;
connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK() ) );
connect( ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
}

CDbSettingsDlg::~CDbSettingsDlg()
{
}

void CDbSettingsDlg::showEvent(QShowEvent *event){
if(event->spontaneous()==false){
createBanner(Banner,Icon_Settings32x32,"Einstellungen");
ComboAlgo->insertItem(trUtf8("AES(Rijndael):  256 Bit   (Standard)"),0);
ComboAlgo->insertItem(trUtf8("Twofish:  256 Bit"),1);
ComboAlgo->setCurrentItem(database->CryptoAlgorithmus); //Achtung: AlgoID muss gleich dem ComboBox Index sein!
EditRounds->setText(QString::number(database->KeyEncRounds));

}
}

void CDbSettingsDlg::OnCancel()
{
done(0);
}


void CDbSettingsDlg::OnOK()
{
if(EditRounds->text()==""){
QMessageBox::warning(NULL,trUtf8("Fehler"),trUtf8("Geben Sie bitte die Anzahl der Verschlüsselungsrunden an."),trUtf8("OK"));
return;
}
bool valid;
int rounds=EditRounds->text().toUInt(&valid,10);
if(valid==false){
QMessageBox::warning(NULL,trUtf8("Fehler"),EditRounds->text()+trUtf8(" ist kein gültiger Zahlenwert"),trUtf8("OK"));
return;
}
if(rounds==0){
QMessageBox::warning(NULL,trUtf8("Fehler"),trUtf8("Die Anzahl an Verschlüsselungsrunden muss mindestens 1 betragen."),trUtf8("OK"));
return;
}
database->KeyEncRounds=rounds;
database->CryptoAlgorithmus=ComboAlgo->currentItem();

done(1);
}





/*$SPECIALIZATION$*/


//#include "databasesettingsdlg.moc"

