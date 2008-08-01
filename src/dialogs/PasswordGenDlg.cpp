/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
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


#include "dialogs/PasswordGenDlg.h"
#include "dialogs/CollectEntropyDlg.h"

#include "apg/randpass.h"
#include "apg/pronpass.h"

bool CGenPwDialog::EntropyCollected=false;

CGenPwDialog::CGenPwDialog(QWidget* parent, bool StandAloneMode,Qt::WFlags fl)
: QDialog(parent,fl)
{
	setupUi(this);
	connect(ButtonGenerate, SIGNAL(clicked()), SLOT(OnGeneratePw()));
	connect(Radio_1, SIGNAL(toggled(bool)), SLOT(OnRadio1StateChanged(bool)));
	connect(Radio_2, SIGNAL(toggled(bool)), SLOT(OnRadio2StateChanged(bool)));
	connect(DialogButtons, SIGNAL(rejected()), SLOT(OnCancel()));
	connect(DialogButtons, SIGNAL(accepted()), SLOT(OnAccept()));
	connect(tabCategory, SIGNAL(currentChanged(int)), SLOT(estimateQuality()));
	connect(Radio_1, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Radio_2, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox1, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox2, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox3, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox4, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox5, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox6, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBox7, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Edit_chars, SIGNAL(textChanged(const QString&)), SLOT(estimateQuality()));
	connect(checkBoxPU, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPL, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPN, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(checkBoxPS, SIGNAL(toggled(bool)), SLOT(estimateQuality()));
	connect(Spin_Num, SIGNAL(valueChanged(int)), SLOT(estimateQuality()));
	connect(Check_CollectEntropy, SIGNAL(stateChanged(int)), SLOT(OnCollectEntropyChanged(int)));
	connect(Edit_chars, SIGNAL(textEdited(const QString&)), SLOT(OnCharsChanged(const QString&)));
	connect(ButtonChangeEchoMode, SIGNAL(clicked()), SLOT(SwapEchoMode()));
	connect(tabCategory, SIGNAL(currentChanged(int)), SLOT(setGenerateEnabled()));
	connect(Radio_1, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(Radio_2, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox1, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox2, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox3, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox4, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox5, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox6, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBox7, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(Edit_chars, SIGNAL(textChanged(const QString&)), SLOT(setGenerateEnabled()));
	connect(checkBoxPU, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPL, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPN, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));
	connect(checkBoxPS, SIGNAL(toggled(bool)), SLOT(setGenerateEnabled()));

	if(!StandAloneMode){
		AcceptButton=DialogButtons->addButton(QDialogButtonBox::Ok);
		AcceptButton->setEnabled(false);
		DialogButtons->addButton(QDialogButtonBox::Cancel);
		connect(Edit_dest, SIGNAL(textChanged(const QString&)), SLOT(setAcceptEnabled(const QString&)));
	}
	else{
		DialogButtons->addButton(QDialogButtonBox::Close);
		AcceptButton=NULL;
	}

	tabCategory->setCurrentIndex(config->pwGenCategory());
	QBitArray pwGenOptions=config->pwGenOptions();
	Radio_1->setChecked(pwGenOptions.at(0));
	Radio_2->setChecked(!pwGenOptions.at(0));
	checkBox1->setChecked(pwGenOptions.at(1));
	checkBox2->setChecked(pwGenOptions.at(2));
	checkBox3->setChecked(pwGenOptions.at(3));
	checkBox4->setChecked(pwGenOptions.at(4));
	checkBox5->setChecked(pwGenOptions.at(5));
	checkBox6->setChecked(pwGenOptions.at(6));
	checkBox7->setChecked(pwGenOptions.at(7));
	Check_CollectEntropy->setChecked(pwGenOptions.at(8));
	Check_CollectOncePerSession->setChecked(pwGenOptions.at(9));
	OnRadio1StateChanged(pwGenOptions.at(0));
	OnRadio2StateChanged(!pwGenOptions.at(0));
	if (pwGenOptions.size()>=14){
		checkBoxPU->setChecked(pwGenOptions.at(10));
		checkBoxPL->setChecked(pwGenOptions.at(11));
		checkBoxPN->setChecked(pwGenOptions.at(12));
		checkBoxPS->setChecked(pwGenOptions.at(13));
	}
	else{
		checkBoxPU->setChecked(true);
		checkBoxPL->setChecked(true);
		checkBoxPN->setChecked(true);
		checkBoxPS->setChecked(false);
	}
	Spin_Num->setValue(config->pwGenLength());
	adjustSize();
	setMaximumSize(size());
	setMinimumSize(size());
	createBanner(&BannerPixmap,getPixmap("dice"),tr("Password Generator"),width());
	
	if(!config->showPasswords())
		SwapEchoMode();
	else
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
}

CGenPwDialog::~CGenPwDialog(){
	config->setPwGenCategory(tabCategory->currentIndex());
	QBitArray pwGenOptions(14);
	pwGenOptions.setBit(0,Radio_1->isChecked());
	pwGenOptions.setBit(1,checkBox1->isChecked());
	pwGenOptions.setBit(2,checkBox2->isChecked());
	pwGenOptions.setBit(3,checkBox3->isChecked());
	pwGenOptions.setBit(4,checkBox4->isChecked());
	pwGenOptions.setBit(5,checkBox5->isChecked());
	pwGenOptions.setBit(6,checkBox6->isChecked());
	pwGenOptions.setBit(7,checkBox7->isChecked());
	pwGenOptions.setBit(8,Check_CollectEntropy->isChecked());
	pwGenOptions.setBit(9,Check_CollectOncePerSession->isChecked());
	pwGenOptions.setBit(10,checkBoxPU->isChecked());
	pwGenOptions.setBit(11,checkBoxPL->isChecked());
	pwGenOptions.setBit(12,checkBoxPN->isChecked());
	pwGenOptions.setBit(13,checkBoxPS->isChecked());
	config->setPwGenOptions(pwGenOptions);
	config->setPwGenLength(Spin_Num->value());
}

void CGenPwDialog::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void CGenPwDialog::OnRadio1StateChanged(bool state)
{
	if(state){
		checkBox1->setEnabled(true);
		checkBox2->setEnabled(true);
		checkBox3->setEnabled(true);
		checkBox4->setEnabled(true);
		checkBox5->setEnabled(true);
		checkBox6->setEnabled(true);
		checkBox7->setEnabled(true);
	}else{
		checkBox1->setDisabled(true);
		checkBox2->setDisabled(true);
		checkBox3->setDisabled(true);
		checkBox4->setDisabled(true);
		checkBox5->setDisabled(true);
		checkBox6->setDisabled(true);
		checkBox7->setDisabled(true);
	}
}

void CGenPwDialog::OnRadio2StateChanged(bool state){
	if(state)
		Edit_chars->setEnabled(true);
	else
		Edit_chars->setDisabled(true);
}

void CGenPwDialog::OnGeneratePw()
{
	int length = Spin_Num->value();
	char* buffer = new char[length+1];
	
	if (tabCategory->currentIndex()==1)
	{
		unsigned int mode = 0;
		if (checkBoxPU->isChecked())
			mode |= S_CL;
		if (checkBoxPL->isChecked())
			mode |= S_SL;
		if (checkBoxPN->isChecked())
			mode |= S_NB;
		if (checkBoxPS->isChecked())
			mode |= S_SS;
		
		char* hyphenated_word = new char[length*18+1];
		gen_pron_pass(buffer, hyphenated_word, length, length, mode);
		delete[] hyphenated_word;
	}
	else if (Radio_1->isChecked() && !checkBox5->isChecked() &&
	         !checkBox6->isChecked() && !checkBox7->isChecked())
	{
		unsigned int mode = 0;
		if (checkBox1->isChecked())
			mode |= S_CL;
		if (checkBox2->isChecked())
			mode |= S_SL;
		if (checkBox3->isChecked())
			mode |= S_NB;
		if (checkBox4->isChecked())
			mode |= S_SS;
		
		gen_rand_pass(buffer, length, length, mode);
	}
	else{
		generatePasswordInternal(buffer, length);
	}

	Edit_dest->setText(buffer);
	delete[] buffer;
}

int CGenPwDialog::AddToAssoctable(char* table,int start,int end,int pos){
	for(int i=start;i<=end;i++){
		table[pos]=i;
		pos++;
	}
	return (end-start)+1;
}


bool CGenPwDialog::trim(unsigned char &x, int r){
	if(x<r)
		return true;
	if(256%r!=0)
		return false;
	x=x-(x/r)*r;
	return true;
}

void CGenPwDialog::estimateQuality(){
	int num=0;
	if (tabCategory->currentIndex()==0){
		if(Radio_1->isChecked()){
			if(checkBox1->isChecked())
				num+=26;
			if(checkBox2->isChecked())
				num+=26;
			if(checkBox3->isChecked())
				num+=10;
			if(checkBox4->isChecked())
				num+=32;
			if(checkBox5->isChecked())
				num++;
			if(checkBox6->isChecked() && !checkBox4->isChecked())
				num++;
			if(checkBox7->isChecked() && !checkBox4->isChecked())
				num++;
		}
		else
			num=Edit_chars->text().length();
	}
	else{
		if (checkBoxPU->isChecked())
			num+=26;
		if (checkBoxPL->isChecked())
			num+=26;
		if (checkBoxPN->isChecked())
			num+=10;
		if (checkBoxPS->isChecked())
			num+=32;
	}

	float bits=0;
	if(num)
		bits=log((float)num)/log(2.0f);
	bits=bits*((float)Spin_Num->value());
	Progress_Quali->setFormat(tr("%1 Bits").arg((int)bits));
	Progress_Quali->update();
	if(bits>128)bits=128;
	Progress_Quali->setValue((int)bits);
}

void CGenPwDialog::OnCharsChanged(const QString& str){
	bool multiple=false;
	for(int i=0;i<str.size();i++){
		int count=0;
		for(int j=0;j<str.size();j++){
			if(str[i]==str[j]){
				if(count){
					multiple=true;
					break;
				}
				else {
					count++;
				}
			}
		}
		if(multiple)break;
	}
	if(!multiple)return;

	QString newstr;
	for(int i=0;i<str.size();i++){
		if(!newstr.count(str[i])){
			newstr+=str[i];
		}
	}
	Edit_chars->setText(newstr);

}

void CGenPwDialog::OnAccept()
{
	done(1);
}

void CGenPwDialog::OnCancel()
{
	done(0);
}

void CGenPwDialog::OnCollectEntropyChanged(int state){
	if(state==Qt::Checked)
		Check_CollectOncePerSession->setDisabled(false);
	else
		Check_CollectOncePerSession->setDisabled(true);

}

void CGenPwDialog::SwapEchoMode(){
	if(Edit_dest->echoMode()==QLineEdit::Normal){
		Edit_dest->setEchoMode(QLineEdit::Password);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_hide"));
	}
	else{
		Edit_dest->setEchoMode(QLineEdit::Normal);
		ButtonChangeEchoMode->setIcon(getIcon("pwd_show"));
	}
}

void CGenPwDialog::generatePasswordInternal(char* buffer, int length){
	/*-------------------------------------------------------
	     ASCII
	  -------------------------------------------------------
	  "A...Z" 65...90
	  "a...z" 97...122
	  "0...9" 48...57
	  Special Characters 33...47; 58...64; 91...96; 123...126
	  "-" 45
	  "_" 95
	  -------------------------------------------------------
	*/

	int num=0;
	char assoctable[255];
	
	if(Radio_1->isChecked()){
		if(checkBox1->isChecked())
			num+=AddToAssoctable(assoctable,65,90,num);
		if(checkBox2->isChecked())
			num+=AddToAssoctable(assoctable,97,122,num);
		if(checkBox3->isChecked())
			num+=AddToAssoctable(assoctable,48,57,num);
		if(checkBox4->isChecked()){
			num+=AddToAssoctable(assoctable,33,47,num);
			num+=AddToAssoctable(assoctable,58,64,num);
			num+=AddToAssoctable(assoctable,91,96,num);
			num+=AddToAssoctable(assoctable,123,126,num);
		}
		if(checkBox5->isChecked())
			num+=AddToAssoctable(assoctable,32,32,num);
		if(checkBox6->isChecked() && !checkBox4->isChecked())
			num+=AddToAssoctable(assoctable,45,45,num);
		if(checkBox7->isChecked() && !checkBox4->isChecked())
			num+=AddToAssoctable(assoctable,95,95,num);
	}
	else{
		QString str=Edit_chars->text();
		for(int i=0;i<str.length();i++){
			assoctable[i]=str[i].toAscii();
			num++;
		}
	}
	
	buffer[length]=0;

	if(Check_CollectEntropy->isChecked()){
		if((Check_CollectOncePerSession->isChecked() && !EntropyCollected) || !Check_CollectOncePerSession->isChecked()){
			CollectEntropyDlg dlg(this);
			dlg.exec();
			EntropyCollected=true;
		}
	}

	unsigned char tmp;
	for(int i=0;i<length;i++){

		do randomize(&tmp,1);
		while(!trim(tmp,num));

		buffer[i]=assoctable[tmp];
	}
}

void CGenPwDialog::setGenerateEnabled(){
	bool enable;
	
	if (tabCategory->currentIndex()==0){
		if (Radio_1->isChecked()){
			enable = checkBox1->isChecked() || checkBox2->isChecked() || checkBox3->isChecked() || 
					 checkBox4->isChecked() || checkBox5->isChecked() || checkBox6->isChecked() || 
					 checkBox7->isChecked();
		}
		else{
			enable = !Edit_chars->text().isEmpty();
		}
	}
	else{
		enable = checkBoxPU->isChecked() || checkBoxPL->isChecked() ||
				 checkBoxPN->isChecked() || checkBoxPS->isChecked();
	}
	
	ButtonGenerate->setEnabled(enable);
}

void CGenPwDialog::setAcceptEnabled(const QString& str){
	AcceptButton->setEnabled(!str.isEmpty());
}
