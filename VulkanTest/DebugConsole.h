#pragma once


#define DC_OPEN_WINDOW 1 //�f�o�b�O�R���\�[����ʃE�C���h�E�ŊJ���ďo�� or �A�E�g�v�b�g�E�C���h�E�֏o�́B

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


