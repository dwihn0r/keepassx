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
#ifndef SEARCHDLG_H
#define SEARCHDLG_H
#include "ui_SearchDlg.h"
#include "main.h"
#include "PwManager.h"

class CSearchDlg : public QDialog, public Ui_Search_Dlg
{
  Q_OBJECT
public:
  CSearchDlg(CGroup* pGroup=NULL,QWidget* parent = 0, const char* name = 0, 
	     bool modal = FALSE, Qt::WFlags fl = 0 );
  ~CSearchDlg();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

public slots:
    virtual void OnButtonClose();
public slots:
    virtual void OnButtonSearch();

private:
  QString txt;
  bool regexp;
};

#endif

