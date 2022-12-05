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

int FAssertInfo::sprint(char* dest) const
{
	return sprintf(dest,
		"ASSERT FAILED\r\n"
		"%s"
		"File:        %s\r\n"
		"Line:        %u\r\n"
		"Expression:  %s\r\n"
		"%s%s%s",
		LINE_SEP2,
		szFileName.c_str(),
		line,
		szExpression.c_str(),
		section("MESSAGE:", szMessage).c_str(),
		section("PYTHON CALLSTACK:", szPythonCallstack).c_str(),
		section("DLL CALLSTACK:", szDLLCallstack).c_str()
	);
}

void fAssert(const char* szExpr, const char* szMsg, const char* szFile, unsigned int line, 
	const char* szFunction, bool* bIgnoreAlways)
{
	FAssertInfo info;
	info.szExpression = szExpr ? szExpr : "";
	info.szMessage = szMsg ? szMsg : "";
	info.szPythonCallstack = getPyTrace();
	info.szDLLCallstack = getDLLTrace();
	info.szFileName = szFile ? szFile : "";
	info.line = line;
	info.bIgnoreAlways = bIgnoreAlways;

	CvErrorHandling::assertFail(info);
}

void fError(const char* szMsg, const char* szFile, unsigned int line,
	const char* szFunction, bool* bIgnoreAlways)
{
	FAssertInfo info;
	info.szExpression = "ERROR";
	info.szMessage = szMsg ? szMsg : "";
	info.szPythonCallstack = getPyTrace();
	info.szDLLCallstack = getDLLTrace();
	info.szFileName = szFile ? szFile : "";
	info.line = line;
	info.bIgnoreAlways = bIgnoreAlways;

	CvErrorHandling::error(info);
}

void fEnsure(const char* szExpr, const char* szMsg, const char* szFile, unsigned int line,
	const char* szFunction, bool* bIgnoreAlways)
{
	FAssertInfo info;
	info.szExpression = szExpr ? szExpr : "";
	info.szMessage = szMsg ? szMsg : "";
	info.szPythonCallstack = getPyTrace();
	info.szDLLCallstack = getDLLTrace();
	info.szFileName = szFile ? szFile : "";
	info.line = line;
	info.bIgnoreAlways = bIgnoreAlways;

	CvErrorHandling::ensureFail(info);
}

#endif // FASSERT_ENABLE && WIN32
