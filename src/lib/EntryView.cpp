/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                 *
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

#include <math.h>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QTime>
#include "main.h"
#include "EntryView.h"



KeepassEntryView::KeepassEntryView(QWidget* parent):QTreeWidget(parent){
AutoResizeColumns=true;
int sum=0;
for(int i=0;i<NUM_COLUMNS;i++)
	sum+=config.ColumnSizes[i];
for(int i=0;i<NUM_COLUMNS;i++)
	ColumnSizes << (float)config.ColumnSizes[i]/(float)sum;

CurrentGroup=0;
updateColumns();
header()->setResizeMode(QHeaderView::Interactive);
header()->setStretchLastSection(false);
connect(header(),SIGNAL(sectionResized(int,int,int)),this,SLOT(OnColumnResized(int,int,int)));
ContextMenu=new QMenu(this);

}

KeepassEntryView::~KeepassEntryView(){
for(int i=0;i<ColumnSizes.size();i++){
	config.ColumnSizes[i]=round(ColumnSizes[i]*10000.0f);
}
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

void KeepassEntryView::resizeEvent(QResizeEvent* e){
resizeColumns();
e->accept();
}


void KeepassEntryView::updateItems(unsigned int GroupID){

clear();
Items.clear();
if(!db)return;
if(!GroupID)return;
for(int i=0;i<db->Entries.size();i++){
  if(db->Entries[i].GroupID==GroupID)
  	setEntry(&db->Entries[i]);
}

}

void KeepassEntryView::showSearchResults(QList<Q_UINT32>& results){
clear();
Items.clear();
for(int j=0; j<results.size(); j++){
	for(int i=0; i<db->Entries.size();i++){
		if(db->Entries[i].sID == results[j])
 		setEntry(&db->Entries[i]);
	}
}
}

void KeepassEntryView::setEntry(CEntry* entry){
  EntryViewItem* tmp=NULL;
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
	  entry->Password.unlock();
      tmp->setText(j++,entry->Password.string());
      entry->Password.lock();}}
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

void KeepassEntryView::refreshItems(){
EntryViewItem *tmp=NULL;
for(int i=0;i<Items.size();i++){
  tmp=Items[i];
  CEntry* entry=tmp->pEntry;

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
      entry->Password.unlock();
      tmp->setText(j++,entry->Password.string());
      entry->Password.lock();}}
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


void KeepassEntryView::updateColumns(){
setColumnCount(0);
QStringList cols;
if(config.Columns[0]){
 cols << tr("Title");}
if(config.Columns[1]){
 cols << tr("Username");}
if(config.Columns[2]){
 cols << tr("URL");}
if(config.Columns[3]){
 cols << tr("Password");}
if(config.Columns[4]){
 cols << tr("Comments");}
if(config.Columns[5]){
 cols << tr("Expires");}
if(config.Columns[6]){
 cols << tr("Creation");}
if(config.Columns[7]){
 cols << tr("Last Change");}
if(config.Columns[8]){
 cols << tr("Last Access");}
if(config.Columns[9]){
 cols << tr("Attachment");}
setHeaderLabels(cols);
resizeColumns();
}

void KeepassEntryView::resizeColumns(){

AutoResizeColumns=false;

for(int i=0;i<NUM_COLUMNS;i++)
	if(!config.Columns[i])ColumnSizes[i]=0;

for(int i=0;i<NUM_COLUMNS;i++)
	if(config.Columns[i] && ColumnSizes[i]==0)ColumnSizes[i]=0.1f;

float sum=0;
for(int i=0;i<NUM_COLUMNS;i++)
	sum+=ColumnSizes[i];

for(int i=0;i<NUM_COLUMNS;i++)
	ColumnSizes[i]/=sum;

int w=viewport()->width();
int wx=0; int j=0;


for(int i=0;i<NUM_COLUMNS;i++){
	if(!config.Columns[i])continue;
	int NewWidth=round(ColumnSizes[i]*(float)w);
	wx+=NewWidth;
	header()->resizeSection(j++,NewWidth);
	//add rounding difference (w-wx) to the last column
	if(j==header()->count()){
		header()->resizeSection(j-1,header()->sectionSize(j-1)+(w-wx));
	}
}

AutoResizeColumns=true;

}


void KeepassEntryView::OnColumnResized(int index,int Old, int New){
if(!AutoResizeColumns)return;

int i=0; int c=-1;
for(i;i<ColumnSizes.size();i++){
	if(config.Columns[i])c++;
	if(c==index)break;
}

int j=0; c=-1; bool IsLastColumn=true;
for(j;j<ColumnSizes.size();j++){
	if(config.Columns[j])c++;
	if(c==(index+1)){IsLastColumn=false; break;}
}

if(IsLastColumn){
	j=0; c=-1;
	for(j;j<ColumnSizes.size();j++){
		if(config.Columns[j])c++;
		if(c==(index-1))break;
	}
}

int w=viewport()->width();
float div=(float)(New-Old)/(float)w;

if(((ColumnSizes[j]-div)*w > 2)){
	ColumnSizes[j]-=div;
	ColumnSizes[i]+=div;
}
resizeColumns();
}



void KeepassEntryView::paintEvent(QPaintEvent * event){
QTreeWidget::paintEvent(event);
}

EntryViewItem::EntryViewItem(QTreeWidget *parent):QTreeWidgetItem(parent){

}

EntryViewItem::EntryViewItem(QTreeWidget *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){

}

EntryViewItem::EntryViewItem(QTreeWidgetItem *parent):QTreeWidgetItem(parent){

}

EntryViewItem::EntryViewItem(QTreeWidgetItem *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){

}
