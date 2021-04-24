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
void logAIJson(std::string *type, std::string *function, std::string *message)
{
	static char buf[4096];
	std::ostringstream oss;
	oss << "{ type: " << type << " function: " << function << "message: " << message << " }" << std::endl;
	const std::string data = oss.str();
	_vsnprintf(buf, 2048 - 4, data.c_str(), (char*)(&data + 1));
	gDLL->logMsg("FLB.log", buf);
}
