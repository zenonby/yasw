QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    amalgamation/sqlite3.c \
    src/SqliteDb.cpp \
    src/SqliteRecordset.cpp \
    src/SqliteCommand.cpp \
    src/SqliteTransaction.cpp

HEADERS += \
    amalgamation/sqlite3.h \
    include/yasw/SqliteDb.h \
    include/yasw/SqliteRecordset.h \
    include/yasw/SqliteCommand.h \
    include/yasw/SqliteTransaction.h \
    include/yasw/SqliteExceptions.h

INCLUDEPATH += amalgamation
INCLUDEPATH += include/yasw

DESTDIR = $$PWD

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
