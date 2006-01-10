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
//Added by qt3to4:
#include <QShowEvent>
#include "EditGroupDlg.h"


CEditGroupDialog::CEditGroupDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
IconID=0;
}

CEditGroupDialog::~CEditGroupDialog()
{
}

void CEditGroupDialog::showEvent(QShowEvent *event){
if(event->spontaneous()==false){
EditTitle->setText(GroupName);
for(int i=0;i<52;i++){
ComboIconPicker->insertItem(EntryIcons[i],"",i);
}
ComboIconPicker->setCurrentItem(IconID);
}}

void CEditGroupDialog::OnOK()
{
GroupName=EditTitle->text();
IconID=ComboIconPicker->currentItem();
OK=true;
close();
}

void CEditGroupDialog::OnCancel()
{
OK=false;
close();
}




/*$SPECIALIZATION$*/


//#include "editgroupdlg.moc"

