
#include "CvGameCoreDLL.h"
#include "Win32.h"

int g_nextHandle = 0;

struct MouseWheelInfo
{
	MouseWheelInfo(int handle = -1) : handle(handle), cumulative(0) {}

	int handle;
	float cumulative;
};

std::vector<MouseWheelInfo> g_mouse_wheel_cumulative;

namespace {
	struct EnumWindowsCallbackArgs {
		EnumWindowsCallbackArgs(DWORD p) : pid(p) { }
		const DWORD pid;
		std::vector<HWND> handles;
	};

	static BOOL CALLBACK EnumWindowsCallback(HWND hnd, LPARAM lParam)
	{
		EnumWindowsCallbackArgs* args = (EnumWindowsCallbackArgs*)lParam;

		DWORD windowPID;
		(void)::GetWindowThreadProcessId(hnd, &windowPID);
		if (windowPID == args->pid) {
			args->handles.push_back(hnd);
		}

		return TRUE;
	}
}

stdext::hash_map<HWND, WNDPROC> g_prev_wnd_proc;

LRESULT CALLBACK WndProcWrapper(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		float wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<float>(WHEEL_DELTA);
		for (size_t i = 0; i < g_mouse_wheel_cumulative.size(); ++i)
		{
			g_mouse_wheel_cumulative[i].cumulative += wheelDelta;
		}
		OutputDebugStringA(CvString::format("WM_MOUSEWHEEL %d %d\n", wParam, lParam).c_str());
		break;
	}

	return CallWindowProc(g_prev_wnd_proc[hwnd], hwnd, uMsg, wParam, lParam);
}

extern HANDLE dllModule;

void Win32::update_hooks()
{
	// Wrap the message handler functions for any windows we find
	EnumWindowsCallbackArgs args(::GetCurrentProcessId());
	if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args))
	{
		for (int i = 0; i < args.handles.size(); ++i)
		{
			HWND hwnd = args.handles[i];
			if(g_prev_wnd_proc.find(hwnd) == g_prev_wnd_proc.end())
			{
				g_prev_wnd_proc[hwnd] = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)&WndProcWrapper);
				OutputDebugStringA(CvString::format("Win32::update_hooks() wrapped new HWND 0x%08X\n", hwnd).c_str());
			}
		}
	}
}

HWND Win32::getToplevelWindow()
{
	EnumWindowsCallbackArgs args(::GetCurrentProcessId());
	if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE)
	{
		return NULL;
	}
	return args.handles[0];
}

bool Win32::isFocused()
{
	EnumWindowsCallbackArgs args(::GetCurrentProcessId());
	if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE)
	{
		return NULL;
	}
	return std::find(args.handles.begin(), args.handles.end(), GetFocus()) != args.handles.end();
}

POINT Win32::getCursorPos()
{
	POINT p;
	// Just assume these work, if they don't then we have bigger problems than mouse pos not being correct
	::GetCursorPos(&p);
	::ScreenToClient(getToplevelWindow(), &p);
	return p;
}

POINT Win32::screenToClient(POINT screenPos)
{
	::ScreenToClient(getToplevelWindow(), &screenPos);
	return screenPos;
}

int Win32::registerMouseWheelListener()
{
	Win32::update_hooks();
	int handle = ++g_nextHandle;
	g_mouse_wheel_cumulative.push_back(MouseWheelInfo(handle));
	OutputDebugStringA(CvString::format("Win32::registerMouseWheelListener %d\n", handle).c_str());
	return handle;
}

void Win32::unregisterMouseWheelListener(int handle)
{
	for (std::vector<MouseWheelInfo>::iterator itr = g_mouse_wheel_cumulative.begin(); itr != g_mouse_wheel_cumulative.end(); ++itr)
	{
		if (itr->handle == handle)
		{
			g_mouse_wheel_cumulative.erase(itr);
			break;
		}
	}
	OutputDebugStringA(CvString::format("Win32::unregisterMouseWheelListener %d\n", handle).c_str());
}

float Win32::getMouseWheelDiff(int handle)
{
	for (std::vector<MouseWheelInfo>::iterator itr = g_mouse_wheel_cumulative.begin(); itr != g_mouse_wheel_cumulative.end(); ++itr)
	{
		if (itr->handle == handle)
		{
			float w = itr->cumulative;
			itr->cumulative = 0;
			return w;
		}
	}
	return 0.f;
}

void Win32::pythonPublish()
{
	using namespace boost::python;
	class_<Win32>("Win32")

		.def("getCursorPos", &Win32::getCursorPos)
		.staticmethod("getCursorPos")
		.def("screenToClient", &Win32::screenToClient)
		.staticmethod("screenToClient")

		.def("registerMouseWheelListener", &Win32::registerMouseWheelListener)
		.staticmethod("registerMouseWheelListener")
		.def("unregisterMouseWheelListener", &Win32::unregisterMouseWheelListener)
		.staticmethod("unregisterMouseWheelListener")
		.def("getMouseWheelDiff", &Win32::getMouseWheelDiff)
		.staticmethod("getMouseWheelDiff")
	;
}
