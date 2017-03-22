#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "DebugConsole.h"

void DebugConsole::PrintToOutputWindow(const char* format, ...)
{
    static char s_printf_buf[1024];
    va_list args;
    va_start(args, format);
    _vsnprintf_s(s_printf_buf, sizeof(s_printf_buf), format, args);
    va_end(args);
    OutputDebugStringA(s_printf_buf);
}


void DebugConsole::Init()
{
#if DC_OPEN_WINDOW>0
    //Create a console for this application
    AllocConsole();
    //Redirect unbuffered STDOUT to the console
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE *COutputHandle = _fdopen(SystemOutput, "w");
    *stdout = *COutputHandle;
    setvbuf(stdout, NULL, _IONBF, 0);

    //Redirect unbuffered STDERR to the console
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE *CErrorHandle = _fdopen(SystemError, "w");
    *stderr = *CErrorHandle;
    setvbuf(stderr, NULL, _IONBF, 0);

    //Redirect unbuffered STDIN to the console
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE *CInputHandle = _fdopen(SystemInput, "r");
    *stdin = *CInputHandle;
    setvbuf(stdin, NULL, _IONBF, 0);

    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    ios::sync_with_stdio(true);
#endif
}

void DebugConsole::Print(const char* format, ...)
{
#if DC_OPEN_WINDOW>0
    static char s_printf_buf[1024];
    va_list args;
    va_start(args, format);
    _vsnprintf(s_printf_buf, sizeof(s_printf_buf), format, args);
    va_end(args);
    OutputDebugStringA(s_printf_buf);

    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD CharsWritten;
    size_t len = strlen(s_printf_buf);
    WriteConsole(ConsoleOutput, s_printf_buf, (DWORD)len, &CharsWritten, 0);
#endif
}



void DebugConsole::Term(void)
{
#if DC_OPEN_WINDOW>0
    //Write "Press any key to exit"
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD CharsWritten;
    WriteConsole(ConsoleOutput, "\nPress any key to exit", 22, &CharsWritten, 0);
    //Disable line-based input mode so we can get a single character
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(ConsoleInput, 0);
    //Read a single character
    TCHAR InputBuffer;
    DWORD CharsRead;
    ReadConsole(ConsoleInput, &InputBuffer, 1, &CharsRead, 0);
#endif
}
