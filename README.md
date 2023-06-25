# EConHostProc
 Enhanced ConHost.exe Window Proceedures.  
 
# Using
* Due to DLL hell where you cannot load two instances of a DLL even in different processes. We have to extract the DLL file every time to a session instance
* Generate unique session name, verify path doesn't exist, then create it if it returns false. use a do while loop
* extract the dll to the session folder or put the session in the name like so `<PATH/SESSION/EConHostProc.dll>` or `<PATH/EConHostProc-<SESSION>.dll>`
* call this code bellow from an external executeable process from the dll
```
string path = "<PATH_TO_DLL>\\EConHostProc.dll";
hinstDLL = LoadLibrary(path.c_str());
SetHook = (SetHookType)GetProcAddress(hinstDLL, "SetHook");
SetHook(GetConsoleWindow());
```

# Compiling
Use Microsoft Visual Studio 2017. 2022 has a bug where if it compiles a dll and namespace std is called it will stop compiling the DLL. I reported it and asked for work arounds but there are none for now.

# FAQ
* Q: Why not just use static arrays or vectors for your DLL's so we don't need to extract them everytime?
* A: Shared Memory segmants are not supported with use of a vector. If I used static arrays it would work all the way till the end of the index happens. If we expanded the index it would overwrite with other processes trying to expand them at the same time thus leading to problems. If we set the array to 32,000 and one launch happened every second in the background but kept one CLI open the issue of it not being able to hook happens again. For these reasons we cannot just use static arrays. This isn't a library for creating a new shared memory vector so I decided to keep the code clean and just force an extraction of 10ms uptime as it's then it can call LoadLibrary like normal. 
