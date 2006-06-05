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
#ifndef GENPWDIALOG_H
#define GENPWDIALOG_H
#include "ui_PasswordGenDlg.h"
#include "EditEntryDlg.h"



class CGenPwDialog : public QDialog, public Ui_GenPwDlg
{
  Q_OBJECT
private:
int AddToAssoctable(char* table,int start,int end,int pos);
bool trim(unsigned char &value,int range);
public:
  CGenPwDialog(QWidget* parent = 0,  bool modal = FALSE, Qt::WFlags fl = 0 );
  ~CGenPwDialog();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
public slots:
    virtual void OnGeneratePw();
    virtual void OnRadio2StateChanged(bool);
    virtual void OnRadio1StateChanged(bool);
    virtual void OnCancel();
    virtual void OnAccept();
};

#endif

