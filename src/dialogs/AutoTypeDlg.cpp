/***************************************************************************
 *   Copyright (C) 2005-2008 by Felix Geyer                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *

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

#include "AutoTypeDlg.h"
#include "KpxConfig.h"
#include <QDesktopWidget>
#include <QPainter>
#include <QPaintEvent>

AutoTypeDlg::AutoTypeDlg(QList<IEntryHandle*> entries, QList<int> numbers){
	setupUi(this);
	createBanner(&BannerPixmap,getPixmap("keepassx_large"),tr("Auto-Type"),width());
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
	setGeometry( QRect(QApplication::desktop()->screenGeometry(QCursor::pos()).center() - rect().center(), size()) );
	setWindowIcon(getIcon("keepassx"));
	entryList->setAlternatingRowColors(config->alternatingRowColors());
	
	QList<QTreeWidgetItem*> itemList;
	AutoTypeEntry autoTypeEntry;
	for (int i=0; i<entries.size(); i++){
		QStringList cols;
		cols << entries[i]->group()->title() << entries[i]->title();
		if (config->hideUsernames())
			cols << "****";
		else
			cols << entries[i]->username();
		
		QTreeWidgetItem* widgetItem = new QTreeWidgetItem(cols);
		itemList.append(widgetItem);
		autoTypeEntry.dbHandle = entries[i];
		autoTypeEntry.nr = numbers[i];
		itemToEntry.insert(widgetItem, autoTypeEntry);
	}
	
	qSort(itemList.begin(), itemList.end(), itemLessThan);
	for (int i=0; i<itemList.size(); i++)
		entryList->addTopLevelItem(itemList[i]);
	
	entryList->resizeColumnToContents(0);
	entryList->resizeColumnToContents(1);
	entryList->resizeColumnToContents(2);
	
	connect(ButtonBox, SIGNAL(rejected()), SLOT(close()));
	connect(entryList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(itemSelected(QTreeWidgetItem*)));
}

bool AutoTypeDlg::itemLessThan(QTreeWidgetItem* i1, QTreeWidgetItem* i2){
	if (i1->text(0) != i2->text(0))
		return i1->text(0) < i2->text(0);
	else if (i1->text(1) != i2->text(1))
		return i1->text(1) < i2->text(1);
	else
		return i1->text(2) < i2->text(2);
}

void AutoTypeDlg::paintEvent(QPaintEvent* event){
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(QPoint(0,0),BannerPixmap);
}

void AutoTypeDlg::itemSelected(QTreeWidgetItem* item){
	close();
	QString err;
	AutoType::perform(itemToEntry[item].dbHandle,err,false,itemToEntry[item].nr);
}
