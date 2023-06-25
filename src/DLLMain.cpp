#include <windows.h>
#include <iostream>
#include <string>
#include <commctrl.h>
#define WM_NOTEPADMESSAGE WM_USER + 10
using namespace std;

#pragma comment( lib, "comctl32.lib")
#pragma data_seg("SHARED")
HWND target = nullptr;
HHOOK hGetMsgHook;
bool hasInit = false;
DWORD consolePID = 0;
#pragma data_seg()
#pragma comment(linker, "/section:SHARED,RWS")

HINSTANCE hInst;

extern "C" __declspec(dllexport) LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		hInst = (HINSTANCE)hModule;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	{

	}
	break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void activateWindow(HWND hwnd)
{
	//if it's minimized restore it
	if (IsIconic(hwnd))
	{
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	//bring the window to the top and activate it
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	SetActiveWindow(hwnd);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	//redraw to prevent the window blank.
	RedrawWindow(hwnd, NULL, 0, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

extern "C" __declspec(dllexport) DWORD getPID(HWND h)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(h, &pid);
	return pid;
}

extern "C" __declspec(dllexport) const char * WinGetEnv(const char * name)
{
	const DWORD buffSize = 65535;
	static char buffer[buffSize];
	if (GetEnvironmentVariableA(name, buffer, buffSize))
	{
		return buffer;
	}
	else
	{
		return "";
	}
}

extern "C" __declspec(dllexport) BOOL CALLBACK SetHook(HWND hwnd)
{
	target = hwnd;
	consolePID = getPID(GetConsoleWindow());
	hGetMsgHook = SetWindowsHookExA(WH_CBT, (HOOKPROC)GetMsgProc, hInst, 0);
	if (!hGetMsgHook)
	{
		std::cout << "SetWindowsHookEx FAILED!" << std::endl;
		return FALSE;
	}
	//set the focus to the desktop
	HWND lHwnd = FindWindow("Shell_TrayWnd", NULL);
	activateWindow(lHwnd);

	//activate our apps
	activateWindow(hwnd);
	return TRUE;
}
/**
* the windows hook hanlder to inject the new windows proc into the application
**/
extern "C" __declspec(dllexport) LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG* lpMsg;
	if (nCode >= 0)
	{
		lpMsg = (MSG*)lParam;
		HWND whand = (HWND)wParam;
		if (whand == target)
		{
			bool bSubclassed = SetWindowSubclass((HWND)target, SubclassProc, 0, 0);
			SetFocus(target);
			UnhookWindowsHookEx(hGetMsgHook);//removes our hook so it doesn't lag the OS
			if (!bSubclassed)
			{
				char out[64] = { 0 };
				sprintf_s(out, 63, "Error Replacing Window Proc: %i", (UINT)target);
				OutputDebugStringA(out);
			}
		}
	}
	return CallNextHookEx(hGetMsgHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void init()
{
	if (!hasInit)
	{
		hasInit = true;
		//Beep(800, 500);
		char filename[MAX_PATH]; //this is a char buffer
		GetModuleFileNameA(hInst, filename, sizeof(filename));
		string s = string(filename);
		LoadLibraryA(s.c_str()); //Makes it so the DLL doesn't unload from the process
	}
}

extern "C" __declspec(dllexport) LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	init();
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			//get WINSIG from the path or the APPDATA
			string winsig = WinGetEnv("WINSIG");
			if (winsig.empty())
				winsig = string(WinGetEnv("APPDATA")) + "\\OpenTerminal\\libs\\WINSIG.exe";

			string cmd = winsig + " " + to_string(consolePID) + " " + to_string(CTRL_C_EVENT);
			system(cmd.c_str());
			MessageBox(NULL, cmd.c_str(), NULL, NULL);
			break;
		}
		default:
			break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}