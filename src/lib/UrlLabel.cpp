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


#include "UrlLabel.h"
#include "main.h"
#include <qfont.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qfontmetrics.h>
#include <QMouseEvent>
#include <QLabel>

LinkLabel::LinkLabel(QWidget *parent, const char* name,const QString& text, int x, int y,Qt::WFlags f) : QLabel(parent,name,f){
QFont font(parentWidget()->font()); font.setUnderline(true);
setFont(font);
setPaletteForegroundColor(QColor(20,20,255));
setCursor(Qt::PointingHandCursor);
setText(text);
setPos(x,y);
}

LinkLabel::~LinkLabel(){

}

QString LinkLabel::url(){
if(URL!=QString())return URL;
if(text().contains("@"))
	return QString("mailto:")+text();
else return text();
}

void LinkLabel::mouseReleaseEvent(QMouseEvent* event){
if(event->button()==Qt::LeftButton)
emit clicked();
openBrowser(url());
}

void LinkLabel::setPos(int x,int y){
QFontMetrics fm(font());
setGeometry(x,y,fm.width(text()),fm.height());
}

void LinkLabel::setText(const QString& text){
QLabel::setText(text);
setPos(geometry().x(),geometry().y());
}
