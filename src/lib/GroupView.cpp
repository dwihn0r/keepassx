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
#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include "main.h"
#include "GroupView.h"

KeepassGroupView::KeepassGroupView(QWidget* parent):QTreeWidget(parent){
LastHoverItem=NULL;
setHeaderLabels(QStringList()<<tr("Gruppen"));
}

void KeepassGroupView:: dragEnterEvent ( QDragEnterEvent * event ){
if(event->mimeData()->hasFormat("keepass/group") || 
   event->mimeData()->hasFormat("keepass/entry")){
event->accept();
}
}

void KeepassGroupView:: dragMoveEvent ( QDragMoveEvent * event ){
GroupViewItem* item=(GroupViewItem*)itemAt(event->pos());
if(LastHoverItem){
  QFont f=LastHoverItem->font(0);
  f.setBold(false);
  LastHoverItem->setFont(0,f);
}

if(item){
 if(!db->isParentGroup(item->pGroup,DragItem->pGroup) && DragItem!=item){
   QFont f=item->font(0);
   f.setBold(true);
   item->setFont(0,f);
   LastHoverItem=item;
   event->setAccepted(true);}
 else 
   event->setAccepted(false);
 }
else{
  LastHoverItem=NULL;}


}

void KeepassGroupView:: dragLeaveEvent ( QDragLeaveEvent * event ){
if(LastHoverItem){
  QFont f=LastHoverItem->font(0);
  f.setBold(false);
  LastHoverItem->setFont(0,f);
}
}


void KeepassGroupView::dropEvent( QDropEvent * event ){
if(LastHoverItem){
  QFont f=LastHoverItem->font(0);
  f.setBold(false);
  LastHoverItem->setFont(0,f);
}

}


void KeepassGroupView::mousePressEvent(QMouseEvent *event){
//save event position - maybe this is the start of a drag
if (event->button() == Qt::LeftButton)
            DragStartPos = event->pos();
//call base function
QTreeWidget::mousePressEvent(event);
}

void KeepassGroupView::mouseMoveEvent(QMouseEvent *event){
 if (!(event->buttons() & Qt::LeftButton))
            return;
 if ((event->pos() - DragStartPos).manhattanLength() < QApplication::startDragDistance())
            return;
	DragItem=(GroupViewItem*)itemAt(DragStartPos);
	if(!DragItem)return;
	QDrag *drag = new QDrag(this);
	QFontMetrics fontmet(DragItem->font(0));
	int DragPixmHeight=16;
	if(fontmet.height()>16)DragPixmHeight=fontmet.height();
	DragPixmap  = QPixmap(fontmet.width(DragItem->text(0))+19,DragPixmHeight);
	DragPixmap.fill(QColor(255,255,255));
	QPainter painter(&DragPixmap);
	painter.setPen(QColor(0,0,0));
	painter.setFont(DragItem->font(0));
	painter.drawPixmap(0,0,DragItem->icon(0).pixmap());
	painter.drawText(19,DragPixmHeight-fontmet.strikeOutPos(),DragItem->text(0));	
        QMimeData *mimeData = new QMimeData;
	mimeData->setData("keepass/group",QByteArray((char*)&(DragItem->pGroup),sizeof(void*)));
        drag->setMimeData(mimeData);
        drag->setPixmap(DragPixmap);
	drag->start();


}

void KeepassGroupView::updateItems(){
clear();
Items.clear();
for(GroupItr i=db->Groups.begin();i!=db->Groups.end();i++){
if((*i).Level==0){
 if(Items.size()) Items.push_back(new GroupViewItem(this,getLastSameLevelItem(0)));
 else 		  Items.push_back(new GroupViewItem(this));
 Items.back()->setText(0,(*i).Name);
 Items.back()->pGroup=&(*i); 
}
 else{
 if((*i).Level>(*(i-1)).Level){
 Items.push_back(new GroupViewItem(Items.back(),getLastSameLevelItem((*i).Level)));
 Items.back()->setText(0,(*i).Name);
 Items.back()->pGroup=&(*i);
 }
 if((*i).Level<=(*(i-1)).Level){
   GroupItemItr j;
   for(j=Items.end()-1;j!=Items.begin();j--){
    if((*j)->pGroup->Level<(*i).Level)break;}
     Items.push_back(new GroupViewItem((*j),getLastSameLevelItem((*i).Level)));
     Items.back()->setText(0,(*i).Name);
     Items.back()->pGroup=&(*i);
     }
 }
Items.back()->setIcon(0,EntryIcons[(*i).ImageID]);
}

for(int i=0;i<Items.size();i++){
 setItemExpanded(Items[i],Items[i]->pGroup->UI_ItemIsExpanded);
}}

GroupViewItem* KeepassGroupView::getLastSameLevelItem(int level){
for(int i=Items.size()-1;i>=0;i--){
 if(Items[i]->pGroup->Level==level){
return Items[i];}
}
return Items.back();

}



GroupViewItem::GroupViewItem(QTreeWidget *parent):QTreeWidgetItem(parent){
}

GroupViewItem::GroupViewItem(QTreeWidget *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){
}

GroupViewItem::GroupViewItem(QTreeWidgetItem *parent):QTreeWidgetItem(parent){
}

GroupViewItem::GroupViewItem(QTreeWidgetItem *parent, QTreeWidgetItem *preceding):QTreeWidgetItem(parent,preceding){
}
