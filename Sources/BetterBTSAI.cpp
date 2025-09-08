#include "CvGameCoreDLL.h"
#include "CvGlobals.h"

// Log levels:
// 0 - None
// 1 - Important decisions only
// 2 - Many decisions
// 3 - All logging
// 4 - All logging with very often trace (unit/promotion eval)
int gPlayerLogLevel = 0;
int gTeamLogLevel = 0;
int gCityLogLevel = 0;
int gUnitLogLevel = 0;

// AI decision making logging
void logBBAI(const char* format, ...)
{
	static char buf[2048];
	_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
	gDLL->logMsg("BBAI.log", buf);

	// Echo to debugger
	strcat(buf, "\n");
	OutputDebugString(buf);
}
void logContractBroker(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("ContractBroker.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}

}

void logAiEvaluations(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("AiEvaluation.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

void logAIJson(CvWString type, CvWString identifier, CvWString squirrel, CvWString message)
{
	const std::wstring data = "{ type: \"" + type + "\" name: \"" + identifier + "\" function: \" " + squirrel + "\" message: \"" + message + "\" }";
	logging::logMsgW("FLB.log", data.c_str());
}

void logCB(CvString message) {

	const std::string data = message;
	logging::logMsg("CB.log", data.c_str());
}

void logToFile(CvString message, const char* filename) {
	const std::string data = message;
	logging::logMsg(filename, data.c_str());
}
