#ifndef QTDUMPER_H
#define QTDUMPER_H

#include <QObject>

//#include "src/dumpuploader/connectiontype.h"
#include "src/dumpuploader/dumpuploader.h"
#include <optional>

class QDumper : public QObject
{
	Q_OBJECT
public:
	explicit QDumper(std::optional<QString> *dirToDumps, bool deleteDumpAfterUpload = true, QObject *parent = nullptr);
	void setDeleteDumpAfterUploadFlag(bool value);
	bool getDeleteDumpAfterUploadFlag() const;
	~QDumper();

private:
	DumpUploader dumpUploader;

	QStringList createDumpfileList(QDir &dumpsDir);
	QDir createDumpsDir(std::optional<QString> *dumpsDirPath);

	void checkAndUploadDump(QDir &dumpsDir);
	void deleteDump(QDir &dumpsDir, const QString &dumpPath);

	bool deleteDumpAfterUploadFlag;
};

#endif // QTDUMPER_H
