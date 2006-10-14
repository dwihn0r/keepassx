# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: ./src
# Das Target ist eine Anwendung:  ../bin/keepass


INSTALLS += target data
data.files += ../share/keepass/* 
TARGET = ../bin/keepassx

unix:!macx{
	isEmpty(PREFIX){
		PREFIX=/usr/local
	}
	target.path = $${PREFIX}/bin
	data.path = $${PREFIX}/share/keepass
	LIBS+=-lXtst
	SOURCES+=lib/AutoType_X11.cpp
}

macx{ 
	target.path = /Applications
	data.path = /Applications/keepass.app/Contents/share/keepass
	SOURCES+=lib/AutoType_X11.cpp
}

win32{
	SOURCES+=lib/AutoType_Win.cpp
	TARGET=../$$TARGET
	QMAKE_LINK_OBJECT_SCRIPT=../build/$$QMAKE_LINK_OBJECT_SCRIPT

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
         forms/PasswordGenDlg.ui \
	 forms/SelectIconDlg.ui
TRANSLATIONS += translations/keepass-de_DE.ts \
		translations/keepass-ru_RU.ts \
		translations/keepass-es_ES.ts \
		translations/keepass-fr_FR.ts \
                translations/keepass-xx_XX.ts
HEADERS += lib/IniReader.h \
           lib/UrlLabel.h \
           mainwindow.h \
           StandardDatabase.h \
           lib/SecString.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           import/Import_PwManager.h \
           export/Export_Txt.h \
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
	   dialogs/SelectIconDlg.h \
           lib/random.h \
           Database.h \
           lib/KdePlugin.h \
	   lib/AutoType.h \
           global.h \
           main.h \
           lib/GroupView.h \
           lib/EntryView.h \
           crypto/arcfour.h \
           lib/KpFileIconProvider.h \
	   crypto/aes_edefs.h \
	   crypto/aes_tdefs.h \
	   crypto/aes.h \
	   crypto/aesopt.h \
	   crypto/aestab.h \
	   crypto/aescpp.h \
	   crypto/sha256.h \
	   crypto/yarrow.h
SOURCES += lib/IniReader.cpp \
           lib/UrlLabel.cpp \
           main.cpp \
           mainwindow.cpp \
           StandardDatabase.cpp \
           lib/SecString.cpp \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           import/Import_PwManager.cpp \
           export/Export_Txt.cpp \
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
	   dialogs/SelectIconDlg.cpp \
           lib/random.cpp \
           Database.cpp \
           lib/KdePlugin.cpp \
           lib/GroupView.cpp \
           lib/EntryView.cpp \
           crypto/arcfour.cpp \
           lib/KpFileIconProvider.cpp \
	   crypto/aescrypt.c \
	   crypto/aeskey.c \
	   crypto/aestab.c \
	   crypto/aes_modes.c \
	   crypto/sha256.cpp \
	   crypto/yarrow.cpp	
QT += xml
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build/
INCLUDEPATH += ./
CONFIG += debug \
warn_off \
qt \
thread \
exceptions \
stl
TEMPLATE = app
