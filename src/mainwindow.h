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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qfont.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>
#include "ui_MainWindow.h"
#include "ListViews.h"
#include "PwmConfig.h"

typedef vector<GroupItem*>::iterator GroupItemItr;
typedef vector<EntryItem*>::iterator EntryItemItr;

class CMainWindow : public MainFrame{
Q_OBJECT

public:
  /*$PUBLIC_FUNCTIONS$*/
  CMainWindow(QApplication* app,QString ArgFile,QString ArgCfg, QWidget* parent = 0,const char* name = 0, WFlags fl = 0 );
  ~CMainWindow();
protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void showEvent(QShowEvent *e);

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void OnClose();
    virtual void OnMainWinResize();
    virtual void OnFileOpen();
    virtual void OnGroupChanged(QListViewItem*);
    virtual void OnEntryChanged(QListViewItem*);
    virtual void OnEntryDoubleClicked(QListViewItem*,const QPoint&,int);
    virtual void OnMenu_closeDB();
    virtual void OnFileSaveAs();
    virtual void OnFileSave();
    virtual void OnMenuExit();
    virtual void OnAddGroup();
    virtual void OnUserNameToClipboard();
    virtual void OnPasswordToClipboard();
    virtual void OnDeleteGroup();
    virtual void OnEditGroup();
    virtual void OnAddSubGroup();
    virtual void OnClipboardTimerEvent();
    virtual void OnOpenURL();
    virtual void OnGroupSearch();
    virtual void OnGlobalSearch();
    virtual void OnDeleteEntry();
    virtual void OnCopyEntry();
    virtual void OnEditEntry();
    virtual void OnAddEntry();
    virtual void OnSaveAttachment();
    virtual void OnEntryRightClicked(QListViewItem* item, const QPoint& pos,int column);
    virtual void OnEntryCtxMenuClicked(int id);
    virtual void OnGroupRightClicked(QListViewItem* item, const QPoint& pos, int column);
    virtual void OnGroupCtxMenuClicked(int id);
    virtual void OnChangeDbKey();
    virtual void OnDbSettings();
    virtual void OnFileNew();
    virtual void OnViewToolbarToggled(bool toggled);
    virtual void OnViewEntryDetailsToggled(bool toggled);
    virtual void OnHelpAbout();
    virtual void OnSettings();
    virtual void OnView_ColumnUrlToggled(bool value);
    virtual void OnView_ColumnTitleToggled(bool value);
    virtual void OnView_ColumnCreationToggled(bool value);
    virtual void OnView_ColumnLastAccessToggled(bool value);
    virtual void OnView_ColumnLastModToggled(bool value);
    virtual void OnView_ColumnCommentToggled(bool value);
    virtual void OnView_ColumnPasswordToggled(bool value);
    virtual void OnView_ColumnAttachmentToggled(bool value);
    virtual void OnView_ColumnExpireToggled(bool value);
    virtual void OnView_ColumnUsernameToggled(bool value);
    virtual void OnExtrasLanguage();
    virtual void OnSearchGroupCtxMenuClicked(int);
    virtual void OnQickSearch();
    virtual void OnGroupItemExpanded(QListViewItem* item);
    virtual void OnGroupItemCollapsed(QListViewItem* item);
    virtual void DEBUG_OnPrintDbStucture();
    virtual void OnImportPwManagerFile();
    virtual void OnView_HideUsernamesToggled(bool state);
    virtual void OnView_HidePasswordsToggled(bool state);
    virtual void OnGroupViewDrop(QDropEvent* e);
    virtual void OnHelpAboutQt();
    virtual void OnImportKWalletXML();


public:
  PwDatabase* db;
  vector<GroupItem*> GroupItems;
  vector<EntryItem*> EntryItems;
  vector<unsigned long> SearchResults;
  QString img_res_dir;
  QPixmap* EntryIcons;
  QPixmap* Icon_Key32x32;
  QPixmap* Icon_Settings32x32;
  QPixmap* Icon_Search32x32;
  QPixmap* Icon_I18n32x32;
  QPixmap* Icon_Ok16x16;
  QTimer ClipboardTimer;
  QString appdir;
  CConfig config;

 enum FileMenuState{STATE_FileOpen,
				STATE_NoFileOpen};
 enum EditMenuState{STATE_NoGroupSelected,
				STATE_NoEntrySelected,
				STATE_SingleGroupSelected,
				STATE_SingleEntrySelected};
 void InitMenus();
 void SetFileMenuState(FileMenuState status);
 void SetEditMenuState(EditMenuState status);
 void LoadImg(QString name,QImage &tmpImg);
 void SetupColumns();
 void ResizeColumns();
 void OpenDatabase(QString filename);
 void CreateBanner(QLabel *Banner,QPixmap* symbol,QString text);
 void CreateBanner(QLabel *Banner,QPixmap* symbol,QString text,QColor color1,QColor color2,QColor textcolor);
 void CloseDataBase();
 void OpenURL(QString url);
 GroupItem* getLastSameLevelItem(int i);

 void setCurrentGroup(GroupItem* item);
 void updateEntryView();
 void updateGroupView();
 void updateEntryDetails();
 void updateEntryDetails(EntryItem* item);
 inline EntryItem* currentEntryItem();
 inline CEntry* currentEntry();
 GroupItem* CurrentGroup;
 QClipboard* Clipboard;

private:
 bool isInSearchResults(CEntry*);
 void Search(CGroup* pGroup=NULL);
 QApplication* App;
 QTranslator* translator;
 bool modflag;
 void setModFlag(bool);
 bool FileOpen;
 QString IniFilename;
};

#endif

