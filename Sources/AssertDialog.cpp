#include "AssertDialog.h"

#include "FDialogTemplate.h"
#include <sstream>


// Some Resource IDs for our dialog template
#define IDC_IGNORE_ALWAYS               1001
#define IDC_IGNORE_ONCE                 1002
#define IDC_DEBUG                       1003
#define IDC_ABORT                       1004
#define IDC_ASSERTION_TEXT              1005
#define IDC_COPY_TO_CLIPBOARD           1006

FAssertInfo g_AssertInfo;
// Use a static array since often times asserts are fired in response to problems
// caused by being low on resources, so dynamically creating strings here might not
// be such a hot idea
const unsigned int MAX_ASSERT_TEXT = 65536;
char g_AssertText[MAX_ASSERT_TEXT];

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

		sprintFAssertMsg(g_AssertText, g_AssertInfo);

		::SetWindowText(::GetDlgItem(hDlg, IDC_ASSERTION_TEXT), g_AssertText);
		::SetFocus(::GetDlgItem(hDlg, IDC_DEBUG));

		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_DEBUG:
			EndDialog(hDlg, AssertDialog::ASSERT_DLG_DEBUG);
			return TRUE;

		case IDC_IGNORE_ONCE:
			EndDialog(hDlg, AssertDialog::ASSERT_DLG_IGNORE);
			return TRUE;

		case IDC_IGNORE_ALWAYS:
			EndDialog(hDlg, AssertDialog::ASSERT_DLG_IGNOREALWAYS);
			return TRUE;

		case IDC_ABORT:
			EndDialog(hDlg, AssertDialog::ASSERT_DLG_EXIT);
			return TRUE;
		}
	}
	break;
	}

	return FALSE;
}


AssertDialog::Result AssertDialog::display(const FAssertInfo& assertInfo)
{
	g_AssertInfo = assertInfo;

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
	return static_cast<Result>(res);
}