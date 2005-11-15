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
#ifndef _CHANGEKEYDLG_H_
#define _CHANGEKEYDLG_H_
#include "ui_ChangeKeyDlg.h"
#include <qstringlist.h>
#include <q3valuelist.h>
#include <qfile.h>

class CChangeKeyDlg : public ChangeKeyDialog
{
  Q_OBJECT

public:
  CChangeKeyDlg(QWidget* parent,PwDatabase* db, const char* name = 0, bool modal = true, Qt::WFlags fl = 0 );
  ~CChangeKeyDlg();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

public slots:
    virtual void OnSelect();
    virtual void OnBrowse();
    virtual void OnCancel();
    virtual void OnOK();
    virtual void OnComboChanged(int i);
    virtual void OnCheckBoxChanged(int i);
    virtual void OnPasswordChanged(const QString& str);
    virtual void OnPassword2Changed(const QString& str);
    virtual void ChangeEchoMode();


private:
PwDatabase* db;
CMainWindow* parentwnd;
QStringList Paths;
Q3ValueList<bool> IsFile;

public:
QString keyfile;
QFile* pKeyFile;
QString password;
};

#endif

