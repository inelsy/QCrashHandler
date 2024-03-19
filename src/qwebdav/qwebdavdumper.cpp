/****************************************************************************
** QWebDAV Library (qwebdavlib) - LGPL v2.1
**
** HTTP Extensions for Web Distributed Authoring and Versioning (WebDAV)
** from June 2007
**      http://tools.ietf.org/html/rfc4918
**
** Web Distributed Authoring and Versioning (WebDAV) SEARCH
** from November 2008
**      http://tools.ietf.org/html/rfc5323
**
** Missing:
**      - LOCK support
**      - process WebDAV SEARCH responses
**
** Copyright (C) 2012 Martin Haller <martin.haller@rebnil.com>
** for QWebDAV library (qwebdavlib) version 1.0
**      https://github.com/mhaller/qwebdavlib
**
** Copyright (C) 2012 Timo Zimmermann <meedav@timozimmermann.de>
** for portions from QWebdav plugin for MeeDav (LGPL v2.1)
**      http://projects.developer.nokia.com/meedav/
**
** Copyright (C) 2009-2010 Corentin Chary <corentin.chary@gmail.com>
** for portions from QWebdav - WebDAV lib for Qt4 (LGPL v2.1)
**      http://xf.iksaif.net/dev/qwebdav.html
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** for naturalCompare() (LGPL v2.1)
**      http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/dialogs/qfilesystemmodel.cpp
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
** http://www.gnu.org/licenses/lgpl-2.1-standalone.html
**
****************************************************************************/

#include "qwebdavdumper.h"

QWebdavDumper::QWebdavDumper(QObject* parent)
	: QNetworkAccessManager(parent)
	, m_rootPath()
	, m_username()
	, m_password()
	, m_baseUrl()
	, m_currentConnectionType(QWebdavDumper::HTTP)
	, m_authenticator_lastReply(nullptr)
	, m_sslCertDigestMd5("")
	, m_sslCertDigestSha1("")

{
	qRegisterMetaType<QNetworkReply*>("QNetworkReply*");
	connect(this, &QWebdavDumper::finished, this, &QWebdavDumper::replyFinished);
	connect(this, &QWebdavDumper::authenticationRequired, this, &QWebdavDumper::provideAuthenication);
}

QWebdavDumper::~QWebdavDumper()
{
}

QString QWebdavDumper::hostname() const
{
    return m_baseUrl.host();
}

int QWebdavDumper::port() const
{
    return m_baseUrl.port();
}

QString QWebdavDumper::rootPath() const
{
    return m_rootPath;
}

QString QWebdavDumper::username() const
{
    return m_username;
}

QString QWebdavDumper::password() const
{
    return m_password;
}

QWebdavDumper::QWebdavConnectionType QWebdavDumper::connectionType() const
{
    return m_currentConnectionType;
}

bool QWebdavDumper::isSSL() const
{
	return (m_currentConnectionType == QWebdavDumper::HTTPS);
}

void QWebdavDumper::setConnectionSettings(const QWebdavConnectionType connectionType,
										  const QString& hostname,
										  const QString& rootPath,
										  const QString& username,
										  const QString& password,
										  int port,
										  const QString& sslCertDigestMd5,
										  const QString& sslCertDigestSha1)
{
    m_rootPath = rootPath;

    if ((m_rootPath.size()>0) && (m_rootPath.endsWith("/")))
        m_rootPath.chop(1);

	QString uriScheme;
	switch (connectionType) {
		case QWebdavDumper::HTTP:
			uriScheme = "http";
			break;
		case QWebdavDumper::HTTPS:
			uriScheme = "https";
			break;
	}

	m_currentConnectionType = connectionType;

	m_baseUrl.setScheme(uriScheme);
    m_baseUrl.setHost(hostname);
    m_baseUrl.setPath(rootPath);

	if (port != 0) {
		// use user-defined port number
		if (!(((port == 80) && (m_currentConnectionType == QWebdavDumper::HTTP)) || ((port == 443) && (m_currentConnectionType == QWebdavDumper::HTTPS))))
			m_baseUrl.setPort(port);
	}

	m_sslCertDigestMd5 = hexToDigest(sslCertDigestMd5);
    m_sslCertDigestSha1 = hexToDigest(sslCertDigestSha1);

    m_username = username;
    m_password = password;
}

void QWebdavDumper::acceptSslCertificate(const QString& sslCertDigestMd5,
										 const QString& sslCertDigestSha1)
{
    m_sslCertDigestMd5 = hexToDigest(sslCertDigestMd5);
    m_sslCertDigestSha1 = hexToDigest(sslCertDigestSha1);
}

