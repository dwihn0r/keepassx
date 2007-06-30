/***************************************************************************
 *   Copyright (C) 2007 by Tarek Saidi                                     *
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
  
#include <QTreeWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include "main.h"
#include "TrashCanDlg.h"

TrashCanDialog::TrashCanDialog(QWidget* parent,IDatabase* database,const QList<IEntryHandle*>& TrashItems):QDialog(parent){
	setupUi(this);
	Entries=TrashItems;
	for(int i=0;i<Entries.size();i++){
		QTreeWidgetItem* item=new QTreeWidgetItem(treeWidget);
		item->setData(0,Qt::UserRole,i);
		item->setText(0,Entries[i]->group()->title());
		item->setText(1,Entries[i]->title());
		item->setText(2,Entries[i]->username());
		item->setText(3,Entries[i]->expire().dateToString(Qt::LocalDate));
		item->setIcon(0,database->icon(Entries[i]->group()->image()));
		item->setIcon(1,database->icon(Entries[i]->image()));
			
	}
	connect(treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(OnItemDoubleClicked(QTreeWidgetItem*,int)));
	connect(treeWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(OnContextMenu(const QPoint&)));
	ContextMenu=new QMenu(this);
	ContextMenu->addAction(getIcon("restore"),"Restore");
	ContextMenu->addAction(getIcon("deleteentry"),"Delete");	
}


void TrashCanDialog::paintEvent(QPaintEvent* event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void TrashCanDialog::resizeEvent(QResizeEvent* event){
	createBanner(&BannerPixmap,getPixmap("trashcan"),tr("Recycle Bin"),width());
	QDialog::resizeEvent(event);	
}

void TrashCanDialog::OnItemDoubleClicked(QTreeWidgetItem* item, int column){
	SelectedEntry=Entries[item->data(0,Qt::UserRole).toInt()];	
	accept();
}

void TrashCanDialog::OnContextMenu(const QPoint& pos){
	if(treeWidget->itemAt(pos)){
		QTreeWidgetItem* item=treeWidget->itemAt(pos);
		if(treeWidget->selectedItems().size()==0){
			treeWidget->setItemSelected(item,true);
		}
		else{
				if(!treeWidget->isItemSelected(item)){
					while(treeWidget->selectedItems().size()){
						treeWidget->setItemSelected(treeWidget->selectedItems()[0],false);
					}
					treeWidget->setItemSelected(item,true);
				}
			}
	}
	else
	{
		while(treeWidget->selectedItems().size())
			treeWidget->setItemSelected(treeWidget->selectedItems()[0],false);
	}
	ContextMenu->popup(treeWidget->viewport()->mapToGlobal(pos));
}



///TODO 0.2.3 locale aware string/date compare for correct sorting
