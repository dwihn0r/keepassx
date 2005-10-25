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
#include "pwsafe.h"

PwSafe::PwSafe(QApplication* app):QMainWindow( 0, "Keepass",WDestructiveClose)
{
QString ArgFile,ArgCfg;
if(app->argc()>1){
int i=1;
	if(app->argv()[i][0]!='-'){
		ArgFile=app->argv()[i];
		i++;
	}
	for(i; i<app->argc();i++){
		if(QString(app->argv()[i])=="-h")
			argHelp();
		else if(QString(app->argv()[i])=="-cfg"){
			if(i-1==app->argc()) cout << "No configuration file specified" << endl;
			else{ArgCfg=app->argv()[i];}
			}
		else{cout << "** Unrecognized option: " << app->argv()[i] <<  endl;
			exit(1);}
	}




}
  mainWin=new CMainWindow(app,ArgFile,ArgCfg,this);
  setCentralWidget( mainWin );

}

PwSafe::~PwSafe()
{
    delete mainWin;
}

void PwSafe::argHelp(){
cout << "Keepass 0.1.3 (Alpha)" << endl;
cout << "Usage: keepass [Filename] [Options]" << endl;
cout << "  -h This Help" << endl;
cout << "  -cfg ConfigFile Use specified configuration" << endl;
exit(0);
}