#include "dumpuploader.h"

DumpUploader::DumpUploader(QObject *parent)
	: QObject{parent}
{
}

DumpUploader::~DumpUploader()
{
}

bool DumpUploader::uploadfile(QString dumpfilePath)
{
	//check openssl
	//		qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
	//if false "" "" and you have error: "qt.network.ssl: No functional TLS backend was found"

	//TRY DISABLING THE SHADOW BUILD

	w.setConnectionSettings(QWebdav::HTTPS, host, "/", crashdumperUser, crashdumperPass, port);
	QFile dumpfile(dumpfilePath);
	if (!dumpfile.open(QIODevice::ReadOnly)) {
		return false;
	}
	QByteArray data = dumpfile.readAll();
	dumpfile.close();

	QFileInfo file(dumpfilePath);

	QString name = file.fileName();
	auto res = w.put(davEntryPoint + updatesDumpRemotePath + name, data);

#ifdef DEBUG_WEBDAV
	qDebug() << "URL: " << res->url();
	qDebug() << "DUMPNAME: " << name;
#endif

	connect(res, &QNetworkReply::uploadProgress, this, [](/*qint64 sent,*/ qint64 total) {
		if (total < 1)
			return;
#ifdef DEBUG_WEBDAV
		int percent = 100 * static_cast<int>(sent / total);
		qDebug() << "got uploadProgress" << percent;
#endif
		//			qDebug() << (percent);
	});
	connect(res, &QNetworkReply::downloadProgress, this, [](/*qint64 rec,*/ qint64 total) {
		if (total < 1)
			return;
#ifdef DEBUG_WEBDAV
		int percent = 100 * (float(rec) / total);
		qDebug() << "got downloadProgress";
#endif
		//			qDebug() << (percent);
	});
#ifdef DEBUG_WEBDAV
	connect(res, &QNetworkReply::encrypted, []() {
		qDebug() << "got encrypted";
	});
	connect(res, &QNetworkReply::metaDataChanged, []() {
		qDebug() << "got metaDataChanged";
	});
	connect(res, &QNetworkReply::preSharedKeyAuthenticationRequired, []() {
		qDebug() << "got preSharedKeyAuthenticationRequired";
	});
	connect(res, &QNetworkReply::redirectAllowed, []() {
		qDebug() << "got redirectAllowed";
	});
	connect(res, &QNetworkReply::redirected, []() {
		qDebug() << "got redirected";
	});
	connect(res, &QNetworkReply::requestSent, []() {
		qDebug() << "got requestSent";
	});
	connect(res, &QNetworkReply::socketStartedConnecting, []() {
		qDebug() << "got socketStartedConnecting";
	});
	connect(res, &QNetworkReply::sslErrors, []() {
		qDebug() << "got sslErrors";
	});
	connect(res, &QNetworkReply::readyRead, this, []() mutable {
		qDebug() << "got readyRead";
	});
#endif

	connect(res, &QNetworkReply::finished, this, [res]() mutable {
#ifdef DEBUG_WEBDAV
		qDebug() << "got finished";

#endif
		if (res->error() == QNetworkReply::NoError) {
			qDebug() << "DUMPFile uploaded successfully";
		} else {
			qDebug() << "Error uploading file:" << res->errorString();
		}
		res->deleteLater();
		//			qDebug() << (res->error());
	});
	return true;
}
