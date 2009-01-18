
CONFIG = qt uic resources thread stl warn_off
QT += xml

DEPENDPATH += crypto dialogs export forms import lib translations res
INCLUDEPATH += . lib crypto plugins/interfaces export import dialogs

MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build
RCC_DIR = ../build/rcc

isEqual(DEBUG,1){
	CONFIG += debug
}
else {
	CONFIG += release
}

isEqual(NOSTRIP,1) {
	CONFIG += nostrip
}

# lipo and freebsd cannot handle precompiled headers (yet)
!isEqual(PRECOMPILED,1){
	macx : isEqual(ARCH,UNIVERSAL) : PRECOMPILED = 0
	freebsd-* : PRECOMPILED = 0
}

win32 : QMAKE_WIN32 = 1

#-------------------------------------------------------------------------------
#   Platform Specific: Unix (except MacOS X)
#-------------------------------------------------------------------------------
unix : !macx : !isEqual(QMAKE_WIN32,1){
	isEmpty(PREFIX): PREFIX = /usr
	!isEqual(AUTOTYPE,0){
		DEFINES += AUTOTYPE
		!isEqual(GLOBAL_AUTOTYPE,0){
			DEFINES += GLOBAL_AUTOTYPE
		}
	}
	TARGET = ../bin/keepassx
	target.path = $${PREFIX}/bin
	data.files += ../share/keepassx
	data.path = $${PREFIX}/share
	share.files = ../share/applications ../share/mime ../share/mimelnk ../share/pixmaps
	share.path = $${PREFIX}/share
	INSTALLS += share
	contains(DEFINES,AUTOTYPE){
		LIBS += -lXtst
		SOURCES += lib/HelperX11.cpp lib/AutoTypeX11.cpp
		HEADERS += lib/HelperX11.h lib/AutoTypeX11.h
	}
	contains(DEFINES,GLOBAL_AUTOTYPE){
		SOURCES += Application_X11.cpp lib/AutoTypeGlobalX11.cpp
		HEADERS += Application_X11.h lib/AutoTypeGlobalX11.h
	}
	SOURCES += main_unix.cpp
}


#-------------------------------------------------------------------------------
#   Platform Specific: MacOS X
#-------------------------------------------------------------------------------
macx {
	isEmpty(PREFIX): PREFIX = /Applications
	TARGET = ../bin/KeePassX
	target.path = $${PREFIX}
	data.files += ../share/keepassx
	data.path = Contents/Resources
	LIBS += -framework CoreFoundation
	isEqual(LINK,DYNAMIC){
		isEmpty(QT_FRAMEWORK_DIR): QT_FRAMEWORK_DIR = /Library/Frameworks
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtCore.framework
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtGui.framework
		private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtXml.framework
		private_frameworks.path = Contents/Frameworks
		QMAKE_BUNDLE_DATA +=  private_frameworks
	}
	isEqual(LINK,STATIC){
		LIBS += -framework Carbon -framework AppKit -lz
	}
	QMAKE_BUNDLE_DATA += data
	QMAKE_INFO_PLIST= ../share/macx_bundle/Info.plist
	ICON = ../share/macx_bundle/icon.icns
	CONFIG += app_bundle
	isEqual(ARCH,UNIVERSAL){
		CONFIG += x86 ppc
	}
	isEqual(ARCH,INTEL): CONFIG += x86
	isEqual(ARCH,PPC): CONFIG += ppc
	SOURCES += main_macx.cpp
}

