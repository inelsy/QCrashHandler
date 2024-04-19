#ifndef DUMPUPLOADER_H
#define DUMPUPLOADER_H

#include "../qwebdav/qwebdavdumper.h"
#include <QObject>

#include "connectiontype.h"

class DumpUploader : public QObject
{
	Q_OBJECT

public:
	explicit DumpUploader(QObject *parent = nullptr, ConnectionConfig Config = ConnectionConfig());

	bool uploadfile(QString dumpfilePath);
	~DumpUploader();

private:
	QWebdavDumper webdav;

	ConnectionConfig WebDavConfig;
};

#endif // DUMPUPLOADER_H
