/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
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
#include "mainwindow.h"
#ifndef EDITENTRYDLG_H
#define EDITENTRYDLG_H
#include "ui_EditEntryDlg.h"
//Added by qt3to4:
#include <QPixmap>
#include <QShowEvent>

class CEditEntryDlg : public EditEntryDialog
{
  Q_OBJECT

public:
  CEditEntryDlg(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0);
  ~CEditEntryDlg();
  virtual void showEvent(QShowEvent *);
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  virtual void OnButtonOK();
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

public:
CMainWindow* mainwnd;
CEntry* entry;
PwDatabase* pw;
QPixmap* banner_pixmap;
bool ModFlag;


void CreateBanner();
void InitGroupComboBox();
void InitIconComboBox();


public slots:
    virtual void OnExpTimeLostFocus();
    virtual void OnExpDateLostFocus();
    virtual void OnPasswordwLostFocus();
    virtual void OnPasswordwTextChanged(const QString&);
    virtual void OnPasswordTextChanged(const QString&);
    virtual void ChangeEchoMode();
    virtual void OnButtonCancel();
    virtual void OnNewAttachment();
    virtual void OnDeleteAttachment();
    virtual void OnSaveAttachment();
    virtual void OnButtonGenPw();

};

#endif