void QWebdavDumper::replyReadyRead()
{
    auto reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply->bytesAvailable() < 256000)
        return;

    QIODevice* dataIO = m_inDataDevices.value(reply, 0);
    if(dataIO == nullptr)
        return;
    dataIO->write(reply->readAll());
}

void QWebdavDumper::replyFinished(QNetworkReply* reply)
{
#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::replyFinished()";
#endif
	disconnect(reply, &QNetworkReply::redirectAllowed, this, &QWebdavDumper::replyReadyRead);
	disconnect(reply, &QNetworkReply::errorOccurred, this, &QWebdavDumper::replyError);

	QIODevice* dataIO = m_inDataDevices.value(reply, 0);
	if (dataIO != 0) {
		dataIO->write(reply->readAll());
        static_cast<QFile*>(dataIO)->flush();
        dataIO->close();
        delete dataIO;
	}
	m_inDataDevices.remove(reply);

    QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
}

void QWebdavDumper::replyDeleteLater(QNetworkReply* reply)
{
#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::replyDeleteLater()";
#endif

    QIODevice *outDataDevice = m_outDataDevices.value(reply, 0);
    if (outDataDevice!=nullptr)
        outDataDevice->deleteLater();
    m_outDataDevices.remove(reply);
}

void QWebdavDumper::replyError(QNetworkReply::NetworkError)
{
    auto reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply==nullptr)
        return;

#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::replyError()  reply->url() == " << reply->url().toString(QUrl::RemoveUserInfo);
#endif

    if ( reply->error() == QNetworkReply::OperationCanceledError) {
        QIODevice* dataIO = m_inDataDevices.value(reply, 0);
        if (dataIO!=nullptr) {
            delete dataIO;
            m_inDataDevices.remove(reply);
        }
        return;
    }

    emit errorChanged(reply->errorString());
}

void QWebdavDumper::provideAuthenication(QNetworkReply* reply, QAuthenticator* authenticator)
{
#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::authenticationRequired()";
    QVariantHash opts = authenticator->options();
    QVariant optVar;
    foreach(optVar, opts) {
        qDebug() << "QWebdav::authenticationRequired()  option == " << optVar.toString();
    }
#endif

    if (reply == m_authenticator_lastReply) {
        reply->abort();
        emit errorChanged("WebDAV server requires authentication. Check WebDAV share settings!");
        reply->deleteLater();
        reply=nullptr;
    }

    m_authenticator_lastReply = reply;

    authenticator->setUser(m_username);
    authenticator->setPassword(m_password);
}

void QWebdavDumper::sslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::sslErrors()   reply->url == " << reply->url().toString(QUrl::RemoveUserInfo);
#endif

    QSslCertificate sslcert = errors[0].certificate();

    if ( ( sslcert.digest(QCryptographicHash::Md5) == m_sslCertDigestMd5 ) &&
         ( sslcert.digest(QCryptographicHash::Sha1) == m_sslCertDigestSha1) )
    {
        // user accepted this SSL certifcate already ==> ignore SSL errors
        reply->ignoreSslErrors();
    } else {
        // user has to check the SSL certificate and has to accept manually
        emit checkSslCertifcate(errors);
        reply->abort();
    }
}

QString QWebdavDumper::digestToHex(const QByteArray& input)
{
    QByteArray inputHex = input.toHex();

    QString result = "";
    for (int i = 0; i<inputHex.size(); i+=2) {
        result.append(inputHex.at(i));
        result.append(inputHex.at(i+1));
        result.append(":");
    }
    result.chop(1);
    result = result.toUpper();

    return result;
}

QByteArray QWebdavDumper::hexToDigest(const QString& input)
{
    QByteArray result;
    int i = 2;
    int l = static_cast<int>(input.size());
    result.append(input.left(2).toLatin1());
    while ((i<l) && (input.at(i) == ':')) {
        ++i;
        result.append(input.mid(i,2).toLatin1());
        i+=2;
    }
    return QByteArray::fromHex(result);
}

QString QWebdavDumper::absolutePath(const QString& relPath)
{
    return QString(m_rootPath + relPath);

}

QNetworkReply* QWebdavDumper::createRequest(const QString& method, QNetworkRequest& req, QIODevice* outgoingData)
{
	if (outgoingData != nullptr && outgoingData->size() != 0) {
		req.setHeader(QNetworkRequest::ContentLengthHeader, outgoingData->size());
		req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");
	}

#ifdef DEBUG_WEBDAV
    qDebug() << " QWebdav::createRequest1";
    qDebug() << "   " << method << " " << req.url().toString();
    QList<QByteArray> rawHeaderList = req.rawHeaderList();
    QByteArray rawHeaderItem;
    foreach(rawHeaderItem, rawHeaderList) {
        qDebug() << "   " << rawHeaderItem << ": " << req.rawHeader(rawHeaderItem);
    }
#endif

    return sendCustomRequest(req, method.toLatin1(), outgoingData);
}

