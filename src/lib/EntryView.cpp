/***************************************************************************
 *   Copyright (C) 2005-2007 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
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

#include <QHeaderView>
#include <QClipboard>
#include <QFileDialog>
#include "lib/AutoType.h"
#include "lib/EntryView.h"
#include "dialogs/EditEntryDlg.h"

// just for the lessThan funtion
QList<EntryViewItem*>* pItems;
KeepassEntryView* pEntryView;

KeepassEntryView::KeepassEntryView(QWidget* parent):QTreeWidget(parent){
	ViewMode=Normal;
	AutoResizeColumns=true;
	header()->setResizeMode(QHeaderView::Interactive);
	header()->setStretchLastSection(false);
	header()->setClickable(true);
	header()->setCascadingSectionResizes(true);
	ColumnSizes=config->columnSizes();
	Columns=config->columns();
	ColumnOrder=config->columnOrder();

	updateColumns();
	header()->setSortIndicator(config->columnSort(), config->columnSortOrder());

	connect(header(),SIGNAL(sectionResized(int,int,int)),this,SLOT(OnColumnResized(int,int,int)));
	connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(OnItemsChanged()));
	connect(&ClipboardTimer, SIGNAL(timeout()), this, SLOT(OnClipboardTimeOut()));
	connect(header(),SIGNAL(sectionClicked(int)),this,SLOT(OnHeaderSectionClicked(int)));
	connect(header(),SIGNAL(sectionMoved(int,int,int)),this,SLOT(OnColumnMoved(int,int,int)));
	Clipboard=QApplication::clipboard();
	ContextMenu=new QMenu(this);
	setAlternatingRowColors(config->alternatingRowColors());

	pItems=&Items;
	pEntryView=this;
}

KeepassEntryView::~KeepassEntryView(){
	config->setColumnSizes(ColumnSizes);
	config->setColumns(Columns);
	config->setColumnOrder(ColumnOrder);
	config->setColumnSort(header()->sortIndicatorSection());
	config->setColumnSortOrder(header()->sortIndicatorOrder());
}

void KeepassEntryView::OnGroupChanged(IGroupHandle* group){
	CurrentGroup=group;
	showGroup(group);
}

void KeepassEntryView::OnShowSearchResults(){
	CurrentGroup=NULL;
	showSearchResults();
}


void KeepassEntryView::OnItemsChanged(){
	switch(selectedItems().size()){
		case 0:	emit selectionChanged(NONE);
				break;
		case 1:	emit selectionChanged(SINGLE);
				break;
		default:emit selectionChanged(MULTIPLE);
	}
}

bool sortSearchResultsLessThan(const IEntryHandle* a, const IEntryHandle* b){
	int indexA, indexB;
	for(indexA=0;indexA<pItems->size();indexA++){
		if((*pItems)[indexA]->EntryHandle==a)break;
	}
	for(indexB=0;indexB<pItems->size();indexB++){
		if((*pItems)[indexB]->EntryHandle==b)break;
	}
	return pEntryView->indexOfTopLevelItem((*pItems)[indexA])<pEntryView->indexOfTopLevelItem((*pItems)[indexB]);
}


void KeepassEntryView::OnHeaderSectionClicked(int index){
	if(header()->sortIndicatorSection()==index){
		header()->setSortIndicator(index,header()->sortIndicatorOrder() ? Qt::DescendingOrder : Qt::AscendingOrder);
	}
	else{
		header()->setSortIndicator(index,Qt::AscendingOrder);
	}

	if(ViewMode==Normal){
		for(int i=0;i<Items.size();i++){
			Items[i]->EntryHandle->setVisualIndexDirectly(indexOfTopLevelItem(Items[i]));
		}
	}
	else if(ViewMode==ShowSearchResults){
		if(header()->sortIndicatorOrder()==Qt::AscendingOrder)
			qSort(SearchResults.begin(),SearchResults.end(),sortSearchResultsLessThan);
		else
			qSort(SearchResults.end(),SearchResults.begin(),sortSearchResultsLessThan);

	}
}

void KeepassEntryView::OnSaveAttachment(){
	if (selectedItems().size() == 0) return;
	CEditEntryDlg::saveAttachment(((EntryViewItem*)selectedItems().first())->EntryHandle,this);
}

void KeepassEntryView::OnCloneEntry(){
	QList<QTreeWidgetItem*> entries=selectedItems();
	for(int i=0; i<entries.size();i++){
		Items.append(new EntryViewItem(this));
		Items.back()->EntryHandle=
			db->cloneEntry(((EntryViewItem*)entries[i])->EntryHandle);
		updateEntry(Items.back());
	}
	emit fileModified();
}

void KeepassEntryView::OnDeleteEntry(){
	QList<QTreeWidgetItem*> entries=selectedItems();
	
	if(config->askBeforeDelete()){
		QString text;
		if(entries.size()==1)
			text=tr("Are you sure you want to delete this entry?");
		else
			text=tr("Are you sure you want to delete these %1 entries?").arg(entries.size());
		if(QMessageBox::question(this,tr("Delete?"),text,QMessageBox::Yes | QMessageBox::No,QMessageBox::No)==QMessageBox::No)
			return;			
	}
	
	for(int i=0; i<entries.size();i++){
		db->deleteEntry(((EntryViewItem*)entries[i])->EntryHandle);
		Items.removeAt(Items.indexOf((EntryViewItem*)entries[i]));
		delete entries[i];
	}
	emit fileModified();
}



void KeepassEntryView::updateEntry(EntryViewItem* item){
	IEntryHandle* entry = item->EntryHandle;
	int j=0;
	if(Columns.at(0)){
		item->setText(j++,entry->title());
		item->setIcon(0,db->icon(entry->image()));
	}
	if (Columns.at(1)){
		if(config->hideUsernames())
			item->setText(j++,"******");
		else
			item->setText(j++,entry->username());}
		if (Columns.at(2)){item->setText(j++,entry->url());}
		if (Columns.at(3)){
			if(config->hidePasswords())
				item->setText(j++,"******");
			else{
				SecString password=entry->password();
				password.unlock();
				item->setText(j++,password.string());
			}
		}
	if (Columns.at(4)){
		item->setText(j++,entry->comment().section('\n',0,0));}
	if (Columns.at(5)){
		item->setText(j++,entry->expire().dateToString(Qt::LocalDate));}
	if (Columns.at(6)){
		item->setText(j++,entry->creation().dateToString(Qt::LocalDate));}
	if (Columns.at(7)){
		item->setText(j++,entry->lastMod().dateToString(Qt::LocalDate));}
	if (Columns.at(8)){
		item->setText(j++,entry->lastAccess().dateToString(Qt::LocalDate));}
	if (Columns.at(9)){
		item->setText(j++,entry->binaryDesc());}
	if(Columns.at(10) && ViewMode==ShowSearchResults){
		item->setText(j,entry->group()->title());
		item->setIcon(j++,db->icon(entry->group()->image()));}
}

void KeepassEntryView::editEntry(EntryViewItem* item){
	CEditEntryDlg dlg(db,item->EntryHandle,this,true);
	switch(dlg.exec()){
		case 0: //canceled or no changes
			break;
		case 1: //modifications but same group
			updateEntry(item);
			emit fileModified();
			break;
		case 2: //entry moved to another group
			delete item;
			Items.removeAt(Items.indexOf(item));
			emit fileModified();
			break;
	}

}


void KeepassEntryView::OnNewEntry(){
	IEntryHandle* NewEntry = NULL;
	if (!CurrentGroup){// We must be viewing search results. Add the new entry to the first group.		
		if (db->groups().size() > 0)
			NewEntry=db->newEntry(db->sortedGroups()[0]);
		else{
			QMessageBox::critical(NULL,tr("Error"),tr("At least one group must exist before adding an entry."),tr("OK"));
		}
	}
	else
		NewEntry=db->newEntry(CurrentGroup);
	CEditEntryDlg dlg(db,NewEntry,this,true,true);
	if(!dlg.exec()){
		db->deleteLastEntry();
	}
	else{
		Items.append(new EntryViewItem(this));
		Items.back()->EntryHandle=NewEntry;
		updateEntry(Items.back());
		emit fileModified();
	}

}

void KeepassEntryView::OnEntryActivated(QTreeWidgetItem* item,int Column){
	switch(columnListIndex(Column)){
		case 0:	editEntry((EntryViewItem*)item);
		break;
		case 1: OnUsernameToClipboard();
		break;
		case 2: OnEditOpenUrl();
		break;
		case 3:	OnPasswordToClipboard();
		break;
	}

}

void KeepassEntryView::OnEditEntry(){
	if (selectedItems().size() == 0) return;
	editEntry((EntryViewItem*)selectedItems().first());
}

void KeepassEntryView::OnEditOpenUrl(){
	if (selectedItems().size() == 0) return;
	openBrowser( ((EntryViewItem*)selectedItems().first())->EntryHandle );
}

void KeepassEntryView::OnUsernameToClipboard(){
	if (selectedItems().size() == 0) return;
	Clipboard->setText(((EntryViewItem*)selectedItems().first())->EntryHandle->username(),  QClipboard::Clipboard);
	if(Clipboard->supportsSelection()){
		Clipboard->setText(((EntryViewItem*)selectedItems().first())->EntryHandle->username(),QClipboard::Selection);
	}
	ClipboardTimer.setSingleShot(true);
	ClipboardTimer.start(config->clipboardTimeOut()*1000);
}

void KeepassEntryView::OnPasswordToClipboard(){
	if (selectedItems().size() == 0) return;
	SecString password;
	password=((EntryViewItem*)selectedItems().first())->EntryHandle->password();
	password.unlock();
	Clipboard->setText(password.string(),QClipboard::Clipboard);
	if(Clipboard->supportsSelection()){
		Clipboard->setText(password.string(),QClipboard::Selection);
	}
	ClipboardTimer.setSingleShot(true);
	ClipboardTimer.start(config->clipboardTimeOut()*1000);
}

void KeepassEntryView::OnClipboardTimeOut(){
	Clipboard->clear(QClipboard::Clipboard);
	if(Clipboard->supportsSelection()){
		Clipboard->clear(QClipboard::Selection);
	}
}


void KeepassEntryView::contextMenuEvent(QContextMenuEvent* e){
	if(itemAt(e->pos())){
		EntryViewItem* item=(EntryViewItem*)itemAt(e->pos());
		if(!selectedItems().size()){
			setItemSelected(item,true);
		}
		else{
				if(!isItemSelected(item)){
					while(selectedItems().size()){
						setItemSelected(selectedItems().first(),false);
					}
					setItemSelected(item,true);
				}
			}
	}
	else
	{while(selectedItems().size()){
		setItemSelected(selectedItems().first(),false);}
	}
	e->accept();
	ContextMenu->popup(e->globalPos());
}

void KeepassEntryView::resizeEvent(QResizeEvent* e){
	resizeColumns();
	e->accept();
}


void KeepassEntryView::showSearchResults(){
	if(ViewMode==Normal){
		ViewMode=ShowSearchResults;
		if(Columns.at(10))ColumnOrder[10]--;
		updateColumns();
	}
	clear();
	Items.clear();
	createItems(SearchResults);
}


void KeepassEntryView::showGroup(IGroupHandle* group){
	if(ViewMode==ShowSearchResults){
		ViewMode=Normal;
		updateColumns();
	}
	clear();
	Items.clear();
	if(group==NULL)return;
	QList<IEntryHandle*>entries=db->entries(group);
	createItems(entries);
}

void KeepassEntryView::createItems(QList<IEntryHandle*>& entries){
	for(int i=0;i<entries.size();i++){
		if(!entries[i]->isValid())continue;
		EntryViewItem* item=new EntryViewItem(this);
		Items.push_back(item);
		Items.back()->EntryHandle=entries[i];
		int j=0;
		if (Columns.at(0)){
			item->setText(j++,entries[i]->title());
			item->setIcon(0,db->icon(entries[i]->image()));}
		if (Columns.at(1)){
			if(config->hideUsernames())
				item->setText(j++,"******");
			else
				item->setText(j++,entries[i]->username());}
		if (Columns.at(2)){item->setText(j++,entries[i]->url());}
		if (Columns.at(3)){
			if(config->hidePasswords())
				item->setText(j++,"******");
			else{
				SecString password=entries[i]->password();
				password.unlock();
				item->setText(j++,password.string());
			}
		}
		if (Columns.at(4)){
			item->setText(j++,entries[i]->comment().section('\n',0,0));}
		if (Columns.at(5)){
			item->setText(j++,entries[i]->expire().dateToString(Qt::LocalDate));}
		if (Columns.at(6)){
			item->setText(j++,entries[i]->creation().dateToString(Qt::LocalDate));}
		if (Columns.at(7)){
			item->setText(j++,entries[i]->lastMod().dateToString(Qt::LocalDate));}
		if (Columns.at(8)){
			item->setText(j++,entries[i]->lastAccess().dateToString(Qt::LocalDate));}
		if (Columns.at(9)){
			item->setText(j++,entries[i]->binaryDesc());}
		if(Columns.at(10) && ViewMode==ShowSearchResults){
			item->setText(j,entries[i]->group()->title());
			item->setIcon(j++,db->icon(entries[i]->group()->image()));}
	}
}

void KeepassEntryView::updateIcons(){
	for(int i=0;i<Items.size();i++){
		Items[i]->setIcon(0,db->icon(Items[i]->EntryHandle->image()));
	}
}


void KeepassEntryView::setEntry(IEntryHandle* entry){

}

void KeepassEntryView::updateColumns(){
	setColumnCount(0);
	QStringList cols;
	if (Columns.at(0)){
		cols << tr("Title");}
	if (Columns.at(1)){
		cols << tr("Username");}
	if (Columns.at(2)){
		cols << tr("URL");}
	if (Columns.at(3)){
		cols << tr("Password");}
	if (Columns.at(4)){
		cols << tr("Comments");}
	if (Columns.at(5)){
		cols << tr("Expires");}
	if (Columns.at(6)){
		cols << tr("Creation");}
	if (Columns.at(7)){
		cols << tr("Last Change");}
	if (Columns.at(8)){
		cols << tr("Last Access");}
	if (Columns.at(9)){
		cols << tr("Attachment");}
	if(Columns.at(10) && ViewMode==ShowSearchResults){
		cols << tr("Group");}
	setHeaderLabels(cols);

	for(int i=0;i<NUM_COLUMNS;i++){
		if(!Columns.at(i))
			ColumnOrder[i]=100;
	}

	QMap<int,int> Order;
	for(int i=NUM_COLUMNS-1;i>=0;i--)
		Order.insertMulti(ColumnOrder.at(i),i);

	QMapIterator<int, int> i(Order);
 	while (i.hasNext()) {
		i.next();
		int index=i.value();
		if(!Columns.at(index))continue;
		header()->moveSection(header()->visualIndex(logicalColIndex(index)),header()->count()-1);
	}

	resizeColumns();
}

void KeepassEntryView::refreshItems(){
	for (int i=0;i<Items.size();i++)
		updateEntry(Items.at(i));
}

void KeepassEntryView::OnColumnMoved(int LogIndex,int OldVisIndex,int NewVisIndex){
	for(int i=0;i<header()->count();i++){
		ColumnOrder[columnListIndex(header()->logicalIndex(i))]=i;
	}
}

int KeepassEntryView::logicalColIndex(int LstIndex){
	int c=-1;
	for(int i=0;i<NUM_COLUMNS;i++){
		if(Columns.at(i))c++;
		if(i==10 && Columns.at(10) && ViewMode!=ShowSearchResults)c--;
		if(i==LstIndex)return c;
	}
	Q_ASSERT(false);
	return -1;
}

void KeepassEntryView::resizeColumns(){
	AutoResizeColumns=false;
	int w=viewport()->width();
	int sum=0;

	for(int i=0;i<NUM_COLUMNS;i++){
	//	if(i==10) continue; //skip "Group" column
		if(!Columns.at(i))
			ColumnSizes[i]=0;
		if(Columns.at(i) && !ColumnSizes.at(i))
			ColumnSizes[i]=w/10;
	}

	for(int i=0;i<header()->count();i++){
		sum+=ColumnSizes.at(columnListIndex(i));
	}
	float stretch=((float)w)/((float)sum);
	sum=0;
	for(int i=0;i<header()->count();i++){
		int lstIndex=columnListIndex(header()->logicalIndex(i));
		int NewSize=qRound(stretch*(float)ColumnSizes.at(lstIndex));
		sum+=NewSize;
		if(i==header()->count()-1){
			NewSize+=(w-sum);	// add rounding difference to the last column
		}
		header()->resizeSection(header()->logicalIndex(i),NewSize);
		ColumnSizes[lstIndex]=NewSize;
	}
	AutoResizeColumns=true;
}

int KeepassEntryView::columnListIndex(int LogicalIndex){
	int c=-1; int i;
	for(i=0;i<NUM_COLUMNS;i++){
		if(Columns.at(i))c++;
		if(i==10 && Columns.at(10) && ViewMode!=ShowSearchResults)c--;
		if(c==LogicalIndex)break;
	}
	return i;
}


void KeepassEntryView::OnColumnResized(int lindex,int Old, int New){
	if(!AutoResizeColumns)return;
	for(int i=0;i<header()->count();i++){
		ColumnSizes[columnListIndex(i)]=header()->sectionSize(i);
	}
	resizeColumns();
}

void KeepassEntryView::mousePressEvent(QMouseEvent *event){
	//save event position - maybe this is the start of a drag
	if (event->button() == Qt::LeftButton)
		DragStartPos = event->pos();
	QTreeWidget::mousePressEvent(event);
}

void KeepassEntryView::mouseMoveEvent(QMouseEvent *event){
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - DragStartPos).manhattanLength() < QApplication::startDragDistance())
		return;

	DragItems.clear();
	EntryViewItem* DragStartItem=(EntryViewItem*)itemAt(DragStartPos);
	if(!DragStartItem){
		while(selectedItems().size()){
			setItemSelected(selectedItems().first(),false);}
		return;
	}
	if(selectedItems().isEmpty()){
			setItemSelected(DragStartItem,true);}
	else{
		bool AlreadySelected=false;
		for(int i=0;i<selectedItems().size();i++){
			if(selectedItems()[i]==DragStartItem){AlreadySelected=true; break;}
		}
		if(!AlreadySelected){
			while(selectedItems().size()){
				setItemSelected(selectedItems().first(),false);
			}
			setItemSelected(DragStartItem,true);
		}
	}

	DragItems=selectedItems();
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	void* pDragItems=&DragItems;
	mimeData->setData("text/plain;charset=UTF-8",DragItems.first()->text(0).toUtf8());
	mimeData->setData("application/x-keepassx-entry",QByteArray((char*)&pDragItems,sizeof(void*)));
	drag->setMimeData(mimeData);
	EventOccurredBlock = true;
	drag->exec(Qt::MoveAction);
	EventOccurredBlock = false;
}

void KeepassEntryView::removeDragItems(){
	for(int i=0;i<DragItems.size();i++){
		for(int j=0;j<Items.size();j++){
			if(Items[j]==DragItems[i]){
				Items.removeAt(j);
				j--;
				delete DragItems[i];
			}
		}
	}
}

#ifdef AUTOTYPE
void KeepassEntryView::OnAutoType(){
	if (selectedItems().size() == 0) return;
	QString error;
	AutoType::perform(((EntryViewItem*)selectedItems().first())->EntryHandle,error);
}
#endif

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


bool EntryViewItem::operator<(const QTreeWidgetItem& other)const{
	int SortCol=treeWidget()->header()->sortIndicatorSection();
	int ListIndex=((KeepassEntryView*)treeWidget())->columnListIndex(SortCol);
	if(ListIndex < 5 || ListIndex==9 || ListIndex==10){ //columns with string values (Title, Username, Password, URL, Comment, Group)
		if(QString::localeAwareCompare(text(SortCol),other.text(SortCol)) < 0)
			return true;
		else
			return false;
	}
	KpxDateTime DateThis;
	KpxDateTime DateOther;


	switch(SortCol){
		case 5: DateThis=EntryHandle->expire();
				DateOther=((EntryViewItem&)other).EntryHandle->expire();
				break;
		case 6: DateThis=EntryHandle->creation();
				DateOther=((EntryViewItem&)other).EntryHandle->creation();
				break;
		case 7: DateThis=EntryHandle->lastMod();
				DateOther=((EntryViewItem&)other).EntryHandle->lastMod();
				break;
		case 8: DateThis=EntryHandle->lastAccess();
				DateOther=((EntryViewItem&)other).EntryHandle->lastAccess();
				break;
		default:Q_ASSERT(false);
	}
	return DateThis < DateOther;
}


void KeepassEntryView::setCurrentEntry(IEntryHandle* entry){
	bool found=false;
	int i;
	for(i=0;i<Items.size();i++)
		if(Items.at(i)->EntryHandle==entry){found=true; break;}
	if(!found)return;
	setCurrentItem(Items.at(i));
}
