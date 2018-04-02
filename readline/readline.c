/**
 * @file readline/readline.c
 *
 * Yori shell input a line of text
 *
 * Copyright (c) 2018 Malcolm J. Smith
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

#include <yoripch.h>
#include <yorilib.h>

/**
 Help text to display to the user.
 */
const
CHAR strHelpText[] =
        "\n"
        "Inputs a line and sends it to output.\n"
        "\n"
        "READLINE\n"
        "\n";

/**
 Display usage text to the user.
 */
BOOL
ReadLineHelp()
{
    YORI_STRING License;

    YoriLibMitLicenseText(_T("2018"), &License);

    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("ReadLine %i.%i\n"), READLINE_VER_MAJOR, READLINE_VER_MINOR);
#if YORI_BUILD_ID
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("  Build %i\n"), YORI_BUILD_ID);
#endif
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%hs\n"), strHelpText);
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%y"), &License);
    YoriLibFreeStringContents(&License);
    return TRUE;
}


/**
 The main entrypoint for the readline cmdlet.

 @param ArgC The number of arguments.

 @param ArgV An array of arguments.

 @return Exit code of the process indicating success or failure.
 */
DWORD
ymain(
    __in DWORD ArgC,
    __in YORI_STRING ArgV[]
    )
{
    BOOL ArgumentUnderstood;
    DWORD i;
    DWORD StartArg = 1;
    DWORD CurrentMode;
    YORI_STRING InputString;
    YORI_STRING Arg;

    for (i = 1; i < ArgC; i++) {

        ArgumentUnderstood = FALSE;
        ASSERT(YoriLibIsStringNullTerminated(&ArgV[i]));

        if (YoriLibIsCommandLineOption(&ArgV[i], &Arg)) {

            if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("?")) == 0) {
                ReadLineHelp();
                return EXIT_SUCCESS;
            }
        } else {
            ArgumentUnderstood = TRUE;
            StartArg = i;
            break;
        }

        if (!ArgumentUnderstood) {
            YoriLibOutput(YORI_LIB_OUTPUT_STDERR, _T("Argument not understood, ignored: %y\n"), &ArgV[i]);
        }
    }

    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &CurrentMode)) {
        YoriLibOutput(YORI_LIB_OUTPUT_STDERR, _T("readline: cannot read from input device\n"));
        return EXIT_FAILURE;
    }

    CurrentMode |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT;

    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), CurrentMode);

    if (!YoriLibAllocateString(&InputString, 4096)) {
        return EXIT_FAILURE;
    }

    if (!ReadConsole(GetStdHandle(STD_INPUT_HANDLE), InputString.StartOfString, InputString.LengthAllocated, &InputString.LengthInChars, NULL)) {
        return EXIT_FAILURE;
    }

    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%y"), &InputString);

    return EXIT_SUCCESS;
}

// vim:sw=4:ts=4:et: