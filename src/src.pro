
CONFIG = qt uic resources thread stl warn_off
QT += xml

DEPENDPATH += crypto dialogs export forms import lib translations res
INCLUDEPATH += . lib crypto plugins/interfaces export import dialogs

MOC_DIR = ../build/moc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build
RCC_DIR = ../build/rcc

isEqual(DEBUG,1) {
  CONFIG += debug
}
else {
  CONFIG += release
}

win32:QMAKE_WIN32 = 1

#-------------------------------------------------------------------------------
#   Platform Specific: Unix (except MacOS X)
#-------------------------------------------------------------------------------
unix : !macx : !isEqual(QMAKE_WIN32,1) {
    isEmpty(PREFIX):PREFIX = /usr
    !isEqual(AUTOTYPE,0) {
        DEFINES += AUTOTYPE
        !isEqual(GLOBAL_AUTOTYPE,0) {
            DEFINES += GLOBAL_AUTOTYPE
        }
    }
    TARGET = ../bin/keepassx
    TARGET.path = $${PREFIX}/bin
    data.path = $${PREFIX}/share
    pixmaps.files = ../share/pixmaps/*
    pixmaps.path = $${PREFIX}/share/pixmaps
    desktop.files = ../share/applications/*
    desktop.path = $${PREFIX}/share/applications
    INSTALLS += pixmaps desktop
    contains(DEFINES,AUTOTYPE) {
        LIBS += -lXtst
        SOURCES += lib/HelperX11.cpp lib/AutoType_X11.cpp
        HEADERS += lib/HelperX11.h
    }
    contains(DEFINES,GLOBAL_AUTOTYPE) {
        SOURCES += Application_X11.cpp
        HEADERS += Application_X11.h
    }
    isEqual(BUILD_FOR_LSB,1) {
       QMAKE_CXX = lsbcc
    }
}


#-------------------------------------------------------------------------------
#   Platform Specific: MacOS X
#-------------------------------------------------------------------------------
macx {
    isEmpty(PREFIX):PREFIX = /Applications
    TARGET = ../bin/KeePassX
    TARGET.path = $${PREFIX}
    data.path = Contents/Resources
    isEmpty(QT_FRAMEWORK_DIR) : QT_FRAMEWORK_DIR = /Library/Frameworks
    private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtCore.framework
    private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtGui.framework
    private_frameworks.files += $${QT_FRAMEWORK_DIR}/QtXml.framework
    private_frameworks.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += data private_frameworks
    LIBS += -framework CoreFoundation
    ICON = ../share/macx_bundle/icon.icns
    CONFIG += app_bundle
    isEqual(ARCH,UNIVERSAL) : CONFIG += x86 ppc
    isEqual(ARCH,INTEL) : CONFIG += x86
    isEqual(ARCH,PPC) : CONFIG += ppc
}

#-------------------------------------------------------------------------------
#   Platform Specific: Windows
#-------------------------------------------------------------------------------
isEqual(QMAKE_WIN32,1) {
    CONFIG += windows
    isEmpty(PREFIX):PREFIX = "C:/Program files/KeePassX"
    TARGET = ../bin/KeePassX
    TARGET.path = $${PREFIX}
    data.path = $${PREFIX}/share
    RC_FILE = ../share/ico/keepassx.rc
    QMAKE_LINK_OBJECT_SCRIPT = $${OBJECTS_DIR}/$${QMAKE_LINK_OBJECT_SCRIPT}
}


data.files += ../share/keepassx
INSTALLS += TARGET data

contains(DEFINES,GLOBAL_AUTOTYPE) {
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
#           lib/KdePlugin.h \
           lib/AutoType.h \
           lib/FileDialogs.h \
           lib/ShortcutWidget.h \
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
           crypto/yarrow.h \
           crypto/blowfish.h \
           crypto/sha1.h \
           lib/WaitAnimationWidget.h \
           plugins/interfaces/IFileDialog.h \
#           plugins/interfaces/IKdeInit.h \
#           plugins/interfaces/IGnomeInit.h \
           plugins/interfaces/IIconTheme.h \
           KpxConfig.h \
#           KpxFirefox.h \
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
#           lib/KdePlugin.cpp \
           lib/GroupView.cpp \
           lib/EntryView.cpp \
           lib/FileDialogs.cpp \
           crypto/arcfour.cpp \
           lib/KpFileIconProvider.cpp \
           lib/ShortcutWidget.cpp \
           crypto/aescrypt.c \
           crypto/aeskey.c \
           crypto/aestab.c \
           crypto/aes_modes.c \
           crypto/sha256.cpp \
           crypto/yarrow.cpp \
           lib/WaitAnimationWidget.cpp \
           KpxConfig.cpp \
#           KpxFirefox.cpp \
           dialogs/AddBookmarkDlg.cpp \
           lib/bookmarks.cpp \
           dialogs/ManageBookmarksDlg.cpp

RESOURCES += res/resources.qrc