QNetworkReply* QWebdavDumper::createRequest(const QString& method, QNetworkRequest& req, const QByteArray& outgoingData)
{
    auto dataIO = new QBuffer;
    dataIO->setData(outgoingData);
    dataIO->open(QIODevice::ReadOnly);

#ifdef DEBUG_WEBDAV
    qDebug() << " QWebdav::createRequest2";
    qDebug() << "   " << method << " " << req.url().toString();
    QList<QByteArray> rawHeaderList = req.rawHeaderList();
    QByteArray rawHeaderItem;
    foreach(rawHeaderItem, rawHeaderList) {
        qDebug() << "   " << rawHeaderItem << ": " << req.rawHeader(rawHeaderItem);
    }
#endif

    QNetworkReply* reply = createRequest(method, req, dataIO);
    m_outDataDevices.insert(reply, dataIO);
    return reply;
}

QNetworkReply* QWebdavDumper::list(const QString& path)
{
#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::list() path = " << path;
#endif
    return list(path, 1);
}

QNetworkReply* QWebdavDumper::list(const QString& path, int depth)
{
	QWebdavDumper::PropNames query;
	QStringList props;

	// Small set of properties
	// href in response contains also the name
	// e.g. /container/front.html
	props << "getlastmodified";         // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getlastmodified
    // e.g. Mon, 12 Jan 1998 09:25:56 GMT
    props << "getcontentlength";        // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontentlength
    // e.g. "4525"
    props << "resourcetype";            // http://www.webdav.org/specs/rfc4918.html#PROPERTY_resourcetype
    // e.g. "collection" for a directory

    // Following properties are available as well.
    //props << "creationdate";          // http://www.webdav.org/specs/rfc4918.html#PROPERTY_creationdate
    // e.g. "1997-12-01T18:27:21-08:00"
    //props << "displayname";           // http://www.webdav.org/specs/rfc4918.html#PROPERTY_displayname
    // e.g. "Example HTML resource"
    //props << "getcontentlanguage";    // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontentlanguage
    // e.g. "en-US"
    //props << "getcontenttype";        // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getcontenttype
    // e.g "text/html"
    //props << "getetag";               // http://www.webdav.org/specs/rfc4918.html#PROPERTY_getetag
    // e.g. "zzyzx"

    // Additionally, there are also properties for locking

    query["DAV:"] = props;

    return propfind(path, query, depth);
}

QNetworkReply* QWebdavDumper::search(const QString& path, const QString& q)
{
    QByteArray query = "<?xml version=\"1.0\"?>\r\n";
    query.append( "<D:searchrequest xmlns:D=\"DAV:\">\r\n" );
    query.append( q.toUtf8() );
    query.append( "</D:searchrequest>\r\n" );

    QNetworkRequest req;

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return this->createRequest("SEARCH", req, query);
}

QNetworkReply* QWebdavDumper::get(const QString& path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

#ifdef DEBUG_WEBDAV
//    qDebug() << "QWebdav::get() url (before) = " << req.url().toString(QUrl::RemoveUserInfo);
#endif

    req.setUrl(reqUrl);

#ifdef DEBUG_WEBDAV
//    qDebug() << "QWebdav::get() url (after) = " << req.url().toString(QUrl::RemoveUserInfo);
#endif

    return QNetworkAccessManager::get(req);
}

QNetworkReply* QWebdavDumper::get(const QString& path, QIODevice* data)
{
    return get(path, data, 0);
}

QNetworkReply* QWebdavDumper::get(const QString& path, QIODevice* data, quint64 fromRangeInBytes)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::get() url = " << req.url().toString(QUrl::RemoveUserInfo);
#endif

    if (fromRangeInBytes>0) {
        // RFC2616 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        QByteArray fromRange = "bytes=" + QByteArray::number(fromRangeInBytes) + "-";   // byte-ranges-specifier
        req.setRawHeader("Range",fromRange);
    }

	QNetworkReply* reply = QNetworkAccessManager::get(req);
	m_inDataDevices.insert(reply, data);
	connect(reply, &QNetworkReply::readyRead, this, &QWebdavDumper::replyReadyRead);
	connect(reply, &QNetworkReply::errorOccurred, this, &QWebdavDumper::replyError);

	return reply;
}

QNetworkReply* QWebdavDumper::put(const QString& path, QIODevice* data)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::put() url = " << req.url().toString(QUrl::RemoveUserInfo);
#endif

    return QNetworkAccessManager::put(req, data);
}

QNetworkReply* QWebdavDumper::put(const QString& path, const QByteArray& data)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));
	req.setUrl(reqUrl);

