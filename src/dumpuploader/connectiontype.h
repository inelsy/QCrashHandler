#ifndef CONNECTIONTYPE_H
#define CONNECTIONTYPE_H

#include "../qwebdav/qwebdavdumper.h"

struct ConnectionConfig
{
    QWebdavDumper::QWebdavConnectionType connectionType = QWebdavDumper::HTTPS;
    QString host;
    QString rootPath;
    QString username;
    QString password;
    int short port;
    QString davEntryPoint;
    QString updatesDumpRemotePath;
};

#endif // CONNECTIONTYPE_H
