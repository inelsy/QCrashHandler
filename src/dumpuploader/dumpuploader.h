#ifndef DUMPUPLOADER_H
#define DUMPUPLOADER_H

#include "../qtwebdav/qwebdav.h"
#include <QObject>


class DumpUploader : public QObject
{
	Q_OBJECT

public:
	explicit DumpUploader(QObject *parent = nullptr);
	QWebdav w;
	bool uploadfile(QString dumpfilePath);
	~DumpUploader();

private:
	QString crashdumperUser = QStringLiteral("dumper");
	QString crashdumperPass = QStringLiteral("CrashieDumpington");

	const short int port = 443;
	const QString host = "inls.freemyip.com";

	const QString davEntryPoint = "/remote.php/webdav";

#if defined(Q_OS_WIN32)
	const QString updatesDumpRemotePath = QStringLiteral("/crashdumps/windows/dumps/");
#elif defined(Q_OS_LINUX)
	const QString updatesDumpRemotePath = QStringLiteral("/crashdumps/linux/dumps/");
#endif
};

#endif // DUMPUPLOADER_H
