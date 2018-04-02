/**
 * @file builtins/pushd.c
 *
 * Yori shell push and pop current directories
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
#include <yoricall.h>

/**
 Help text to display to the user.
 */
const
CHAR strPushdHelpText[] =
        "\n"
        "Push the current directory onto a stack and change to a new directory.\n"
        "\n"
        "PUSHD <directory>\n";

/**
 Display usage text to the user.
 */
BOOL
PushdHelp()
{
    YORI_STRING License;

    YoriLibMitLicenseText(_T("2018"), &License);

    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("PushD %i.%i\n"), YORI_VER_MAJOR, YORI_VER_MINOR);
#if YORI_BUILD_ID
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("  Build %i\n"), YORI_BUILD_ID);
#endif
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%hs\n"), strPushdHelpText);
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%y"), &License);
    YoriLibFreeStringContents(&License);
    return TRUE;
}

/**
 Help text to display to the user.
 */
const
CHAR strPopdHelpText[] =
        "\n"
        "Pop a previous current directory from the stack.\n"
        "\n"
        "POPD\n";

/**
 Display usage text to the user.
 */
BOOL
PopdHelp()
{
    YORI_STRING License;

    YoriLibMitLicenseText(_T("2018"), &License);

    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("PopD %i.%i\n"), YORI_VER_MAJOR, YORI_VER_MINOR);
#if YORI_BUILD_ID
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("  Build %i\n"), YORI_BUILD_ID);
#endif
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%hs\n"), strPopdHelpText);
    YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("%y"), &License);
    YoriLibFreeStringContents(&License);
    return TRUE;
}

/**
 Information describing saved state at the time of a pushd call.
 */
typedef struct _PUSHD_STACK {

    /**
     Pointers to the next most recent and previous in time pushd calls.
     */
    YORI_LIST_ENTRY StackLinks;

    /**
     A string containing the current directory at the time pushd was
     executed.
     */
    YORI_STRING PreviousDirectory;
} PUSHD_STACK, *PPUSHD_STACK;

/**
 A linked list of entries describing when pushd was executed.  The tail
 of this list corresponds to the most recent pushd call.
 */
YORI_LIST_ENTRY PushdStack;

/**
 The main entrypoint for the popd command.

 @param ArgC The number of arguments.

 @param ArgV The argument array.

 @return ExitCode, zero for success, nonzero for failure.
 */
DWORD
YORI_BUILTIN_FN
YoriCmd_POPD(
    __in DWORD ArgC,
    __in YORI_STRING ArgV[]
    )
{
    DWORD i;
    BOOL ArgumentUnderstood;
    PYORI_LIST_ENTRY ListEntry;
    PPUSHD_STACK StackLocation;
    YORI_STRING Arg;

    for (i = 1; i < ArgC; i++) {

        ArgumentUnderstood = FALSE;

        if (YoriLibIsCommandLineOption(&ArgV[i], &Arg)) {

            if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("?")) == 0) {
                PopdHelp();
                return EXIT_SUCCESS;
            }
        }

        if (!ArgumentUnderstood) {
            YoriLibOutput(YORI_LIB_OUTPUT_STDERR, _T("Argument not understood, ignored: %y\n"), &ArgV[i]);
        }
    }

    if (PushdStack.Next == NULL) {
        return EXIT_FAILURE;
    }

    ListEntry = YoriLibGetPreviousListEntry(&PushdStack, NULL);
    if (ListEntry == NULL) {
        return EXIT_FAILURE;
    }

    StackLocation = CONTAINING_RECORD(ListEntry, PUSHD_STACK, StackLinks);
    YoriLibRemoveListItem(&StackLocation->StackLinks);

    if (PushdStack.Next == &PushdStack) {
        YORI_STRING PopdCmd;
        YoriLibConstantString(&PopdCmd, _T("POPD"));
        YoriCallBuiltinUnregister(&PopdCmd, YoriCmd_POPD);
    }

    if (!SetCurrentDirectory(StackLocation->PreviousDirectory.StartOfString)) {
        YoriLibFree(StackLocation);
        return EXIT_FAILURE;
    }

    YoriLibFree(StackLocation);
    return EXIT_SUCCESS;
}


/**
 Push a directory onto the stack and switch to a new one.

 @param ArgC The number of arguments.

 @param ArgV The argument array.

 @return ExitCode, zero for success, nonzero for failure.
 */
DWORD
YORI_BUILTIN_FN
YoriCmd_PUSHD(
    __in DWORD ArgC,
    __in YORI_STRING ArgV[]
    )
{
    BOOL ArgumentUnderstood;
    DWORD i;
    DWORD StartArg = 0;
    PPUSHD_STACK NewStackEntry;
    DWORD CurrentDirectoryLength;
    YORI_STRING ChdirCmd;
    YORI_STRING Arg;

    for (i = 1; i < ArgC; i++) {

        ArgumentUnderstood = FALSE;
        ASSERT(YoriLibIsStringNullTerminated(&ArgV[i]));

        if (YoriLibIsCommandLineOption(&ArgV[i], &Arg)) {

            if (YoriLibCompareStringWithLiteralInsensitive(&Arg, _T("?")) == 0) {
                PushdHelp();
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

    if (StartArg == 0) {
        YoriLibOutput(YORI_LIB_OUTPUT_STDOUT, _T("pushd: missing argument\n"));
        return EXIT_FAILURE;
    }

    CurrentDirectoryLength = GetCurrentDirectory(0, NULL);

    NewStackEntry = YoriLibMalloc(sizeof(PUSHD_STACK) + CurrentDirectoryLength * sizeof(TCHAR));
    if (NewStackEntry == NULL) {
        return EXIT_FAILURE;
    }

    YoriLibInitEmptyString(&NewStackEntry->PreviousDirectory);
    NewStackEntry->PreviousDirectory.StartOfString = (LPTSTR)(NewStackEntry + 1);
    NewStackEntry->PreviousDirectory.LengthInChars = GetCurrentDirectory(CurrentDirectoryLength, NewStackEntry->PreviousDirectory.StartOfString);

    //
    //  Invoke chdir to actually change directory.  This provides consistent
    //  path parsing and will handle things like drive switching consistently.
    //

    if (!YoriLibAllocateString(&ChdirCmd, 6 + ArgV[StartArg].LengthInChars + 1)) {
        YoriLibFree(NewStackEntry);
        return EXIT_FAILURE;
    }

    ChdirCmd.LengthInChars = YoriLibSPrintf(ChdirCmd.StartOfString, _T("CHDIR %y"), &ArgV[StartArg]);
    YoriCallExecuteExpression(&ChdirCmd);
    YoriLibFreeStringContents(&ChdirCmd);

    if (PushdStack.Next == NULL) {
        YoriLibInitializeListHead(&PushdStack);
    }
    if (PushdStack.Next == &PushdStack) {
        YORI_STRING PopdCmd;
        YoriLibConstantString(&PopdCmd, _T("POPD"));
        if (!YoriCallBuiltinRegister(&PopdCmd, YoriCmd_POPD)) {
            YoriLibFree(NewStackEntry);
            return EXIT_FAILURE;
        }
    }

    YoriLibAppendList(&PushdStack, &NewStackEntry->StackLinks);

    return EXIT_SUCCESS;
}

// vim:sw=4:ts=4:et: