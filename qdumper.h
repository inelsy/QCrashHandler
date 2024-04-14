#ifndef QTDUMPER_H
#define QTDUMPER_H

#include <QObject>

#include "src/dumpuploader/connectiontype.h"
#include "src/dumpuploader/dumpuploader.h"
#include <optional>

class QDumper : public QObject
{
	Q_OBJECT
public:
    explicit QDumper(QString dirToDumps = "", bool deleteDumpAfterUpload = true, ConnectionConfig config = ConnectionConfig(), QObject *parent = nullptr);
    void setDeleteDumpAfterUploadFlag(bool value);
    bool getDeleteDumpAfterUploadFlag() const;
    ~QDumper();

private:
    DumpUploader *dumpUploader;

    QStringList createDumpfileList(QDir &dumpsDir);
    QDir createDumpsDir(QString dumpsDirPath);

    void checkAndUploadDump(QDir dumpsDir);

    bool deleteDumpAfterUploadFlag;
};

#endif // QTDUMPER_H
