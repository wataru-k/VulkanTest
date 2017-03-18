#pragma once


// Include header required for parsing the command line options.
#include <shellapi.h>

class CmdLineArgs {

private:
    int argc_ = 0;
    char **argv_ = nullptr;

public:

    int argc() { return argc_; }
    char **argv() { return argv_; }

    CmdLineArgs()
    {
        LPWSTR *commandLineArgs = CommandLineToArgvW(GetCommandLineW(), &argc_);
        if (nullptr == commandLineArgs) {
            argc_ = 0;
        }
        if (argc_ > 0) {
            argv_ = (char **)malloc(sizeof(char *) * argc_);
            if (argv_ == nullptr) {
                argc_ = 0;
            }
            else {
                for (int iii = 0; iii < argc_; iii++) {
                    size_t wideCharLen = wcslen(commandLineArgs[iii]);
                    size_t numConverted = 0;

                    argv_[iii] = (char *)malloc(sizeof(char) * (wideCharLen + 1));
                    if (argv_[iii] != nullptr) {
                        wcstombs_s(&numConverted, argv_[iii], wideCharLen + 1,
                            commandLineArgs[iii], wideCharLen + 1);
                    }
                }
            }
        }
        else {
            argv_ = nullptr;
        }
    }

    ~CmdLineArgs()
    {
        // Free up the items we had to allocate for the command line arguments.
        if (argc_ > 0 && argv_ != nullptr) {
            for (int iii = 0; iii < argc_; iii++) {
                if (argv_[iii] != nullptr) {
                    free(argv_[iii]);
                }
            }
            free(argv_);
        }
    }

};
