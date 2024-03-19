#ifndef QTDUMPER_H
#define QTDUMPER_H

#include <QObject>

#include "src/dumpuploader/dumpuploader.h"


class QtDumper : public QObject
{
	Q_OBJECT
public:
	explicit QtDumper(QString *dirToDumps = nullptr, bool deleteDumpAfterUpload = true, QObject *parent = nullptr);
	void setDeleteDumpAfterUploadFlag(bool value);
	bool getDeleteDumpAfterUploadFlag();
	~QtDumper();

private:
	DumpUploader dumpUploader;

	QStringList getDumpfileList(QDir &dumpsDir);
	QDir createDumpsDir(QString *dumpsDirPath);

	void checkAndUploadDump(QDir &dumpsDir);
	void deleteDump(QDir &dumpsDir, QString &dumpPath);

	bool deleteDumpAfterUploadFlag;
};

#endif // QTDUMPER_H