#-------------------------------------------------------------------------------
#   Platform Specific: Windows
#-------------------------------------------------------------------------------
isEqual(QMAKE_WIN32,1){
	CONFIG += windows
	isEmpty(PREFIX): PREFIX = "C:/Program\ files/KeePassX"
	TARGET = ../bin/KeePassX
	target.path = $${PREFIX}
	data.files += ../share/keepassx/*
	data.path = $${PREFIX}/share
	!isEqual(INSTALL_QTLIB,0){
		qt_libs.files = $${QMAKE_LIBDIR_QT}/QtCore4.dll $${QMAKE_LIBDIR_QT}/QtGui4.dll $${QMAKE_LIBDIR_QT}/QtXml4.dll
		qt_libs.path = $${PREFIX}
		INSTALLS += qt_libs
	}
	RC_FILE = ../share/win_ico/keepassx.rc
	QMAKE_LINK_OBJECT_SCRIPT = $${OBJECTS_DIR}/$${QMAKE_LINK_OBJECT_SCRIPT}
	SOURCES += main_win32.cpp
}


INSTALLS += target data

contains(DEFINES,GLOBAL_AUTOTYPE){
	FORMS += forms/AutoTypeDlg.ui forms/TargetWindowDlg.ui
	HEADERS += dialogs/AutoTypeDlg.h lib/AutoTypeTreeWidget.h dialogs/TargetWindowDlg.h
	SOURCES += dialogs/AutoTypeDlg.cpp lib/AutoTypeTreeWidget.cpp dialogs/TargetWindowDlg.cpp
}


FORMS += forms/AboutDlg.ui \
         forms/AddBookmarkDlg.ui \
         forms/CalendarDlg.ui \
         forms/CollectEntropyDlg.ui \
         forms/CustomizeDetailViewDlg.ui \
         forms/DatabaseSettingsDlg.ui \
         forms/EditEntryDlg.ui \
         forms/EditGroupDlg.ui \
         forms/ExpiredEntriesDlg.ui \
         forms/MainWindow.ui \
         forms/ManageBookmarksDlg.ui \
         forms/PasswordDlg.ui \
         forms/PasswordGenDlg.ui \
         forms/SearchDlg.ui \
         forms/SelectIconDlg.ui \
         forms/SettingsDlg.ui \
         forms/SimplePasswordDlg.ui \
#         forms/TrashCanDlg.ui \
         forms/WorkspaceLockedWidget.ui

TRANSLATIONS += translations/keepassx-cs_CZ.ts \
                translations/keepassx-de_DE.ts \
                translations/keepassx-es_ES.ts \
                translations/keepassx-fr_FR.ts \
                translations/keepassx-gl_ES.ts \
                translations/keepassx-it_IT.ts \
                translations/keepassx-ja_JP.ts \
                translations/keepassx-ru_RU.ts \
                translations/keepassx-xx_XX.ts

HEADERS += main.h \
           global.h \
           mainwindow.h \
           KpxConfig.h \
           Database.h \
           Kdb3Database.h \
           lib/AutoType.h \
           lib/bookmarks.h \
           lib/EntryView.h \
           lib/FileDialogs.h \
           lib/GroupView.h \
           lib/random.h \
           lib/SecString.h \
           lib/ShortcutWidget.h \
           lib/tools.h \
           lib/UrlLabel.h \
           lib/WaitAnimationWidget.h \
           crypto/aes.h \
           crypto/aescpp.h \
           crypto/aes_endian.h \
           crypto/aes_types.h \
           crypto/aesopt.h \
           crypto/aestab.h \
           crypto/aes_via_ace.h \
           crypto/arcfour.h \
           crypto/blowfish.h \
           crypto/sha256.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           crypto/yarrow.h \
           apg/convert.h \
           apg/owntypes.h \
           apg/pronpass.h \
           apg/randpass.h \
           apg/smbl.h \
           dialogs/AboutDlg.h \
           dialogs/AddBookmarkDlg.h \
           dialogs/CalendarDlg.h \
           dialogs/CollectEntropyDlg.h \
           dialogs/CustomizeDetailViewDlg.h \
           dialogs/DatabaseSettingsDlg.h \
           dialogs/EditEntryDlg.h \
           dialogs/EditGroupDlg.h \
           dialogs/ExpiredEntriesDlg.h \
           dialogs/ManageBookmarksDlg.h \
           dialogs/PasswordDlg.h \
           dialogs/PasswordGenDlg.h \
           dialogs/SearchDlg.h \
           dialogs/SelectIconDlg.h \
           dialogs/SettingsDlg.h \
           dialogs/SimplePasswordDlg.h \
#           dialogs/TrashCanDlg.h \
           import/Import.h \
#           import/Import_GnuKeyRing.h \
           import/Import_KeePassX_Xml.h \
           import/Import_KWalletXml.h \
           import/Import_PwManager.h \
           export/Export.h \
           export/Export_KeePassX_Xml.h \
           export/Export_Txt.h \
           plugins/interfaces/IFileDialog.h \
           plugins/interfaces/IIconTheme.h \
           plugins/interfaces/IGnomeInit.h \
           plugins/interfaces/IKdeInit.h

SOURCES += main.cpp \
           mainwindow.cpp \
           KpxConfig.cpp \
           Database.cpp \
           Kdb3Database.cpp \
           lib/bookmarks.cpp \
           lib/EntryView.cpp \
           lib/FileDialogs.cpp \
           lib/GroupView.cpp \
           lib/random.cpp \
           lib/SecString.cpp \
           lib/ShortcutWidget.cpp \
           lib/tools.cpp \
           lib/UrlLabel.cpp \
           lib/WaitAnimationWidget.cpp \
           crypto/aescrypt.c \
           crypto/aeskey.c \
           crypto/aes_modes.c \
           crypto/aestab.c \
           crypto/arcfour.cpp \
           crypto/blowfish.cpp \
           crypto/sha256.cpp \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           crypto/yarrow.cpp \
           apg/convert.c \
           apg/pronpass.c \
           apg/randpass.c \
           dialogs/AboutDlg.cpp \
           dialogs/AddBookmarkDlg.cpp \
           dialogs/CalendarDlg.cpp \
           dialogs/CollectEntropyDlg.cpp \
           dialogs/CustomizeDetailViewDlg.cpp \
           dialogs/DatabaseSettingsDlg.cpp \
           dialogs/EditEntryDlg.cpp \
           dialogs/EditGroupDlg.cpp \
           dialogs/ExpiredEntriesDlg.cpp \
           dialogs/ManageBookmarksDlg.cpp \
           dialogs/PasswordDlg.cpp \
           dialogs/PasswordGenDlg.cpp \
           dialogs/SearchDlg.cpp \
           dialogs/SelectIconDlg.cpp \
           dialogs/SettingsDlg.cpp \
           dialogs/SimplePasswordDlg.cpp \
#           dialogs/TrashCanDlg.cpp \
           import/Import.cpp \
#           import/Import_GnuKeyRing.cpp \
           import/Import_KeePassX_Xml.cpp \
           import/Import_KWalletXml.cpp \
           import/Import_PwManager.cpp \
           export/Export.cpp \
           export/Export_KeePassX_Xml.cpp \
           export/Export_Txt.cpp

isEqual(PRECOMPILED,0) {
	QMAKE_CXXFLAGS += -include keepassx.h
}
else {
	CONFIG += precompile_header
	PRECOMPILED_HEADER = keepassx.h
}

RESOURCES += res/resources.qrc
