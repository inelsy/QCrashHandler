QT     *= core network xml
TEMPLATE = app


#CONFIG += deploy #расскоментировать для сборки с отладочной информацией библиотек qt

if($$find(CONFIG, deploy)) {
    CONFIG(release, debug|release) {
        CONFIG += debug
        CONFIG += optimize_full
    } else{
        CONFIG *= force_debug_info
    }
}

DEFINES *= QWEBDAV_LIBRARY
#DEFINES += DEBUG_WEBDAV #for debuging upload files


include($$PWD/src/qpreprocess.pri)
include($$PWD/src/qcrashhandler/qcrashhandler.pri)

SOURCES += $$PWD/qdumper.cpp \
    $$PWD/src/dumpuploader/dumpuploader.cpp \
    $$PWD/src/qwebdav/qnaturalsortdumper.cpp \
    $$PWD/src/qwebdav/qwebdavdirparserdumper.cpp \
    $$PWD/src/qwebdav/qwebdavdumper.cpp \
    $$PWD/src/qwebdav/qwebdavitemdumper.cpp



HEADERS +=  $$PWD/qdumper.h \
    $$PWD/src/dumpuploader/dumpuploader.h \
    $$PWD/src/dumpuploader/connectiontype.h \
    $$PWD/src/qwebdav/qnaturalsortdumper.h \
    $$PWD/src/qwebdav/qwebdav_global.h \
    $$PWD/src/qwebdav/qwebdavdirparserdumper.h \
    $$PWD/src/qwebdav/qwebdavdumper.h \
    $$PWD/src/qwebdav/qwebdavitemdumper.h


include($$PWD/src/qpostprocess.pri)

