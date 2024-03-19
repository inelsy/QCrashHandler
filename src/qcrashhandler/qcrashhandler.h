#pragma once

#include <QObject>
#include <QtCore/QString>

namespace Breakpad {
class CrashHandlerPrivate;
class CrashHandler : QObject
{
public:
	static CrashHandler* instance();
	void Init(QString reportPath);


private:
	CrashHandler();
	~CrashHandler();
	Q_DISABLE_COPY(CrashHandler)
	CrashHandlerPrivate* d;
};
} // namespace Breakpad
