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
#include "main.h"
#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H
#include "lib/UrlLabel.h"
#include "ui_PasswordDlg.h"
//Added by qt3to4:
#include <Q3ValueList>

class CPasswordDialog : public QDialog, public Ui_PasswordDlg
{
  Q_OBJECT

private:
int NumComboEntries;
QStringList Paths;
Q3ValueList<bool> IsFile;
LinkLabel* Label_select;


public:

QString keyfile;
QString password;



public:
  CPasswordDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
  ~CPasswordDialog();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
    virtual void OnOK();
    virtual void OnCancel();
    virtual void OnSelectClicked();
    virtual void OnButtonBrowse();
    virtual void OnComboSelectionChanged(int);
    virtual void OnPasswordChanged(const QString &txt);
    virtual void OnCheckBox_BothChanged(int state);
    virtual void ChangeEchoMode();
protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/


};

#endif

