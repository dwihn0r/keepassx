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



CSelectIconDlg::CSelectIconDlg(Database* database,QWidget* parent,const char* name, bool modal, Qt::WFlags fl):QDialog(parent,name,modal,fl){
setupUi(this);
db=database;

connect(Button_AddIcon, SIGNAL(clicked()), this, SLOT(OnAddIcon()));

for(int i=0; i<db->numIcons(); i++){
	if(i<BUILTIN_ICONS)
		List->addItem(new QListWidgetItem(QIcon(db->icon(i)),QString::number(i)));
	else
		List->addItem(new QListWidgetItem(QIcon(db->icon(i)),"["+QString::number(i)+"]"));
}
}


void CSelectIconDlg::OnAddIcon(){
QStringList filenames=QFileDialog::getOpenFileNames(this,tr("Add Icons..."),QDir::homePath());
QStringList errors;
for(int i=0;i<filenames.size();i++){
	QPixmap icon;
	if(!icon.load(filenames[i])){
		errors+=tr("%1: File could not be loaded.\n").arg(filenames[i].section("/",-1));
		continue;}
	db->addIcon(icon.scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
if(errors.size())
	QMessageBox::warning(this,tr("Error"),tr("An error occured while loading the icon(s):\n"));
}