#ifndef CONNECTIONTYPE_H
#define CONNECTIONTYPE_H

#include "../qwebdav/qwebdavdumper.h"

struct ConnectionConfig
{
	QWebdavDumper::QWebdavConnectionType connectionType = QWebdavDumper::HTTPS;
	QString host = "inls.freemyip.com";
	QString rootPath = "/";
	QString username = "dumper";
	QString password = "CrashieDumpington";
	int short port = 443;
	QString davEntryPoint = "/remote.php/webdav";

#if defined(Q_OS_WIN32)
	QString updatesDumpRemotePath = "/crashdumps/windows/dumps/";
#elif defined(Q_OS_LINUX)
	const QString updatesDumpRemotePath = "/crashdumps/linux/dumps/";
#endif
};

#endif // CONNECTIONTYPE_H
