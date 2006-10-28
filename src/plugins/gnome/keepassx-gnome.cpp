/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
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
 
#include <gtk/gtk.h> 
#include "keepassx-gnome.h"
#define CSTRING(x)(x.toUtf8().data())

Q_EXPORT_PLUGIN2(keepassx_gnome, GnomePlugin)

QString GnomePlugin::openExistingFileDialog(QWidget* parent,QString title,QString dir,
					QStringList Filters){

	GtkWidget *FileDlg;
	QString filename;
	gtk_init(0,0);
	FileDlg=gtk_file_chooser_dialog_new(title.toUtf8().data(),NULL,
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(FileDlg),dir.toUtf8().data());
	GtkFileFilter** filters=parseFilterStrings(Filters);
	
	for(int i=0;i<Filters.size();i++){		
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileDlg),filters[i]);
	}
	if (gtk_dialog_run(GTK_DIALOG(FileDlg)) == GTK_RESPONSE_ACCEPT){
		char* filename_cstring=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDlg));
		filename = QString::fromUtf8(filename_cstring);
		g_free(filename_cstring);
 	}
	gtk_widget_destroy (FileDlg);
	return filename;
}

GtkFileFilter** GnomePlugin::parseFilterStrings(const QStringList& filters){
	if(!filters.size())return NULL;
	GtkFileFilter **f=new GtkFileFilter*[filters.size()];
	for(int i=0;i<filters.size();i++){
		f[i]=gtk_file_filter_new();
		QString name;
		int p=0;
		for(p;p<filters[i].size();p++){
			if(filters[i][p]!='(')
				name += filters[i][p];
			else
				break;			
		}
		gtk_file_filter_set_name(f[i],CSTRING(name));
		p++;
		QString pattern;
		for(p;p<filters[i].size()-1;p++){
			if(filters[i][p]==' '){
				gtk_file_filter_add_pattern(f[i],CSTRING(pattern));
				pattern=QString();
			}
			else{
				pattern += filters[i][p];				
			}
			
		}
	}
	return f;
}

QString GnomePlugin::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters){return QString();}