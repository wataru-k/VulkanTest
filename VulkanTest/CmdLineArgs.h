#pragma once


// Include header required for parsing the command line options.
#include <shellapi.h>

namespace CmdLineArgs {

    void begin(int &argc, char **& argv)
    {
        LPWSTR *commandLineArgs = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (nullptr == commandLineArgs) {
            argc = 0;
        }
        if (argc > 0) {
            argv = (char **)malloc(sizeof(char *) * argc);
            if (argv == nullptr) {
                argc = 0;
            }
            else {
                for (int iii = 0; iii < argc; iii++) {
                    size_t wideCharLen = wcslen(commandLineArgs[iii]);
                    size_t numConverted = 0;

                    argv[iii] = (char *)malloc(sizeof(char) * (wideCharLen + 1));
                    if (argv[iii] != nullptr) {
                        wcstombs_s(&numConverted, argv[iii], wideCharLen + 1,
                            commandLineArgs[iii], wideCharLen + 1);
                    }
                }
            }
        }
        else {
            argv = nullptr;
        }
    }

    void end(int &argc, char **& argv)
    {
        // Free up the items we had to allocate for the command line arguments.
        if (argc > 0 && argv != nullptr) {
            for (int iii = 0; iii < argc; iii++) {
                if (argv[iii] != nullptr) {
                    free(argv[iii]);
                }
            }
            free(argv);
        }
    }

};
