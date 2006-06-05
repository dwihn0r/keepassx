/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
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
 
#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QMessageBox>
#include "SelectIconDlg.h"



CSelectIconDlg::CSelectIconDlg(Database* database,int CurrentID,QWidget* parent, bool modal, Qt::WFlags fl):QDialog(parent,fl){
setupUi(this);
db=database;
CtxMenu=new QMenu(this);
ReplaceAction=CtxMenu->addAction(*Icon_Swap,tr("Replace..."));
DeleteAction=CtxMenu->addAction(*Icon_EditDelete,tr("Delete"));
connect(Button_AddIcon, SIGNAL(clicked()), this, SLOT(OnAddIcon()));
connect(Button_PickIcon, SIGNAL(clicked()), this, SLOT(OnPickIcon()));
connect(Button_Cancel, SIGNAL(clicked()), this, SLOT(OnCancel()));
connect(DeleteAction,SIGNAL(triggered()),this,SLOT(OnDelete()));
connect(ReplaceAction,SIGNAL(triggered()),this,SLOT(OnReplace()));
connect(List,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(OnSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
updateView();
List->setCurrentItem(List->item(CurrentID));
}

void CSelectIconDlg::updateView(){
List->clear();
for(int i=0; i<db->numIcons(); i++){
	QListWidgetItem* item;
	if(i<BUILTIN_ICONS)
		List->addItem(item=new QListWidgetItem(QIcon(db->icon(i)),QString::number(i)));
	else
		if(!db->icon(i).isNull())
			List->addItem(item=new QListWidgetItem(QIcon(db->icon(i)),"["+QString::number(i)+"]"));
	item->setData(32,i);
}
}


void CSelectIconDlg::OnAddIcon(){
QStringList filenames=QFileDialog::getOpenFileNames(this,tr("Add Icons..."),QDir::homePath(),tr("Images (%1)")
													.arg("*.png *.jpeg *.jpg *.bmp *.gif *.bpm *.pgm *.ppm *.xbm *xpm"));
QString errors;
for(int i=0;i<filenames.size();i++){
	QPixmap icon;
	if(!icon.load(filenames[i])){
		errors+=tr("%1: File could not be loaded.\n").arg(filenames[i].section("/",-1));
		continue;}
	db->addIcon(icon.scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
if(errors.size())
	QMessageBox::warning(this,tr("Error"),tr("An error occured while loading the icon(s):\n%1").arg(errors));
updateView();
List->setCurrentItem(List->item(List->count()-1));
}

void CSelectIconDlg::contextMenuEvent(QContextMenuEvent *event){

QListWidgetItem* item=List->itemAt(List->mapFromParent(event->pos()));

if(!item)return;
if(item->data(32).toInt()<BUILTIN_ICONS){
	DeleteAction->setDisabled(true);
	ReplaceAction->setDisabled(true);}
else{
	DeleteAction->setDisabled(false);
	ReplaceAction->setDisabled(false);}

event->accept();
CtxMenu->popup(event->globalPos());
}

void CSelectIconDlg::OnDelete(){
db->removeIcon(List->currentItem()->data(32).toInt());
updateView();
List->setCurrentItem(List->item(0));
}

void CSelectIconDlg::OnReplace(){
QString filename=QFileDialog::getOpenFileName(this,tr("Add Icons..."),QDir::homePath(),tr("Images (%1)")
													.arg("*.png *.jpeg *.jpg *.bmp *.gif *.bpm *.pgm *.ppm *.xbm *xpm"));
if(filename==QString())return;
QPixmap icon;
if(!icon.load(filename)){
	QMessageBox::warning(this,tr("Error"),tr("An error occured while loading the icon."));
	return;
}
db->replaceIcon(List->currentItem()->data(32).toInt(),icon.scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
List->currentItem()->setIcon(QIcon(db->icon(List->currentItem()->data(32).toInt())));
}

void CSelectIconDlg::OnPickIcon(){
done(List->currentItem()->data(32).toInt());
}

void CSelectIconDlg::OnCancel(){
done(-1);
}


void CSelectIconDlg::OnSelectionChanged(QListWidgetItem* cur,QListWidgetItem* prev){
Button_PickIcon->setEnabled(cur);

}
