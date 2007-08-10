/***************************************************************************
 *   Copyright (C) 2005 by Tarek Saidi                                     *
 *   tarek@linux                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *

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

#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H


#include <QColor>
#include <QPixmap>
#include <QPaintEvent>
#include "ui_SettingsDlg.h"
#include "main.h"

class CSettingsDlg : public QDialog, public Ui_SettingsDialog
{
	Q_OBJECT
	public:
		CSettingsDlg(QWidget* parent);
		~CSettingsDlg();

	public slots:
    	virtual void OnCancel();
    	virtual void OnOK();
    	virtual void OnTextColor();
    	virtual void OnColor2();
    	virtual void OnColor1();
		void OnOtherButton(QAbstractButton*);
		void OnIntPluginNone(bool);
		void OnIntPluginGnome(bool);
		void OnIntPluginKde(bool);
		void OnCheckBoxOpenLastChanged(int state);
		void OnCheckBoxBrowserDefaultChanged(int state);
		void OnMountDirBrowse();
		void OnCustomizeEntryDetails();

	private:
 		virtual void paintEvent(QPaintEvent*);
		void apply();
 		QColor color1,color2,textcolor;
 		QPixmap BannerPixmap;
		static bool PluginsModified;

};

#endif

