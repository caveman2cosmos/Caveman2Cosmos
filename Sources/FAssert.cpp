#include "CvGameCoreDLL.h"

#if (defined(FASSERT_ENABLE) || !defined(_DEBUG)) && defined(WIN32)

#include "CvPython.h"
#include "FDialogTemplate.h"
#include "StackWalker.h"
#include <sstream>

// Some Resource IDs for our dialog template
#define IDC_IGNORE_ALWAYS               1001
#define IDC_IGNORE_ONCE                 1002
#define IDC_DEBUG                       1003
#define IDC_ABORT                       1004
#define IDC_ASSERTION_TEXT              1005
#define IDC_COPY_TO_CLIPBOARD           1006
#define LINE_SEP "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n"
#define LINE_SEP2 "==============================================================================================================================================================================\r\n"
namespace detail
{
	// These are the return values from the modal Assert Dialog
	enum
	{
		ASSERT_DLG_DEBUG = 0x01,
		ASSERT_DLG_IGNORE = 0x02,
		ASSERT_DLG_IGNOREALWAYS = 0x03,
		ASSERT_DLG_EXIT = 0x04,
	};

	// This global structure is filled out by the original call to our FAssert
	// and is used by the dialog proc to display appropriate debug info
	struct AssertInfo
	{
		std::string szExpression;
		std::string szMessage;
		std::string szPythonCallstack;
		std::string szDLLCallstack;
		std::string szFileName;
		unsigned int line;

		// EIP / EBP / ESP
		CONTEXT context;
	} g_AssertInfo;

	// Use a static array since often times asserts are fired in response to problems
	// caused by being low on resources, so dynamically creating strings here might not
	// be such a hot idea
	const unsigned int MAX_ASSERT_TEXT = 65536;
	char g_AssertText[MAX_ASSERT_TEXT];

	std::string section(const char* title, const std::string& details)
	{
		if (details.empty())
		{
			return std::string();
		}
		return std::string(LINE_SEP2) +
			std::string(title) + "\r\n" +
			std::string(LINE_SEP) +
			details + "\r\n";
	}

	INT_PTR CALLBACK AssertDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_INITDIALOG:
		{
			char modulePath[MAX_PATH];
			GetModuleFileName(NULL, modulePath, MAX_PATH);

			const char* moduleName = strrchr(modulePath, '\\');
			moduleName = moduleName ? moduleName + 1 : modulePath;

			char title[MAX_PATH + 20];
			sprintf(title, "Assert Failed: %s", moduleName);
			SetWindowText(hDlg, title);

			sprintf(g_AssertText,
				"ASSERT FAILED\r\n"
				LINE_SEP2
				"File:        %s\r\n"
				"Line:        %u\r\n"
				"Expression:  %s\r\n"
				"%s%s%s",
				g_AssertInfo.szFileName.c_str(),
				g_AssertInfo.line,
				g_AssertInfo.szExpression.c_str(),
				section("MESSAGE:", g_AssertInfo.szMessage).c_str(),
				section("PYTHON CALLSTACK:", g_AssertInfo.szPythonCallstack).c_str(),
				section("DLL CALLSTACK:", g_AssertInfo.szDLLCallstack).c_str()
			);

			::SetWindowText(::GetDlgItem(hDlg, IDC_ASSERTION_TEXT), g_AssertText);
			::SetFocus(::GetDlgItem(hDlg, IDC_DEBUG));

			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_DEBUG:
				EndDialog(hDlg, ASSERT_DLG_DEBUG);
				return TRUE;

			case IDC_IGNORE_ONCE:
				EndDialog(hDlg, ASSERT_DLG_IGNORE);
				return TRUE;

			case IDC_IGNORE_ALWAYS:
				EndDialog(hDlg, ASSERT_DLG_IGNOREALWAYS);
				return TRUE;

			case IDC_ABORT:
				EndDialog(hDlg, ASSERT_DLG_EXIT);
				return TRUE;
			}
		}
		break;
		}

		return FALSE;
	}

	DWORD DisplayAssertDialog()
	{
		const int wid = 600, hgt = 300;
		const int button_wid = 64, button_hgt = 14;
		const int padding = 8;
		CDialogTemplate dialogTemplate(_T("Assert Failed!"),
			DS_SETFONT | DS_CENTER | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU,
			0, 0, wid, hgt, _T("Consolas"), 10);

		dialogTemplate.AddButton(_T("&Debug"), WS_VISIBLE, 0,
			wid - (button_wid + padding) * 1, hgt - button_hgt - padding, button_wid, button_hgt, IDC_DEBUG);

		dialogTemplate.AddButton(_T("&Abort"), WS_VISIBLE, 0,
			wid - (button_wid + padding) * 2, hgt - button_hgt - padding, button_wid, button_hgt, IDC_ABORT);

		dialogTemplate.AddButton(_T("Ignore Always"), WS_VISIBLE, 0,
			wid - (button_wid + padding) * 3, hgt - button_hgt - padding, button_wid, button_hgt, IDC_IGNORE_ALWAYS);

		dialogTemplate.AddButton(_T("&Ignore Once"), WS_VISIBLE, 0,
			wid - (button_wid + padding) * 4, hgt - button_hgt - padding, button_wid, button_hgt, IDC_IGNORE_ONCE);

		dialogTemplate.AddEditBox(_T(""), ES_MULTILINE | ES_AUTOVSCROLL |
			ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE, WS_EX_STATICEDGE,
			padding, padding, wid - padding * 2, hgt - padding * 3 - button_hgt, IDC_ASSERTION_TEXT);

		int res = DialogBoxIndirect(GetModuleHandle(0), dialogTemplate.GetDialogTemplate(), NULL, (DLGPROC)AssertDlgProc);
		return res;
	}

	std::string getPyTrace()
	{
		std::vector<Cy::StackFrame> trace = Cy::get_stack_trace();

		std::stringstream buffer;

		for (std::vector<Cy::StackFrame>::const_iterator itr = trace.begin(); itr != trace.end(); ++itr)
		{
			if (itr != trace.begin()) buffer << "\r\n";
			buffer << CvString::format("%s.py (%d): %s", itr->filename.c_str(), itr->line, itr->code.c_str());
		}

		return buffer.str();
	}

	class C2CStackWalker : StackWalker
	{
	public:
		std::string get_callstack(int skip = 0)
		{
			m_buffer.str(std::string());
			m_idx = 0;
			m_skip = skip;
			m_started = false;
			ShowCallstack();
			return m_buffer.str();
		}

		static C2CStackWalker& get()
		{
			static C2CStackWalker inst;
			return inst;
		}

	protected:

		virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry)
		{
			if ((eType != lastEntry) && (entry.offset != 0) && m_idx++ >= m_skip)
			{
				if(m_started)
					m_buffer << "\r\n";
				m_started = true;
				if (entry.lineFileName[0] == 0)
				{
					m_buffer << (LPVOID)entry.offset;
					if (entry.moduleName[0] == 0)
						m_buffer << " (module-name not available):";
					else
						m_buffer << " (" << entry.moduleName << "):";
					m_buffer << " (filename not available): ";
				}
				else
				{
					std::string fileName(entry.lineFileName);
					std::string fileNameLwr(fileName);
					algo::transform(fileNameLwr, fileNameLwr.begin(), ::tolower);
					size_t offs = fileNameLwr.find("caveman2cosmos");
					if (offs != std::string::npos)
					{
						fileName = fileName.substr(offs + 15);
					}
					m_buffer << fileName << " (" << entry.lineNumber << "): ";
				}
				if (entry.undName[0] != 0)
					m_buffer << entry.undName;
				else if (entry.undFullName[0] != 0)
					m_buffer << entry.undFullName;
				else if (entry.name[0] != 0)
					m_buffer << entry.name;
				else
					m_buffer << "(function-name not available)";
			}
		}
	private:
		int m_idx;
		int m_skip;
		bool m_started;
		std::stringstream m_buffer;
	};

	std::string getDLLTrace()
	{
		return C2CStackWalker::get().get_callstack(4);
	}
} // end anonymous namespace

