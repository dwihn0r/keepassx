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
#include "PwManager.h"
#include "SearchDlg.h"
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qmessagebox.h>

CSearchDlg::CSearchDlg(CGroup* pGroup,QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
createBanner(Banner,Icon_Search32x32,tr("Suchen"));
checkBox_Cs->setChecked(config.SearchOptions[0]);
checkBox_regExp->setChecked(config.SearchOptions[1]);
checkBox_Title->setChecked(config.SearchOptions[2]);
checkBox_Username->setChecked(config.SearchOptions[3]);
checkBox_Password->setChecked(config.SearchOptions[4]);
checkBox_Comment->setChecked(config.SearchOptions[5]);
checkBox_URL->setChecked(config.SearchOptions[6]);
checkBox_Attachment->setChecked(config.SearchOptions[7]);
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
}

void CSearchDlg::OnButtonClose()
{
done(0);
}

void CSearchDlg::OnButtonSearch()
{
/*
txt=Edit_Search->text();
regexp=checkBox_regExp->isChecked();
if(txt==""){
QMessageBox::information(this,trUtf8("Hinweis"),trUtf8("Bitte geben Sie einen Suchbegriff ein."),"OK",0,0);
return;}

for(int i=0;i<pw->Entries.size();i++){
 if(group){if(pw->Entries[i].GroupID != group->ID)continue;}
 bool hit=false;
 if(checkBox_Title->isChecked())	hit=hit||search(pw->Entries[i].Title);
 if(checkBox_Username->isChecked())	hit=hit||search(pw->Entries[i].UserName);
 if(checkBox_URL->isChecked())		hit=hit||search(pw->Entries[i].URL);
 if(checkBox_Comment->isChecked())	hit=hit||search(pw->Entries[i].Additional);
 if(checkBox_Attachment->isChecked())	hit=hit||search(pw->Entries[i].BinaryDesc);
 if(checkBox_Password->isChecked())	hit=hit||search(pw->Entries[i].Password.getString());
 pw->Entries[i].Password.delRef();
 if(hit)hits.push_back(&pw->Entries[i]);
}
*/
done(1);

}

/*
bool CSearchDlg::search(QString& str){
if(regexp){
 QRegExp exp(txt,checkBox_Cs->isChecked());
 if(str.contains(exp)==0)return false;}
else{
 if(str.contains(txt,checkBox_Cs->isChecked())==0)return false;}
return true;
}*/

/*$SPECIALIZATION$*/


