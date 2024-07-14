#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "logging.h"

namespace logging
{
	void logMsg(const char* file, const char* msg, ...)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 -4, msg, (char*)(&msg +1));
		gDLL->logMsg(file, buf, false, false);
#ifdef _DEBUG
		strcat(buf, "\n");
		OutputDebugString(buf);
#endif
	}

	void logMsgW(const char* file, const wchar_t* msg, ...)
	{
		static wchar_t buf[2048];
		_vsnwprintf(buf, 2048 -4, msg, (char*)(&msg +1));
		static char buf2[2048];
		wcstombs(buf2, buf, 2048 -4);
		gDLL->logMsg(file, buf2, false, false);
#ifdef _DEBUG
		strcat(buf2, "\n");
		OutputDebugString(buf2);
#endif
	}
}
