#ifndef _KDEPLUGIN_H_
#define _KDEPLUGIN_H_
#include <qlibrary.h>
#include <qapplication.h>

class CKdePlugin{
public:
	bool resolveSymbols(QLibrary& lib);
	QApplication*(*getAppObj)(int,char**);
};


#endif