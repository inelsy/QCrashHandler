#include "qcrashhandler.h"

#include <QtCore/QProcess>


#if defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif

//#define DEBUG_WEBDAV
namespace Breakpad {
/************************************************************************/
/* CrashHandlerPrivate                                                  */
/************************************************************************/
class CrashHandlerPrivate
{
public:
	CrashHandlerPrivate()
	{
		pHandler = NULL;
	}

	~CrashHandlerPrivate()
	{
		delete pHandler;
	}

	void InitCrashHandler(const QString& dumpPath);
	static google_breakpad::ExceptionHandler* pHandler;
};

google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;

void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath)
{
	if (pHandler != NULL) {
		return;
	}

#if defined(Q_OS_WIN32)
		std::wstring pathAsStr = (const wchar_t*) dumpPath.utf16();
		pHandler = new google_breakpad::ExceptionHandler(
			pathAsStr,
			/*FilterCallback*/ 0,
			/*MinidumpCallback*/ 0,
			/*context*/ 0,
			true);

		//for dump with full memory storage
		//		pHandler = new google_breakpad::ExceptionHandler(pathAsStr, 0, 0, 0, google_breakpad::ExceptionHandler::HANDLER_ALL, MINIDUMP_TYPE::MiniDumpWithFullMemory, HANDLE(), 0);

#elif defined(Q_OS_LINUX)
	std::string pathAsStr = dumpPath.toStdString();
	google_breakpad::MinidumpDescriptor md(pathAsStr);
	pHandler = new google_breakpad::ExceptionHandler(
		md,
		/*FilterCallback*/ 0,
		/*MinidumpCallback*/ 0,
		/*context*/ 0,
		true,
		-1);
#endif
}

	/************************************************************************/
	/* CrashHandler                                                         */
	/************************************************************************/
	CrashHandler* CrashHandler::instance()
	{
		static CrashHandler globalHandler;
		return &globalHandler;
	}

	CrashHandler::CrashHandler()
	{
		d = new CrashHandlerPrivate();
	}

	CrashHandler::~CrashHandler()
	{
		delete d;
	}

	void CrashHandler::Init(QString reportPath)
	{
		d->InitCrashHandler(reportPath);
	}


	} // namespace Breakpad
