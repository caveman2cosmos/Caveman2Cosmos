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
		logging::logMsg("EventsJson.log", picojson::value(obj).serialize().c_str());
#endif
	}

	void logMsg(const char* file, const char* msg, ...)
	{
		if (GC.isXMLLogging())
		{
			static char buf[2048];
			_vsnprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			strcat(buf, "\n");

			const std::string path = getModDir() + "\\Logs\\" + file;
			std::fstream stream(path.c_str(), std::ios::out | std::ios::app);
			FAssert(stream.is_open())
			stream << buf;
			stream.close();

			OutputDebugString(buf);
		}
	}

	void logMsgW(const char* file, const wchar_t* msg, ...)
	{
			static wchar_t buf[2048];
			_vsnwprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			static char buf2[2048];
			wcstombs(buf2, buf, 2048 -4);
			strcat(buf2, "\n");

			const std::string path = getModDir() + "\\Logs\\" + file;
			std::fstream stream(path.c_str(), std::ios::out | std::ios::app);
			FAssert(stream.is_open())
			stream << buf2;
			stream.close();
			OutputDebugString(buf2);
	}

	void createLogsFolder()
	{
		const std::string logsDir = getModDir() + "\\Logs";
		CreateDirectory(logsDir.c_str(), NULL);
	}

	void deleteLogs()
	{
		const std::string path = getModDir() + "\\Logs\\*.*";
		WIN32_FIND_DATA FileInformation;
		HANDLE hFile = FindFirstFile(path.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do {
				const std::string cFile = getModDir() + "\\Logs\\" + FileInformation.cFileName;
				const bool deleted = DeleteFile(cFile.c_str());
				//FAssert(deleted)
			} while (FindNextFile(hFile, &FileInformation));
		}
	}
}
