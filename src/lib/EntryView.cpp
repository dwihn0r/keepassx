/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
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


#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include "main.h"
#include "EntryView.h"

KeepassEntryView::KeepassEntryView(QWidget* parent):QTreeWidget(parent){
CurrentGroup=0;
updateColumns();
header()->setResizeMode(QHeaderView::Stretch);
ContextMenu=new QMenu(this);
}


void KeepassEntryView::contextMenuEvent(QContextMenuEvent* e){
if(itemAt(e->pos())){
	EntryViewItem* item=(EntryViewItem*)itemAt(e->pos());
	if(selectedItems().size()==0){
		setItemSelected(item,true);}
	else{
		bool AlreadySelected=false;
		for(int i=0;i<selectedItems().size();i++){
			if(selectedItems()[i]==item){AlreadySelected=true; break;}
		}
		if(!AlreadySelected){
			while(selectedItems().size()){
				setItemSelected(selectedItems()[0],false);
			}
			setItemSelected(item,true);
		}
	}
}
else
{while(selectedItems().size()){
	setItemSelected(selectedItems()[0],false);}
}

e->accept();
ContextMenu->popup(e->globalPos());
}



void KeepassEntryView::updateItems(){
clear();
Items.clear();
if(!db)return;
EntryViewItem *tmp=NULL;
for(int i=0;i<db->Entries.size();i++){
  CEntry* entry=&db->Entries[i];
  Items.push_back(tmp=new EntryViewItem(this));
  Items.back()->pEntry=entry;
  int j=0;
  if(config.Columns[0]){
    tmp->setText(j++,entry->Title);}
  if(config.Columns[1]){
    if(config.ListView_HideUsernames)
      tmp->setText(j++,"******");
    else
      tmp->setText(j++,entry->UserName);}
  if(config.Columns[2]){
    tmp->setText(j++,entry->URL);}
  if(config.Columns[3]){
    if(config.ListView_HidePasswords)
      tmp->setText(j++,"******");
    else{
      tmp->setText(j++,entry->Password.getString());
      entry->Password.delRef();}}
  if(config.Columns[4]){
    tmp->setText(j++,entry->Additional.section('\n',0,0));}
  if(config.Columns[5]){
    tmp->setText(j++,entry->Expire.toString(DateTimeFormat));}
  if(config.Columns[6]){
    tmp->setText(j++,entry->Creation.toString(DateTimeFormat));}
  if(config.Columns[7]){
    tmp->setText(j++,entry->LastMod.toString(DateTimeFormat));}
  if(config.Columns[8]){
    tmp->setText(j++,entry->LastAccess.toString(DateTimeFormat));}
  if(config.Columns[9]){
   tmp->setText(j++,entry->BinaryDesc);}
  Items.back()->setIcon(0,EntryIcons[entry->ImageID]);
}
setCurrentGroup(CurrentGroup);
}

void KeepassEntryView::setCurrentGroup(uint id){
CurrentGroup=id;
for(int i=0; i<Items.size();i++){
 setItemHidden(Items[i],(Items[i]->pEntry->GroupID != id));
}
}

void KeepassEntryView::showSearchResults(QList<Q_UINT32>& results){
setCurrentGroup(0);
for(int j=0; j<results.size(); j++){
	for(int i=0; i<Items.size();i++){
		if(Items[i]->pEntry->sID == results[j])
 		setItemHidden(Items[i],false);
	}
}
}

void KeepassEntryView::refreshVisibleItems(){
EntryViewItem *tmp=NULL;
for(int i=0;i<Items.size();i++){
  tmp=Items[i];
  CEntry* entry=tmp->pEntry;
if(entry->GroupID==CurrentGroup){
  int j=0;
  if(config.Columns[0]){
    tmp->setText(j++,entry->Title);}
  if(config.Columns[1]){
    if(config.ListView_HideUsernames)
      tmp->setText(j++,"******");
    else
      tmp->setText(j++,entry->UserName);}
  if(config.Columns[2]){
    tmp->setText(j++,entry->URL);}
  if(config.Columns[3]){
    if(config.ListView_HidePasswords)
      tmp->setText(j++,"******");
    else{
      tmp->setText(j++,entry->Password.getString());
      entry->Password.delRef();}}
  if(config.Columns[4]){
    tmp->setText(j++,entry->Additional.section('\n',0,0));}
  if(config.Columns[5]){
    tmp->setText(j++,entry->Expire.toString(DateTimeFormat));}
  if(config.Columns[6]){
    tmp->setText(j++,entry->Creation.toString(DateTimeFormat));}
  if(config.Columns[7]){
    tmp->setText(j++,entry->LastMod.toString(DateTimeFormat));}
  if(config.Columns[8]){
    tmp->setText(j++,entry->LastAccess.toString(DateTimeFormat));}
  if(config.Columns[9]){
   tmp->setText(j++,entry->BinaryDesc);}
  tmp->setIcon(0,EntryIcons[entry->ImageID]);
}
}

}


void KeepassEntryView::updateColumns(){
setColumnCount(0);
QStringList cols;
if(config.Columns[0]){
 cols << trUtf8("Titel");}
if(config.Columns[1]){
 cols << trUtf8("Benutzername");}
if(config.Columns[2]){
 cols << trUtf8("URL");}
if(config.Columns[3]){
 cols << trUtf8("Passwort");}
if(config.Columns[4]){
 cols << trUtf8("Kommentare");}
if(config.Columns[5]){
 cols << trUtf8("Gültig bis");}
if(config.Columns[6]){
 cols << trUtf8("Erstellung");}
if(config.Columns[7]){
 cols << trUtf8("letzte Ãnderung");}
if(config.Columns[8]){
 cols << trUtf8("letzter Zugriff");}
if(config.Columns[9]){
 cols << trUtf8("Anhang");}
setHeaderLabels(cols);
}



EntryViewItem::EntryViewItem(QTreeWidget *parent):QTreeWidgetItem(parent){

}

EntryViewItem::EntryViewItem(QTreeWidget *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){

}

EntryViewItem::EntryViewItem(QTreeWidgetItem *parent):QTreeWidgetItem(parent){

}

EntryViewItem::EntryViewItem(QTreeWidgetItem *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){

}