bool FAssertDlg( const char* szExpr, const char* szMsg, const char* szFile, unsigned int line, const char* szFunction, bool& bIgnoreAlways )
{
//	FILL_CONTEXT( g_AssertInfo.context );

	std::string pyTrace = detail::getPyTrace();
	std::string dllTrace = detail::getDLLTrace();

#ifdef FASSERT_LOGGING
	logging::logMsg("Asserts.log", "%s %s (%d): %s,  %s\n%s\n%s",
		szFile ? szFile : "",
		szFunction ? szFunction : "",
		line,
		szExpr ? szExpr : "",
		szMsg ? szMsg : "",
		pyTrace.c_str(),
		dllTrace.c_str()
	);

	picojson::value::object obj;

	if (szFile) obj["file"] = picojson::value(szFile);
	if (szFunction) obj["function"] = picojson::value(szFunction);
	obj["line"] = picojson::value(static_cast<double>(line));
	if (szExpr) obj["expr"] = picojson::value(szExpr);
	if (szMsg) obj["msg"] = picojson::value(szMsg);
	if (!pyTrace.empty()) obj["py_trace"] = picojson::value(pyTrace);
	if (!dllTrace.empty()) obj["dll_trace"] = picojson::value(dllTrace);
	obj["assert_key"] = picojson::value(CvString::format(
		"%s %s (%d): %s",
		szFile ? szFile : "nofile",
		szFunction ? szFunction : "nofunc",
		line,
		szExpr ? szExpr : "noexpr"
	).c_str());
	obj["callstack_key"] = picojson::value(CvString::format("%u", stdext::hash_value(pyTrace + dllTrace)).c_str());
	logging::logMsg("AssertsJson.log", picojson::value(obj).serialize().c_str());

	return false;
#else
	detail::g_AssertInfo.szExpression = szExpr ? szExpr : "";
	detail::g_AssertInfo.szMessage = szMsg ? szMsg : "";
	detail::g_AssertInfo.szPythonCallstack = pyTrace;
	detail::g_AssertInfo.szDLLCallstack = dllTrace;
	detail::g_AssertInfo.szFileName = szFile ? szFile : "";
	detail::g_AssertInfo.line = line;

	DWORD dwResult = detail::DisplayAssertDialog();

	switch( dwResult )
	{
	case detail::ASSERT_DLG_DEBUG:
		return true;

	case detail::ASSERT_DLG_IGNORE:
		return false;

	case detail::ASSERT_DLG_IGNOREALWAYS:
		bIgnoreAlways = true;
		return false;

	case detail::ASSERT_DLG_EXIT:
		exit(0);
		break;
	}
	return true;
#endif
}

#endif // FASSERT_ENABLE && WIN32
