
INCLUDEPATH +=	/usr/lib/kde4/include
TEMPLATE = lib
CONFIG += plugin release
HEADERS += keepassx-kde.h
SOURCES += keepassx-kde.cpp
MOC_DIR = build/moc
OBJECTS_DIR = build/
LIBS+=-L/usr/lib/kde4/lib -lkio -lkdecore
