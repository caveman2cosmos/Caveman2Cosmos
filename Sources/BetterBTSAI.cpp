#include "CvGameCoreDLL.h"
#include "CvGlobals.h"

#ifdef LOG_AI
int gPlayerLogLevel = 0;
int gTeamLogLevel = 0;
int gCityLogLevel = 0;
int gUnitLogLevel = 0;
#endif

// AI decision making logging

void logBBAI(char* format, ... )
{
	if (GC.isXMLLogging())
	{
		static char buf[2048];
		_vsnprintf( buf, 2048-4, format, (char*)(&format+1) );
		gDLL->logMsg("BBAI.log", buf);

		//	Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}
void logAIJson(CvWString type,CvWString identifier, CvWString squirrel, CvWString message)
{


	const std::wstring data = "{ type: \"" + type + "\" name: \""+identifier+ "\" function: \" " + squirrel + "\" message: \"" + message + "\" }";
	logging::logMsgW("FLB.log", data.c_str());
}
