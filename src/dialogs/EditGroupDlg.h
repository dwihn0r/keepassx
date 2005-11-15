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

#ifndef EDITGROUPDLG_H
#define EDITGROUPDLG_H

#include "ui_EditGroupDlg.h"
#include <qstring.h>
//Added by qt3to4:
#include <QShowEvent>

class CEditGroupDialog : public EditGroupDialog
{
  Q_OBJECT

public:
  CEditGroupDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
  ~CEditGroupDialog();
virtual void showEvent(QShowEvent *event);
  /*$PUBLIC_FUNCTIONS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

public:
 int IconID;
 QString GroupName;
 bool OK;
 

public slots:
    virtual void OnOK();
    virtual void OnCancel();
};

#endif