#ifdef DEBUG_WEBDAV
    qDebug() << "QWebdav::put() url = " << req.url().toString(QUrl::RemoveUserInfo);
#endif

    return QNetworkAccessManager::put(req, data);
}

QNetworkReply* QWebdavDumper::propfind(const QString& path, const QWebdavDumper::PropNames& props, int depth)
{
//    QByteArray query;
    QString query;

    query = R"(<?xml version="1.0" encoding="utf-8" ?>)";
    query += R"(<D:propfind xmlns:D="DAV:" >)";
    query += "<D:prop>";
    for(const auto& ns: props.keys()) {
        for(const auto& key: props[ns]) {
            if (ns == "DAV:")
                query += "<D:" + key + "/>";
            else
                query += "<" + key + " xmlns=\"" + ns + "\"/>";
        }
    }
    query += "</D:prop>";
    query += "</D:propfind>";
    return propfind(path, query.toUtf8(), depth);
}

QNetworkReply* QWebdavDumper::propfind(const QString& path, const QByteArray& query, int depth)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);
    req.setRawHeader("Depth", depth == 2 ? QString("infinity").toUtf8() : QString::number(depth).toUtf8());

    return createRequest("PROPFIND", req, query);
}

QNetworkReply* QWebdavDumper::proppatch(const QString& path, const QWebdavDumper::PropValues& props)
{
//    QByteArray query;
    QString query;

    query = R"(<?xml version="1.0" encoding="utf-8" ?>)";
    query += R"(<D:proppatch xmlns:D="DAV:" >)";
    query += "<D:prop>";
    for (const auto& ns: props.keys())
    {
        QMap < QString , QVariant >::const_iterator i;

        for (i = props[ns].constBegin(); i != props[ns].constEnd(); ++i) {
            if (ns == "DAV:") {
                query += "<D:" + i.key() + ">";
                query += i.value().toString();
                query += "</D:" + i.key() + ">" ;
            } else {
                query += "<" + i.key() + " xmlns=\"" + ns + "\">";
                query += i.value().toString();
                query += "</" + i.key() + " xmlns=\"" + ns + "\"/>";
            }
        }
    }
    query += "</D:prop>";
    query += "</D:propfind>";

    return proppatch(path, query.toUtf8());
}

QNetworkReply* QWebdavDumper::proppatch(const QString& path, const QByteArray& query)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createRequest("PROPPATCH", req, query);
}

QNetworkReply* QWebdavDumper::mkdir(const QString& path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createRequest("MKCOL", req);
}

QNetworkReply* QWebdavDumper::copy(const QString& pathFrom, const QString& pathTo, bool overwrite)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(pathFrom));

    req.setUrl(reqUrl);

    // RFC4918 Section 10.3 requires an absolute URI for destination raw header
    //  http://tools.ietf.org/html/rfc4918#section-10.3
    // RFC3986 Section 4.3 specifies the term absolute URI
    //  http://tools.ietf.org/html/rfc3986#section-4.3
    QUrl dstUrl(m_baseUrl);
    //dstUrl.setUserInfo("");
    dstUrl.setPath(absolutePath(pathTo));
    req.setRawHeader("Destination", dstUrl.toString().toUtf8());

    req.setRawHeader("Depth", "infinity");
    req.setRawHeader("Overwrite", overwrite ? "T" : "F");

    return createRequest("COPY", req);
}

QNetworkReply* QWebdavDumper::move(const QString& pathFrom, const QString& pathTo, bool overwrite)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(pathFrom));

    req.setUrl(reqUrl);

    // RFC4918 Section 10.3 requires an absolute URI for destination raw header
    //  http://tools.ietf.org/html/rfc4918#section-10.3
    // RFC3986 Section 4.3 specifies the term absolute URI
    //  http://tools.ietf.org/html/rfc3986#section-4.3
    QUrl dstUrl(m_baseUrl);
    //dstUrl.setUserInfo("");
    dstUrl.setPath(absolutePath(pathTo));
    req.setRawHeader("Destination", dstUrl.toString().toUtf8());

    req.setRawHeader("Depth", "infinity");
    req.setRawHeader("Overwrite", overwrite ? "T" : "F");

    return createRequest("MOVE", req);
}

QNetworkReply* QWebdavDumper::remove(const QString& path)
{
    QNetworkRequest req = buildRequest();

    QUrl reqUrl(m_baseUrl);
    reqUrl.setPath(absolutePath(path));

    req.setUrl(reqUrl);

    return createRequest("DELETE", req);
}

QNetworkRequest QWebdavDumper::buildRequest()
{
	QNetworkRequest req;
	if (isSSL()) {
		QString concatenated = m_username + ":" + m_password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        req.setRawHeader("Authorization", headerData.toLocal8Bit());
	}
	return req;
}
