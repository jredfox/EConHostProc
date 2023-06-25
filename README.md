# EConHostProc
 Enhanced ConHost.exe Window Proceedures.  
 
 To use said features call this fallowing code from an externally from the DLL file of EConHostProc.dll
```
string path = "<PATH_TO_DLL>\\EConHostProc.dll";
hinstDLL = LoadLibrary(path.c_str());
SetHook = (SetHookType)GetProcAddress(hinstDLL, "SetHook");
SetHook(GetConsoleWindow());
```

# Compiling
Use Microsoft Visual Studio 2017. 2022 has a bug where if it compiles a dll and namespace std is called it will stop compiling the DLL. I reported it and asked for work arounds but there are none for now.
