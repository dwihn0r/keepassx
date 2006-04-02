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

#include <qcombobox.h>
#include <qlineedit.h>
#include <QShowEvent>
#include "EditGroupDlg.h"
#include "SelectIconDlg.h"


CEditGroupDialog::CEditGroupDialog(Database* database,QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
db=database;
IconID=0;
connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( OnOK() ) );
connect( ButtonCancel, SIGNAL( clicked() ), this, SLOT( OnCancel() ) );
connect( Button_Icon, SIGNAL( clicked() ), this, SLOT( OnIconDlg() ));
}

CEditGroupDialog::~CEditGroupDialog()
{
}

void CEditGroupDialog::showEvent(QShowEvent *event){
if(event->spontaneous()==false){
	EditTitle->setText(GroupName);
	for(int i=0;i<db->numIcons();i++){
		ComboIconPicker->insertItem(db->icon(i),"",i);
	}
	ComboIconPicker->setCurrentItem(IconID);
}
}

void CEditGroupDialog::OnOK()
{
GroupName=EditTitle->text();
IconID=ComboIconPicker->currentItem();
done(1);
}

void CEditGroupDialog::OnCancel()
{
done(0);
}


void CEditGroupDialog::OnIconDlg(){
CSelectIconDlg dlg(db,this);
int r=dlg.exec();
if(r!=-1){
	ComboIconPicker->clear();
	for(int i=0;i<db->numIcons();i++)
		ComboIconPicker->insertItem(db->icon(i),"",i);
	IconID=r;
	ComboIconPicker->setCurrentItem(IconID);
}
}
