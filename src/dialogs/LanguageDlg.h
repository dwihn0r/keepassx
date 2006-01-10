/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi   *
 *   mail@tarek-saidi.de   *
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
#ifndef _LANGUAGEDLG_H_
#define _LANGUAGEDLG_H_
#include "ui_LanguageDlg.h"
#include <qpixmap.h>
//Added by qt3to4:
#include <QShowEvent>



class CLanguageDlg : public QDialog, public Ui_LanguageDlg
{
  Q_OBJECT

public:
  CLanguageDlg(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
  ~CLanguageDlg();
  vector<QString> filenames;
  vector<Q3ListViewItem*> pItems;
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/


protected:
  /*$PROTECTED_FUNCTIONS$*/


protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void showEvent(QShowEvent *e);
    virtual void OnItemDoubleClicked(Q3ListViewItem* item);
    virtual void OnItemRightClick(Q3ListViewItem* item);
    virtual void OnButtonCloseClicked();
    virtual void OnApplyButtonClicked();

};

#endif

