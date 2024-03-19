QT     += core network xml
TEMPLATE = app

CONFIG += c++17
CONFIG += console
CONFIG *= force_debug_info



DEFINES += QWEBDAV_LIBRARY

#DEFINES += UPLOAD_DUMPS #for upload dumps in debug build

include($$PWD/src/qpreprocess.pri)
include($$PWD/src//qcrashhandler/qcrashhandler.pri)

SOURCES += $$PWD/qtdumper.cpp \
    $$PWD/src/dumpuploader/dumpuploader.cpp \
    $$PWD/src/qtwebdav/qnaturalsort.cpp \
    $$PWD/src/qtwebdav/qwebdav.cpp \
    $$PWD/src/qtwebdav/qwebdavdirparser.cpp \
    $$PWD/src/qtwebdav/qwebdavitem.cpp



HEADERS +=  $$PWD/qtdumper.h \
    $$PWD/src/dumpuploader/dumpuploader.h \
    $$PWD/src/qtwebdav/qnaturalsort.h \
    $$PWD/src/qtwebdav/qwebdav.h \
    $$PWD/src/qtwebdav/qwebdav_global.h \
    $$PWD/src/qtwebdav/qwebdavdirparser.h \
    $$PWD/src/qtwebdav/qwebdavitem.h

include($$PWD/src/qpostprocess.pri)

