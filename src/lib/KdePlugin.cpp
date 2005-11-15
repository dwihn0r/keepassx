
#include "KdePlugin.h"

bool CKdePlugin::resolveSymbols(QLibrary& lib){
getAppObj=(QApplication*(*)(int,char**))lib.resolve("getAppObj");
if(getAppObj == NULL) return false;
return true;
}