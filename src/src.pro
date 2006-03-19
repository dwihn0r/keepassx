# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/keepass

INSTALLS += target \
            Share
Share.files += ../share/keepass/* 
unix{ target.path = /usr/local/bin
      Share.path = /usr/local/share/keepass
}
macx{ target.path = /Applications
      Share.path = /Applications/keepass.app/Contents/share/keepass
}
FORMS += forms/EditGroupDlg.ui \
         forms/SearchDlg.ui \
         forms/AboutDlg.ui \
         forms/SettingsDlg.ui \
         forms/MainWindow.ui \
         forms/SimplePasswordDlg.ui \
         forms/DatabaseSettingsDlg.ui \
         forms/PasswordDlg.ui \
         forms/EditEntryDlg.ui \
         forms/PasswordGenDlg.ui 
TRANSLATIONS += translations/keepass-de_DE.ts \
		translations/keepass-ru_RU.ts \
		translations/keepass-es_ES.ts \
                translations/keepass-xx_XX.ts
HEADERS += lib/IniReader.h \
           lib/UrlLabel.h \
           mainwindow.h \
           PwManager.h \
           crypto/rijndael.h \
           lib/SecString.h \
           crypto/sha256.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           import/Import_PwManager.h \
           export/Export_Txt.h \
           crypto/blowfish.h \
           crypto/sha1.h \
           import/Import_KWalletXml.h \
           PwmConfig.h \
           dialogs/AboutDlg.h \
           dialogs/EditGroupDlg.h \
           dialogs/SearchDlg.h \
           dialogs/SettingsDlg.h \
           dialogs/DatabaseSettingsDlg.h \
           dialogs/PasswordDlg.h \
           dialogs/SimplePasswordDlg.h \
           dialogs/EditEntryDlg.h \
           dialogs/PasswordGenDlg.h \
           lib/random.h \
           Database.h \
           lib/KdePlugin.h \
           global.h \
           main.h \
           lib/GroupView.h \
           lib/EntryView.h \
           crypto/arcfour.h \
           lib/KpFileIconProvider.h 
SOURCES += lib/IniReader.cpp \
           lib/UrlLabel.cpp \
           main.cpp \
           mainwindow.cpp \
           PwManager.cpp \
           crypto/rijndael.cpp \
           lib/SecString.cpp \
           crypto/sha256.c \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           import/Import_PwManager.cpp \
           export/Export_Txt.cpp \
           crypto/blowfish.cpp \
           crypto/sha1.cpp \
           import/Import_KWalletXml.cpp \
           PwmConfig.cpp \
           dialogs/AboutDlg.cpp \
           dialogs/EditGroupDlg.cpp \
           dialogs/SearchDlg.cpp \
           dialogs/SettingsDlg.cpp \
           dialogs/DatabaseSettingsDlg.cpp \
           dialogs/PasswordDlg.cpp \
           dialogs/SimplePasswordDlg.cpp \
           dialogs/EditEntryDlg.cpp \
           dialogs/PasswordGenDlg.cpp \
           lib/random.cpp \
           Database.cpp \
           lib/KdePlugin.cpp \
           lib/GroupView.cpp \
           lib/EntryView.cpp \
           crypto/arcfour.cpp \
           lib/KpFileIconProvider.cpp 
QT += xml qt3support
QT -= network sql
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build/
TARGET = ../bin/keepass
INCLUDEPATH += ./
CONFIG += debug \
warn_off \
qt \
thread \
exceptions \
stl
TEMPLATE = app