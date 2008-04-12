/***************************************************************************
 *   Copyright (C) 2005-2008 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
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

#include <QProcess>
#include <QDesktopServices>

void createBanner(QPixmap* Pixmap,const QPixmap* IconAlpha,const QString& Text,int Width){
	createBanner(Pixmap,IconAlpha,Text,Width,config->bannerColor1(),config->bannerColor2(),config->bannerTextColor());
}

void createBanner(QPixmap* Pixmap,const QPixmap* IconAlpha,const QString& Text,int Width, QColor Color1, QColor Color2, QColor TextColor){
	*Pixmap=QPixmap(Width,50);
	QPainter painter(Pixmap);
	QLinearGradient grad(0,0,Width,0);
	grad.setColorAt(0,Color1);
	grad.setColorAt(1,Color2);
	painter.setPen(Qt::NoPen);
	painter.setBrush(grad);
	painter.drawRect(0,0,Width,50);
	
	QPixmap Icon(32,32);
	if(IconAlpha){
		Icon.fill(TextColor);
		Icon.setAlphaChannel(*IconAlpha);
		painter.drawPixmap(10,10,Icon);
	}
	
	painter.setPen(QPen(TextColor));
	painter.setFont(QFont(QApplication::font().family(),16));
	painter.drawText(50,35,Text);
}

QString decodeFileError(QFile::FileError Code){
	switch(Code){
	case QFile::NoError: return QApplication::translate("FileErrors","No error occurred.");
	case QFile::ReadError: return QApplication::translate("FileErrors","An error occurred while reading from the file.");
	case QFile::WriteError: return QApplication::translate("FileErrors","An error occurred while writing to the file.");
	case QFile::FatalError: return QApplication::translate("FileErrors","A fatal error occurred.");
	case QFile::ResourceError: return QApplication::translate("FileErrors","An resource error occurred.");
	case QFile::OpenError: return QApplication::translate("FileErrors","The file could not be opened.");
	case QFile::AbortError: return QApplication::translate("FileErrors","The operation was aborted.");
	case QFile::TimeOutError: return QApplication::translate("FileErrors","A timeout occurred.");
	case QFile::UnspecifiedError: return QApplication::translate("FileErrors","An unspecified error occurred.");
	case QFile::RemoveError: return QApplication::translate("FileErrors","The file could not be removed.");
	case QFile::RenameError: return QApplication::translate("FileErrors","The file could not be renamed.");
	case QFile::PositionError: return QApplication::translate("FileErrors","The position in the file could not be changed.");
	case QFile::ResizeError: return QApplication::translate("FileErrors","The file could not be resized.");
	case QFile::PermissionsError: return QApplication::translate("FileErrors","The file could not be accessed.");
	case QFile::CopyError: return QApplication::translate("FileErrors","The file could not be copied.");
	}
	return QString();
}

void openBrowser(IEntryHandle* entry){
	QString url = entry->url();
	url.replace("{TITLE}", entry->title(), Qt::CaseInsensitive);
	url.replace("{USERNAME}", entry->username(), Qt::CaseInsensitive);
	
	if (url.contains("{PASSWORD}",Qt::CaseInsensitive)){
		SecString password=entry->password();
		password.unlock();
		url.replace("{PASSWORD}", password, Qt::CaseInsensitive);
	}
	
	openBrowser(url);
}

void openBrowser(const QString& UrlString){
	if (UrlString.trimmed().isEmpty())
		return;
	
	if (UrlString.startsWith("cmd://") && UrlString.length()>6){
		QProcess::startDetached(UrlString.right(UrlString.length()-6));
		return;
	}
	
	QUrl url(UrlString);
	if(url.scheme().isEmpty())
		url=QUrl("http://"+UrlString);
	if(config->urlCmdDef() || url.scheme()=="mailto"){
		QDesktopServices::openUrl(url);
	}
	else{
		QByteArray UrlEncoded = url.toEncoded();
		QString browser = config->urlCmd();
		if (browser.contains("%u", Qt::CaseInsensitive))
			browser.replace("%u", UrlEncoded, Qt::CaseInsensitive);
		else if (browser.contains("%1"))
			browser.replace("%1", UrlEncoded);
		else
			browser.append(" ").append(UrlEncoded);
		QProcess::startDetached(browser);
	}
}


QString makePathRelative(const QString& AbsDir,const QString& CurDir){
	QStringList abs=AbsDir.split('/');
	QStringList cur=CurDir.split('/');
	QString rel="./";
	int common;
	for(common=0; common < abs.size() && common < cur.size(); common++){
		if(abs[common]!=cur[common])break;
	}
	for(int i=0;i<cur.size()-common;i++)
		rel.append("../");
	for(int i=common;i<abs.size();i++)
		rel.append(abs[i]+"/");
	return rel;
}

void showErrMsg(const QString& msg,QWidget* parent){
	QMessageBox::critical(parent,QApplication::translate("Main","Error"),msg,QApplication::translate("Main","OK"));
}

QString getImageFile(const QString& name){
	if (QFile::exists(DataDir+"/icons/"+name))
		return DataDir+"/icons/"+name;
	else{
		QMessageBox::critical(0,QApplication::translate("Main","Error"),
		                      QApplication::translate("Main","File '%1' could not be found.")
		                      .arg(name),QApplication::translate("Main","OK"),0,0,2,1);
		exit(1);
	}
}


const QIcon& getIcon(const QString& name){
	static QHash<QString,QIcon*>IconCache;		
	QIcon* CachedIcon=IconCache.value(name);
	if(CachedIcon)
		return *CachedIcon;
	QIcon* NewIcon=NULL;
	if(IconLoader){
		NewIcon=new QIcon(IconLoader->getIcon(name));
		if(NewIcon->isNull()){
			delete NewIcon;
			NewIcon=NULL;
		}
		else
			IconCache.insert(name,NewIcon);
	}
	if(!NewIcon)
	{
		NewIcon=new QIcon(getImageFile(name+".png"));
		IconCache.insert(name,NewIcon);
	}
	return *NewIcon;
}

const QPixmap* getPixmap(const QString& name){
	static QHash<QString,QPixmap*>PixmapCache;
	QPixmap* CachedPixmap=PixmapCache.value(name);
	if(CachedPixmap)
		return CachedPixmap;
	QImage img(getImageFile(name+".png"));
	QPixmap* NewPixmap=new QPixmap(QPixmap::fromImage(img));
	PixmapCache.insert(name,NewPixmap);
	return NewPixmap;
}


bool createKeyFile(const QString& filename,QString* error,int length, bool Hex){
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Unbuffered)){
		*error=decodeFileError(file.error());
		return false;
	}
	if(Hex)length*=2;
	unsigned char* key=new unsigned char[length];
	randomize(key,length);
	if(Hex){
		// convert binary data to hex code (8 bit ==> 2 digits)
		for(int i=0; i<length; i+=2){
			unsigned char dig1,dig2;
			dig1=key[i]/16;
			key[i]-=(16*dig1);
			dig2=key[i];
			if(dig1>9)key[i]='A'+dig1-10;
			else key[i]='0'+dig1;
			if(dig2>9)key[i+1]='A'+dig2-10;
			else key[i+1]='0'+dig2;
		}
	}
	if(file.write((char*)key,length)==-1){
		delete [] key;
		*error=decodeFileError(file.error());
		file.close();
		return false;
	}
	file.close();
	delete [] key;
	return true;
}


