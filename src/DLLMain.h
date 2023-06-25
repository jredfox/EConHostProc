#pragma once
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

extern "C" __declspec(dllexport) BOOL CALLBACK SetHook(HWND hwnd);
extern "C" __declspec(dllexport) LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
extern "C" __declspec(dllexport) void init();
extern "C" __declspec(dllexport) void activateWindow(HWND hwnd);