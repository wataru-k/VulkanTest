#pragma once


#define DC_OPEN_WINDOW 1 //デバッグコンソールを別ウインドウで開いて出力 or アウトプットウインドウへ出力。

namespace DebugConsole {

    void Init();

    void Print(const char* format, ...);

    void PrintToOutputWindow(const char* format, ...);

    void Term();
}


#if DC_OPEN_WINDOW>0
#define DC_PRINT(format, ...) DebugConsole::Print(format, __VA_ARGS__)
#else
#define DC_PRINT(format, ...) DebugConsole::PrintToOutputWindow(format, __VA_ARGS__)
#endif


