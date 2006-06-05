/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   mail@tarek-saidi.de                                                   *
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
#include "PwManager.h"
#include "SearchDlg.h"
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qmessagebox.h>

CSearchDlg::CSearchDlg(Database* _db,CGroup* pGroup,QWidget* parent,  bool modal, Qt::WFlags fl)
: QDialog(parent,fl)
{
setupUi(this);
connect( Button_Search, SIGNAL( clicked() ), this, SLOT( OnButtonSearch() ) );
connect( Button_Close, SIGNAL( clicked() ), this, SLOT( OnButtonClose() ) );

createBanner(Banner,Icon_Search32x32,tr("Search"));
checkBox_Cs->setChecked(config.SearchOptions[0]);
checkBox_regExp->setChecked(config.SearchOptions[1]);
checkBox_Title->setChecked(config.SearchOptions[2]);
checkBox_Username->setChecked(config.SearchOptions[3]);
checkBox_Password->setChecked(config.SearchOptions[4]);
checkBox_Comment->setChecked(config.SearchOptions[5]);
checkBox_URL->setChecked(config.SearchOptions[6]);
checkBox_Attachment->setChecked(config.SearchOptions[7]);
if(pGroup)
 checkBox_Recursive->setChecked(config.SearchOptions[8]);
else{
 checkBox_Recursive->setChecked(false);
 checkBox_Recursive->setEnabled(false);}

db=_db;
group=pGroup;
}

CSearchDlg::~CSearchDlg()
{
config.SearchOptions[0]=checkBox_Cs->isChecked();
config.SearchOptions[1]=checkBox_regExp->isChecked();
config.SearchOptions[2]=checkBox_Title->isChecked();
config.SearchOptions[3]=checkBox_Username->isChecked();
config.SearchOptions[4]=checkBox_Password->isChecked();
config.SearchOptions[5]=checkBox_Comment->isChecked();
config.SearchOptions[6]=checkBox_URL->isChecked();
config.SearchOptions[7]=checkBox_Attachment->isChecked();
if(group) config.SearchOptions[8]=checkBox_Recursive->isChecked();
}

void CSearchDlg::OnButtonClose()
{
done(0);
}

void CSearchDlg::OnButtonSearch()
{
Hits.clear();
txt=Edit_Search->text();
regexp=checkBox_regExp->isChecked();
if(txt==""){
QMessageBox::information(this,tr("Notice"),tr("Please enter a search string."),tr("OK"));
return;}

for(int i=0;i<db->numEntries();i++){
 if(group){
	if(checkBox_Recursive->isChecked()){
		QList<int> groups=db->getChildIds(group);
		groups << group->ID;
		bool IsInAnyGroup=false;
		for(int j=0; j<groups.size();j++){
			if(db->entry(i).GroupID == groups[j]){IsInAnyGroup=true; break;}}
		if(!IsInAnyGroup)continue;
	}
	else
		if(db->entry(i).GroupID != group->ID)continue;
 }

bool hit=false;
 if(checkBox_Title->isChecked())	hit=hit||search(db->entry(i).Title);
 if(checkBox_Username->isChecked())	hit=hit||search(db->entry(i).UserName);
 if(checkBox_URL->isChecked())		hit=hit||search(db->entry(i).URL);
 if(checkBox_Comment->isChecked())	hit=hit||search(db->entry(i).Additional);
 if(checkBox_Attachment->isChecked())	hit=hit||search(db->entry(i).BinaryDesc);
 db->entry(i).Password.unlock();
 if(checkBox_Password->isChecked())	hit=hit||search(db->entry(i).Password.string());
 db->entry(i).Password.lock();
 if(hit)Hits.push_back(db->entry(i).sID);
}

done(1);

}


bool CSearchDlg::search(const QString& str){

if(regexp){
 QRegExp exp(txt,checkBox_Cs->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
 if(str.contains(exp)==0)return false;}
else{
 if(str.contains(txt,checkBox_Cs->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)==0)return false;}
return true;

}


