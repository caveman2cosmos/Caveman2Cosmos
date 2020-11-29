#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "logging.h"

namespace logging
{
	void log_json_event(const char* type, const JsonValues& values)
	{
#ifndef FINAL_RELEASE
		picojson::value::object obj;
		for (std::vector<JsonValue>::const_iterator itr = values.values.begin(); itr != values.values.end(); ++itr)
		{
			obj[itr->name] = itr->value;
		}
		obj["type"] = picojson::value(type);
		gDLL->logMsg("EventsJson.log", picojson::value(obj).serialize().c_str());
#endif
	}

	void logMsg(const char* file, char* msg, ...)
	{
		if (GC.isXMLLogging())
		{
			static char buf[2048];
			_vsnprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			gDLL->logMsg(file, buf);
			OutputDebugString(buf);
		}
	}

	void logMsgW(const char* file, wchar_t* msg, ...)
	{
		if (GC.isXMLLogging())
		{
			static wchar_t buf[2048];
			_vsnwprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			static char buf2[2048];
			wcstombs(buf2, buf, 2048 -4);
			gDLL->logMsg(file, buf2);
			OutputDebugString(buf2);
		}
	}

#ifndef _DEBUG
	void skipLog(const char* file, char* msg, ...) { }
#endif
}
