#include "qtdumper.h"

#include <QDir>
#include <QStandardPaths>

#include "src/qcrashhandler/qcrashhandler.h"

QtDumper::QtDumper(QString *reportPath, bool deleteDumpAfterUploadFlag, QObject *parent)
	: QObject{parent}
{
	QDir dumpsDir = createDumpsDir(reportPath);

	//BREAKPAD INIT
	Breakpad::CrashHandler::instance()->Init(dumpsDir.absolutePath());

	//
	setDeleteDumpAfterUploadFlag(deleteDumpAfterUploadFlag);
	checkAndUploadDump(dumpsDir);
}

QDir QtDumper::createDumpsDir(QString *dumpsDirPath)
{
	QDir dumpsDir;
	if (dumpsDirPath == nullptr) {
		dumpsDir = QDir(QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).first());
		if (dumpsDir.mkpath("crashdumps")) {
			dumpsDir.cd("crashdumps");
		}
	} else {
		dumpsDir = QDir(*dumpsDirPath);
	}
	return dumpsDir;
}

void QtDumper::checkAndUploadDump(QDir &dumpsDir)
{
	QStringList dumpList = getDumpfileList(dumpsDir);

	if (!dumpList.isEmpty()) {
		if (dumpUploader.uploadfile(dumpsDir.absoluteFilePath(dumpList.first()))) {
			deleteDump(dumpsDir, dumpList.first());
			//for multiupload:
			//				dumps.pop_back();
			//and create loop
		}
	}
}

QStringList QtDumper::getDumpfileList(QDir &dumpsDir)
{
	dumpsDir.setNameFilters(QStringList("*.dmp"));
	dumpsDir.setSorting(QDir::Time);
	return dumpsDir.entryList();
}

void QtDumper::deleteDump(QDir &dumpsDir, QString &dumpPath)
{
	if (deleteDumpAfterUploadFlag) {
		if (dumpsDir.remove(dumpPath)) {
			//qDebug()<< "dump removed";
		}
	}
}

bool QtDumper::getDeleteDumpAfterUploadFlag()
{
	return deleteDumpAfterUploadFlag;
}

void QtDumper::setDeleteDumpAfterUploadFlag(bool value)
{
	deleteDumpAfterUploadFlag = value;
}

QtDumper::~QtDumper()
{
}
