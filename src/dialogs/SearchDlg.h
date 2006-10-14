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
#include "Database.h"

class CSearchDlg : public QDialog, public Ui_Search_Dlg
{
  Q_OBJECT
public:
  CSearchDlg(IDatabase* database, IGroupHandle* group=NULL,QWidget* parent = 0, bool modal = true, Qt::WFlags fl = 0 );
  ~CSearchDlg();
  QList<IEntryHandle*> Result;

public slots:
    virtual void OnClose();
    virtual void OnSearch();

private:
 	IGroupHandle* group;
	IDatabase* db;
};

#endif

