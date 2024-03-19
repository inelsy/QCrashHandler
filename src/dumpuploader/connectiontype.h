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
	int short port = 443;
	QString davEntryPoint;

#if defined(Q_OS_WIN32)
	QString updatesDumpRemotePath;
#elif defined(Q_OS_LINUX)
	const QString updatesDumpRemotePath;
#endif
};

#endif // CONNECTIONTYPE_H
