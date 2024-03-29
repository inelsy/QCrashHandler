#include "qdumper.h"

#include <QDir>
#include <QStandardPaths>

#include "src/qcrashhandler/qcrashhandler.h"

QDumper::QDumper(std::optional<QString> *reportPath, bool dDAUFlag, ConnectionConfig config, QObject *parent)
    : QObject{parent}
{
    dumpUploader = new DumpUploader(config, this);

    QDir dumpsDir = createDumpsDir(reportPath);
    Breakpad::CrashHandler::instance()->Init(dumpsDir.absolutePath());

    deleteDumpAfterUploadFlag = dDAUFlag;
    checkAndUploadDump(dumpsDir);
}

QDir QDumper::createDumpsDir(std::optional<QString> *dumpsDirPath)
{
	QDir dumpsDir;
	if (dumpsDirPath) {
		dumpsDir = QDir(dumpsDirPath->value());
		return dumpsDir;
	}

	dumpsDir = QDir(QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).constFirst());
	if (dumpsDir.mkpath("crashdumps")) {
		dumpsDir.cd("crashdumps");
	}

	return dumpsDir;
}

void QDumper::checkAndUploadDump(QDir &dumpsDir)
{
    QStringList dumpList = createDumpfileList(dumpsDir);

    if (dumpList.isEmpty()) {
        return;
    }

    if (dumpUploader->uploadfile(dumpsDir.absoluteFilePath(dumpList.constFirst()))) {
        if (deleteDumpAfterUploadFlag) {
            deleteDump(dumpsDir, dumpList.constFirst());
        }
    }
}

QStringList QDumper::createDumpfileList(QDir &dumpsDir)
{
	dumpsDir.setNameFilters(QStringList("*.dmp"));
	dumpsDir.setSorting(QDir::Time);
	return dumpsDir.entryList();
}

void QDumper::deleteDump(QDir &dumpsDir, const QString &dumpPath)
{
	if (deleteDumpAfterUploadFlag) {
		dumpsDir.remove(dumpPath);
	}
}

QDumper::~QDumper()
{
}
