# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/keepass

INSTALLS += Share \
            target 
target.path = /usr/local/bin 
Share.files += ../share/keepass/* 
Share.path = /usr/local/share/keepass 
FORMS += forms/ui_EditGroupDlg.ui \
         forms/ui_SearchDlg.ui \
         forms/ui_AboutDlg.ui \
         forms/ui_LanguageDlg.ui \
         forms/ui_SettingsDlg.ui \
         forms/ui_ChangeKeyDlg.ui \
         forms/ui_MainWindow.ui \
         forms/ui_SimplePasswordDlg.ui \
         forms/ui_DatabaseSettingsDlg.ui \
         forms/ui_PasswordDlg.ui \
         forms/ui_EditEntryDlg.ui \
         forms/ui_PasswordGenDlg.ui 
TRANSLATIONS += translations/english.ts \
                translations/russian.ts 
HEADERS += lib/IniReader.h \
           lib/UrlLabel.h \
           mainwindow.h \
           PwManager.h \
           pwsafe.h \
           crypto/rijndael.h \
           lib/SecString.h \
           crypto/sha256.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           ListViews.h \
           import/Import_PwManager.h \
           crypto/blowfish.h \
           crypto/sha1.h \
           import/Import_KWalletXml.h \
           PwmConfig.h \
           dialogs/AboutDlg.h \
           dialogs/EditGroupDlg.h \
           dialogs/SearchDlg.h \
           dialogs/ChangeKeyDlg.h \
           dialogs/LanguageDlg.h \
           dialogs/SettingsDlg.h \
           dialogs/DatabaseSettingsDlg.h \
           dialogs/PasswordDlg.h \
           dialogs/SimplePasswordDlg.h \
           dialogs/EditEntryDlg.h \
           dialogs/PasswordGenDlg.h \
           lib/random.h \
           Database.h \
           lib/PwmTime.h \
           lib/KdePlugin.h \
           global.h 
SOURCES += lib/IniReader.cpp \
           lib/UrlLabel.cpp \
           main.cpp \
           mainwindow.cpp \
           PwManager.cpp \
           pwsafe.cpp \
           crypto/rijndael.cpp \
           lib/SecString.cpp \
           crypto/sha256.c \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           ListViews.cpp \
           import/Import_PwManager.cpp \
           crypto/blowfish.cpp \
           crypto/sha1.cpp \
           import/Import_KWalletXml.cpp \
           PwmConfig.cpp \
           dialogs/AboutDlg.cpp \
           dialogs/EditGroupDlg.cpp \
           dialogs/SearchDlg.cpp \
           dialogs/ChangeKeyDlg.cpp \
           dialogs/LanguageDlg.cpp \
           dialogs/SettingsDlg.cpp \
           dialogs/DatabaseSettingsDlg.cpp \
           dialogs/PasswordDlg.cpp \
           dialogs/SimplePasswordDlg.cpp \
           dialogs/EditEntryDlg.cpp \
           dialogs/PasswordGenDlg.cpp \
           lib/random.cpp \
           Database.cpp \
           lib/PwmTime.cpp \
           lib/KdePlugin.cpp 
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build/
TARGET = ../bin/keepass
CONFIG += debug \
warn_off \
qt \
thread \
exceptions \
stl
TEMPLATE = app
