#include "qdumper.h"

#include <QDir>
#include <QStandardPaths>

#include "src/qcrashhandler/qcrashhandler.h"

QDumper::QDumper(QString reportPath, bool dDAUFlag, ConnectionConfig config, QObject *parent)
    : QObject{parent}
{
    dumpUploader = new DumpUploader(config, this);

    QDir dumpsDir = createDumpsDir(reportPath);
    Breakpad::CrashHandler::instance()->Init(dumpsDir.absolutePath());

    deleteDumpAfterUploadFlag = dDAUFlag;
    checkAndUploadDump(dumpsDir);
}

QDir QDumper::createDumpsDir(QString dumpsDirPath)
{
    QDir dumpsDir;
    if (!dumpsDirPath.isEmpty()) {
        dumpsDir = QDir(dumpsDirPath);
        return dumpsDir;
    }

    dumpsDir = QDir(QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).constFirst());
    if (dumpsDir.mkpath("crashdumps")) {
        dumpsDir.cd("crashdumps");
    }

    return dumpsDir;
}

void QDumper::checkAndUploadDump(QDir dumpsDir)
{
    QStringList dumpList = createDumpfileList(dumpsDir);

    if (dumpList.isEmpty()) {
        return;
    }

    QString dumpPath = dumpList.constFirst();
    dumpUploader->uploadfile(dumpsDir.absoluteFilePath(dumpPath),
                             [this, dumpsDir, dumpPath]() mutable {
                                 if (deleteDumpAfterUploadFlag) {
                                     dumpsDir.remove(dumpPath);
                                 }
                             });
}

QStringList QDumper::createDumpfileList(QDir &dumpsDir)
{
	dumpsDir.setNameFilters(QStringList("*.dmp"));
	dumpsDir.setSorting(QDir::Time);
	return dumpsDir.entryList();
}

QDumper::~QDumper()
{
}
