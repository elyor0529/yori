/**
 * @file cvtvt/main.c
 *
 * Main entry point code to convert and process VT100/ANSI escape sequences.
 *
 * Copyright (c) 2015-2018 Malcolm J. Smith
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "cvtvt.h"

/**
 Help text to display for this application.
 */
const
CHAR strHelpText[] =
        "\n"
        "Converts text with VT100 color escapes into another format.\n"
        "\n"
        "CVTVT [Options] [-exec binary|filename]\n"
        "\n"
        " Options include:\n"
        "   -exec binary   Run process and pipe its output into cvtvt\n"
        "   -html4         Generate output with FONT tags (no backgrounds)\n"
        "   -html5         Generate output with CSS\n"
        "   -text          Generate output as plain text\n"
        "   -win32         Convert to native Win32\n"
        "\n"
        " If filename and exec are not specified, operates on standard"
        " input.\n";

/**
 Display help text and license for this application.
 */
BOOL
CvtvtUsage()
{
    YORI_STRING License;

    YoriLibMitLicenseText(_T("2015-2018"), &License);

    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("Cvtvt %i.%i\n"), CVTVT_VER_MAJOR, CVTVT_VER_MINOR);
#if YORI_BUILD_ID
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("  Build %i\n"), YORI_BUILD_ID);
#endif
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%hs\n"), strHelpText);
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%y"), &License);
    YoriLibFreeStringContents(&License);

    return TRUE;
}

/**
 Pump any incoming data from standard input to a specified pipe which will
 be used as the input for a child process.

 @param Context The context for the child, which in this case is the handle
        for the pipe to write data to.

 @return Exit code for the thread, currently unused.
 */
DWORD WINAPI
CvtvtInputPumpThread(
    __in LPVOID Context
    )
{
    HANDLE hOutput = (HANDLE)Context;
    UCHAR StupidBuffer[256];
    DWORD BytesRead;
    DWORD BytesWritten;

    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);

    while (ReadFile(GetStdHandle(STD_INPUT_HANDLE), StupidBuffer, sizeof(StupidBuffer), &BytesRead, NULL)) {
        if (!WriteFile(hOutput, StupidBuffer, BytesRead, &BytesWritten, NULL)) {
            break;
        }
    }

    return 0;
}

/**
 The master entrypoint for cvtvt.

 @param ArgC The number of arguments.

 @param ArgV An array of arguments to cvtvt.

 @return Exit code for the application, typically zero for success and
         nonzero for failure.
 */
