#ifndef DUMPUPLOADER_H
#define DUMPUPLOADER_H

#include "../qwebdav/qwebdavdumper.h"
#include <QObject>

#include "connectiontype.h"

class DumpUploader : public QObject
{
	Q_OBJECT

public:
	explicit DumpUploader(ConnectionConfig Config = ConnectionConfig(), QObject *parent = nullptr );

	void uploadfile(QString dumpfilePath, std::function<void()> deleteDump);
	~DumpUploader();

private:
	QWebdavDumper webdav;

	ConnectionConfig WebDavConfig;
};

#endif // DUMPUPLOADER_H
