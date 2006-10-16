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
  
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QPointF>

class WaitAnimationWidget:public QWidget{
	Q_OBJECT
	public:
		WaitAnimationWidget(QWidget* parent);
		void setSpeed(int rpm){speed=rpm;}
		void setRefreshRate(int fps);
	public slots:
		void refreshAnimation();
	private:
		virtual void paintEvent(QPaintEvent* event);
		virtual void resizeEvent(QResizeEvent* event);
		QTimer timer;
		int speed;
		float CurAngle;
		float DiffAngle;
		float CircSizes[6];
		QPointF CircPositions[6];
};