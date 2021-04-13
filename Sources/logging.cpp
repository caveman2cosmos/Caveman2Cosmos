#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvDLLIniParserIFaceBase.h"
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
		if (GC.getLogging())
		{
			static char buf[2048];
			_vsnprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			strcat(buf, "\n");

			std::ofstream stream((getModDir() + "\\Logs\\" + file).c_str(), std::ios::app);
			FAssert(stream.is_open())
			stream << buf;
			stream.close();

			OutputDebugString(buf);
		}
	}

	void logMsgW(const char* file, const wchar_t* msg, ...)
	{
		if (GC.getLogging())
		{
			static wchar_t buf[2048];
			_vsnwprintf(buf, 2048 -4, msg, (char*)(&msg +1));
			static char buf2[2048];
			wcstombs(buf2, buf, 2048 -4);
			strcat(buf2, "\n");

			std::ofstream stream((getModDir() + "\\Logs\\" + file).c_str(), std::ios::app);
			FAssert(stream.is_open())
			stream << buf2;
			stream.close();

			OutputDebugString(buf2);
		}
	}

	void setOption(bool& option)
	{
		FIniParser* parser = gDLL->getIniParserIFace()->create((getModDir() + "\\Caveman2Cosmos.ini").c_str());
		const bool sectionSet = gDLL->getIniParserIFace()->SetGroupKey(parser, "CONFIG");
		const bool optionFound = gDLL->getIniParserIFace()->GetKeyValue(parser, "EnableLogging", &option);
		FAssert(sectionSet && optionFound);
		gDLL->getIniParserIFace()->destroy(parser);
	}

	void createLogsFolder()
	{
		CreateDirectory((getModDir() + "\\Logs").c_str(), NULL);
	}

	void deleteLogs()
	{
		WIN32_FIND_DATA FileInfo;
		const HANDLE hFile = FindFirstFile((getModDir() + "\\Logs\\*.*").c_str(), &FileInfo);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do {
				DeleteFile((getModDir() + "\\Logs\\" + FileInfo.cFileName).c_str());
			} while (FindNextFile(hFile, &FileInfo));
		}
	}
}