DWORD
ymain(
    __in DWORD ArgC,
    __in YORI_STRING ArgV[]
    )
{
    HANDLE hSource = INVALID_HANDLE_VALUE;
    HANDLE hOutput = INVALID_HANDLE_VALUE;
    HANDLE hControl = INVALID_HANDLE_VALUE;
    
    DWORD  CurrentOffset;
    PVOID  LineReadContext = NULL;
    BOOL   Result;
    YORI_STRING LineString;
    YORI_STRING Arg;
    PYORI_STRING UserFileName = NULL;
    YORI_STRING FileName;
    YORI_LIB_VT_CALLBACK_FUNCTIONS Callbacks;

    BOOLEAN StreamStarted = FALSE;
    BOOLEAN ExecMode = FALSE;
    BOOLEAN DisplayUsage = FALSE;
    BOOLEAN ArgParsed = FALSE;
    BOOLEAN StripEscapes = FALSE;

    CvtvtHtml4SetFunctions(&Callbacks);

    //
    //  Parse arguments
    //

    for (CurrentOffset = 1; CurrentOffset < ArgC; CurrentOffset++) {
        ASSERT(YoriLibIsStringNullTerminated(&ArgV[CurrentOffset]));
        if (YoriLibIsCommandLineOption(&ArgV[CurrentOffset], &Arg)) {

            ArgParsed = FALSE;

            if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("?")) == 0) {
                ArgParsed = TRUE;
                DisplayUsage = TRUE;
            } else if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("exec")) == 0) {
                ArgParsed = TRUE;
                ExecMode = TRUE;
            } else if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("html4")) == 0) {
                ArgParsed = TRUE;
                StripEscapes = FALSE;
                CvtvtHtml4SetFunctions(&Callbacks);
            } else if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("html5")) == 0) {
                ArgParsed = TRUE;
                StripEscapes = FALSE;
                CvtvtHtml5SetFunctions(&Callbacks);
            } else if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("text")) == 0) {
                ArgParsed = TRUE;
                StripEscapes = TRUE;
            } else if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("win32")) == 0) {
                ArgParsed = TRUE;
                StripEscapes = FALSE;
                YoriLibConsoleSetFunctions(&Callbacks);
            }

            if (!ArgParsed) {
                DisplayUsage = TRUE;
            }

        } else {
            UserFileName = &ArgV[CurrentOffset];
        }
    }

    YoriLibInitEmptyString(&FileName);

    //
    //  As a bit of a hack, if the input is stdin and stdin is to
    //  a console (as opposed to a pipe or file) assume the user isn't
    //  sure how to run this program and help them along.
    //

    if (UserFileName == NULL) {
        DWORD FileType = GetFileType(GetStdHandle(STD_INPUT_HANDLE));
        FileType = FileType & ~(FILE_TYPE_REMOTE);
        if (FileType == FILE_TYPE_CHAR) {
            DisplayUsage = TRUE;
        }
    }

    if (DisplayUsage) {
        CvtvtUsage();
        return EXIT_FAILURE;
    }

    //
    //  If we have a file name, read it; otherwise read from stdin
    //

    if (ExecMode) {
        if (UserFileName == NULL) {
            CvtvtUsage();
            return EXIT_FAILURE;
        } else {
            HANDLE hProcessInput;
            HANDLE hProcessOutput;
            STARTUPINFO StartupInfo;
            PROCESS_INFORMATION ProcessInfo;
            TCHAR szTermVar[256];
            DWORD dwConsoleMode;

            if (!YoriLibUserStringToSingleFilePath(UserFileName, TRUE, &FileName)) {
                return EXIT_FAILURE;
            }

            ZeroMemory(&StartupInfo, sizeof(StartupInfo));
            ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

            CreatePipe(&hProcessInput,
                       &hControl,
                       NULL,
                       2048);

            CreatePipe(&hSource,
                       &hProcessOutput,
                       NULL,
                       2048);

            GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwConsoleMode);
            YoriLibSPrintf(szTermVar, _T("color;%s"), dwConsoleMode&ENABLE_WRAP_AT_EOL_OUTPUT?_T(";autolinewrap"):_T(""));
            SetEnvironmentVariable(_T("NTTERM"), szTermVar);

            YoriLibMakeInheritableHandle(hProcessInput, &hProcessInput);
            YoriLibMakeInheritableHandle(hProcessOutput, &hProcessOutput);

            StartupInfo.dwFlags = STARTF_USESTDHANDLES;
            StartupInfo.hStdInput = hProcessInput;
            StartupInfo.hStdOutput = hProcessOutput;
            StartupInfo.hStdError = hProcessOutput;

            SetEnvironmentVariable(_T("PROMPT"), _T("$e[31;1m$p$e[37m$g"));

            if (!CreateProcess(FileName.StartOfString,
                               FileName.StartOfString,
                               NULL,
                               NULL,
                               TRUE,
                               0,
                               NULL,
                               NULL,
                               &StartupInfo,
                               &ProcessInfo)) {
                YoriLibOutput(YORI_LIB_OUTPUT_STDERR, _T("Could not launch process, error %i\n"), (int)GetLastError());
                return EXIT_FAILURE;
            }

            CloseHandle(hProcessInput);
            CloseHandle(hProcessOutput);

            CloseHandle(ProcessInfo.hProcess);
            CloseHandle(ProcessInfo.hThread);

            CreateThread(NULL, 0, CvtvtInputPumpThread, hControl, 0, NULL);
        }
    } else if (UserFileName != NULL) {
        if (!YoriLibUserStringToSingleFilePath(UserFileName, TRUE, &FileName)) {
            return EXIT_FAILURE;
        }

        hSource = CreateFile(FileName.StartOfString, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
        if (hSource == INVALID_HANDLE_VALUE) {
            YoriLibOutput(YORI_LIB_OUTPUT_STDERR, _T("Could not open file, error %i\n"), (int)GetLastError());
            return EXIT_FAILURE;
        }
    } else {
        hSource = GetStdHandle(STD_INPUT_HANDLE);
    }

    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (StripEscapes) {
        DWORD CurrentMode;
        if (GetConsoleMode(hOutput, &CurrentMode)) {
            YoriLibConsoleNoEscapeSetFunctions(&Callbacks);
        } else {
            YoriLibUtf8TextNoEscapesSetFunctions(&Callbacks);
        }
    }

    YoriLibInitEmptyString(&LineString);

    Result = TRUE;
    
    while (YoriLibReadLineToString(&LineString, &LineReadContext, hSource)) {

        //
        //  Start producing HTML
        //

        if (!StreamStarted) {
            Callbacks.InitializeStream(hOutput);
            StreamStarted = TRUE;
        }

        if (!YoriLibProcessVtEscapesOnOpenStream(LineString.StartOfString,
                                                 LineString.LengthInChars,
                                                 hOutput,
                                                 &Callbacks)) {

            Result = FALSE;
            break;
        }

        if (!YoriLibProcessVtEscapesOnOpenStream(_T("\n"),
                                                 1,
                                                 hOutput,
                                                 &Callbacks)) {

            Result = FALSE;
            break;
        }
    }

    if (StreamStarted) {
        Callbacks.EndStream(hOutput);
    }

    YoriLibLineReadClose(LineReadContext);
    YoriLibFreeStringContents(&LineString);
    if (hSource != INVALID_HANDLE_VALUE && UserFileName != NULL) {
        CloseHandle(hSource);
    }
    if (hControl != INVALID_HANDLE_VALUE) {
        CloseHandle(hControl);
    }

    YoriLibFreeStringContents(&FileName);

    if (Result) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

// vim:sw=4:ts=4:et: