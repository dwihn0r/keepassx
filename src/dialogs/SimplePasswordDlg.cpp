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

#include <qlineedit.h>
#include <qpushbutton.h>
#include "main.h"
#include "PwmConfig.h"
#include "SimplePasswordDlg.h"

CSimplePasswordDialog::CSimplePasswordDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent,name, modal,fl)
{
setupUi(this);
if(!config.ShowPasswords)Button_HidePassword->toggle();
}

CSimplePasswordDialog::~CSimplePasswordDialog()
{
}

void CSimplePasswordDialog::OnCancel()
{
done(0);
}


void CSimplePasswordDialog::OnOK()
{
password=EditPassword->text();
done(1);
}


void CSimplePasswordDialog::OnHidePasswordToggled(bool state)
{
if(state)EditPassword->setEchoMode(QLineEdit::Password);
else EditPassword->setEchoMode(QLineEdit::Normal);
}


/*$SPECIALIZATION$*/


