
CONFIG = qt uic resources thread stl warn_off precompile_header
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
            pixmaps.files = ../share/pixmaps/*
            pixmaps.path = $${PREFIX}/share/pixmaps
            desktop.files = ../share/applications/*
            desktop.path = $${PREFIX}/share/applications
            INSTALLS += pixmaps desktop
            contains(DEFINES,AUTOTYPE){
                LIBS += -lXtst
                SOURCES += lib/HelperX11.cpp lib/AutoType_X11.cpp
                HEADERS += lib/HelperX11.h
            }
            contains(DEFINES,GLOBAL_AUTOTYPE){
                SOURCES += Application_X11.cpp
                HEADERS += Application_X11.h
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
    ICON = ../share/macx_bundle/icon.icns
    CONFIG += app_bundle
    isEqual(ARCH,UNIVERSAL) {
        CONFIG += x86 ppc
        # lipo cannot handle precompiled headers (yet)
        CONFIG -= precompile_header
        QMAKE_CXXFLAGS += -include keepassx.h
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
    isEmpty(PREFIX): PREFIX = "C:/Program files/KeePassX"
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
    FORMS += forms/AutoTypeDlg.ui
    HEADERS += dialogs/AutoTypeDlg.h
    SOURCES += dialogs/AutoTypeDlg.cpp
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
         forms/SelectIconDlg.ui \
         forms/CollectEntropyDlg.ui \
         forms/CustomizeDetailViewDlg.ui \
         forms/CalendarDlg.ui \
#         forms/TrashCanDlg.ui \
         forms/ExpiredEntriesDlg.ui \
         forms/WorkspaceLockedWidget.ui \
         forms/AddBookmarkDlg.ui \
         forms/ManageBookmarksDlg.ui

TRANSLATIONS += translations/keepass-de_DE.ts \
                translations/keepass-ru_RU.ts \
                translations/keepass-es_ES.ts \
                translations/keepass-fr_FR.ts \
                translations/keepass-cs_CZ.ts \
                translations/keepass-ja_JP.ts \
                translations/keepass-xx_XX.ts

HEADERS += lib/UrlLabel.h \
           mainwindow.h \
           Kdb3Database.h \
           lib/SecString.h \
           crypto/twoclass.h \
           crypto/twofish.h \
           import/Import.h \
           import/Import_KeePassX_Xml.h \
           import/Import_PwManager.h \
           export/Export_Txt.h \
           export/Export_KeePassX_Xml.h \
           export/Export.h \
           import/Import_KWalletXml.h \
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
           dialogs/CollectEntropyDlg.h \
           dialogs/CustomizeDetailViewDlg.h \
           dialogs/CalendarDlg.h \
           dialogs/ExpiredEntriesDlg.h \
#           dialogs/TrashCanDlg.h \
           lib/random.h \
           Database.h \
           lib/AutoType.h \
           lib/FileDialogs.h \
           lib/ShortcutWidget.h \
           global.h \
           main.h \
           lib/tools.h \
           lib/GroupView.h \
           lib/EntryView.h \
           crypto/arcfour.h \
           crypto/aes_edefs.h \
           crypto/aes_tdefs.h \
           crypto/aes.h \
           crypto/aesopt.h \
           crypto/aestab.h \
           crypto/aescpp.h \
           crypto/sha256.h \
           crypto/yarrow.h \
           crypto/blowfish.h \
           crypto/sha1.h \
           lib/WaitAnimationWidget.h \
           plugins/interfaces/IFileDialog.h \
           plugins/interfaces/IKdeInit.h \
           plugins/interfaces/IGnomeInit.h \
           plugins/interfaces/IIconTheme.h \
           KpxConfig.h \
           dialogs/AddBookmarkDlg.h \
           lib/bookmarks.h \
           dialogs/ManageBookmarksDlg.h

SOURCES += lib/UrlLabel.cpp \
           main.cpp \
           mainwindow.cpp \
           Kdb3Database.cpp \
           lib/SecString.cpp \
           crypto/twoclass.cpp \
           crypto/twofish.cpp \
           crypto/blowfish.cpp \
           crypto/sha1.cpp \
           import/Import.cpp \
           import/Import_PwManager.cpp \
           import/Import_KeePassX_Xml.cpp \
           export/Export_Txt.cpp \
           export/Export_KeePassX_Xml.cpp \
           export/Export.cpp \
           import/Import_KWalletXml.cpp \
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
           dialogs/CollectEntropyDlg.cpp \
           dialogs/CustomizeDetailViewDlg.cpp \
           dialogs/CalendarDlg.cpp \
           dialogs/ExpiredEntriesDlg.cpp \
#           dialogs/TrashCanDlg.cpp \
           lib/random.cpp \
           Database.cpp \
           lib/tools.cpp \
           lib/GroupView.cpp \
           lib/EntryView.cpp \
           lib/FileDialogs.cpp \
           crypto/arcfour.cpp \
           lib/ShortcutWidget.cpp \
           crypto/sha256.cpp \
           crypto/yarrow.cpp \
           lib/WaitAnimationWidget.cpp \
           KpxConfig.cpp \
           dialogs/AddBookmarkDlg.cpp \
           lib/bookmarks.cpp \
           dialogs/ManageBookmarksDlg.cpp \
	crypto/aescrypt.c \
	crypto/aeskey.c \
	crypto/aes_modes.c \
	crypto/aestab.c

PRECOMPILED_HEADER = keepassx.h

RESOURCES += res/resources.qrc
