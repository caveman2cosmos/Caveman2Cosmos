#include "CvGameCoreDLL.h"

#if (defined(FASSERT_ENABLE) || !defined(_DEBUG)) && defined(WIN32)

#include "CvErrorHandling.h"
#include "FDialogTemplate.h"
#include "CvStackTraces.h"
#include <sstream>


bool FAssertInfo::bDummyIgnoreAlways;

const char* LINE_SEP =
	"------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n";
const char* LINE_SEP2 = 
	"==============================================================================================================================================================================\r\n";

std::string section(const char* title, const std::string& details)
{
	if (details.empty())
	{
		return std::string();
	}
	return
		std::string(
		) +
		std::string(title) + "\r\n" +
		std::string(
		) +
		details + "\r\n";
}

int sprintFAssertMsg(char* dest, const std::string& fileName, unsigned int line, const std::string& expression, 
	const std::string& msg, const std::string& pythonCallstack, const std::string& dllCallstack)
{
	return sprintf(dest,
		"ASSERT FAILED\r\n"
		"%s"
		"File:        %s\r\n"
		"Line:        %u\r\n"
		"Expression:  %s\r\n"
		"%s%s%s",
		LINE_SEP2,
		fileName.c_str(),
		line,
		expression.c_str(),
		section("MESSAGE:", msg).c_str(),
		section("PYTHON CALLSTACK:", pythonCallstack).c_str(),
		section("DLL CALLSTACK:", dllCallstack).c_str()
	);
}

void fAssert(const char* szExpr, const char* szMsg, const char* szFile, unsigned int line, 
	const char* szFunction, bool* bIgnoreAlways, bool ensure)
{
	FAssertInfo info;
	info.szExpression = szExpr ? szExpr : "";
	info.szMessage = szMsg ? szMsg : "";
	info.szPythonCallstack = getPyTrace();
	info.szDLLCallstack = getDLLTrace();
	info.szFileName = szFile ? szFile : "";
	info.line = line;
	info.bIgnoreAlways = bIgnoreAlways;

	CvErrorHandling::assertFail(info, ensure);
}

void fError(const char* szMsg, const char* szFile, unsigned int line,
	const char* szFunction, bool* bIgnoreAlways, bool ensure)
{
	FAssertInfo info;
	info.szExpression = "ERROR";
	info.szMessage = szMsg ? szMsg : "";
	info.szPythonCallstack = getPyTrace();
	info.szDLLCallstack = getDLLTrace();
	info.szFileName = szFile ? szFile : "";
	info.line = line;
	info.bIgnoreAlways = bIgnoreAlways;

	CvErrorHandling::error(info, ensure);
}

#endif // FASSERT_ENABLE && WIN32
