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


#include <QLineEdit>
#include <QCheckBox>
#include <QRegExp>
#include <QMessageBox>
#include <QPainter>
#include "main.h"
#include "PwmConfig.h"
#include "SearchDlg.h"


SearchDialog::SearchDialog(IDatabase* database,IGroupHandle* Group,QWidget* parent):QDialog(parent)
{
	setupUi(this);
	connect( Button_Search, SIGNAL( clicked() ), this, SLOT( OnSearch() ) );
	connect( Button_Close, SIGNAL( clicked() ), this, SLOT( OnClose() ) );
	db=database;
	group=Group;
	createBanner(&BannerPixmap,getPixmap("search"),tr("Search"),width());
	checkBox_Cs->setChecked(config.SearchOptions[0]);
	checkBox_regExp->setChecked(config.SearchOptions[1]);
	checkBox_Title->setChecked(config.SearchOptions[2]);
	checkBox_Username->setChecked(config.SearchOptions[3]);
	checkBox_Password->setChecked(config.SearchOptions[4]);
	checkBox_Comment->setChecked(config.SearchOptions[5]);
	checkBox_URL->setChecked(config.SearchOptions[6]);
	checkBox_Attachment->setChecked(config.SearchOptions[7]);
	if(group)
		checkBox_Recursive->setChecked(config.SearchOptions[8]);
	else{
		checkBox_Recursive->setChecked(false);
		checkBox_Recursive->setEnabled(false);
	}
}

SearchDialog::~SearchDialog()
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

void SearchDialog::OnClose()
{
	done(0);
}

void SearchDialog::OnSearch()
{
	bool Fields[6];
	Fields[0]=checkBox_Title->isChecked();
	Fields[1]=checkBox_Username->isChecked();
	Fields[2]=checkBox_URL->isChecked();
	Fields[3]=checkBox_Password->isChecked();
	Fields[4]=checkBox_Comment->isChecked();
	Fields[5]=checkBox_Attachment->isChecked();	
	Result=db->search(group,Edit_Search->text(),checkBox_Cs->isChecked(),checkBox_regExp->isChecked(),checkBox_Recursive->isChecked(),Fields);
	done(1);
}

void SearchDialog::paintEvent(QPaintEvent *event){
	QDialog::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}



