/**
 * @file lib/env.c
 *
 * Yori lib environment variable control
 *
 * Copyright (c) 2017 Malcolm J. Smith
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
 Prototype for the FreeEnvironmentStringsW function.
 */
typedef BOOL FREE_ENVIRONMENT_STRINGSW(LPTSTR);

/**
 Prototype for a pointer to the FreeEnvironmentStringsW function.
 */
typedef FREE_ENVIRONMENT_STRINGSW *PFREE_ENVIRONMENT_STRINGSW;

/**
 Prototype for the GetEnvironmentStringsW function.
 */
typedef LPTSTR GET_ENVIRONMENT_STRINGSW();

/**
 Prototype for a pointer to the GetEnvironmentStringsW function.
 */
typedef GET_ENVIRONMENT_STRINGSW *PGET_ENVIRONMENT_STRINGSW;

/**
 An implementation of GetEnvironmentStrings that can do appropriate dances
 to work with versions of Windows supporting free, and versions that don't
 support free, as well as those with a W suffix and those without.

 @param EnvStrings On successful completion, populated with the array of
        environment strings.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriLibGetEnvironmentStrings(
    __out PYORI_STRING EnvStrings
    )
{
    LPTSTR OsEnvStrings;
    LPSTR OsEnvStringsA;
    PFREE_ENVIRONMENT_STRINGSW pFreeEnvironmentStringsW;
    PGET_ENVIRONMENT_STRINGSW pGetEnvironmentStringsW;
    DWORD CharCount;

    //
    //  Use GetEnvironmentStringsW where it exists.  Where it doesn't
    //  exist (NT 3.1) we need to upconvert to Unicode.
    //

    pGetEnvironmentStringsW = (PGET_ENVIRONMENT_STRINGSW)GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")), "GetEnvironmentStringsW");
    pFreeEnvironmentStringsW = (PFREE_ENVIRONMENT_STRINGSW)GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")), "FreeEnvironmentStringsW");

    if (pGetEnvironmentStringsW != NULL) {
        OsEnvStrings = pGetEnvironmentStringsW();
        for (CharCount = 0; OsEnvStrings[CharCount] != '\0' || OsEnvStrings[CharCount + 1] != '\0'; CharCount++);

        CharCount += 2;
        if (!YoriLibAllocateString(EnvStrings, CharCount)) {
            if (pFreeEnvironmentStringsW) {
                pFreeEnvironmentStringsW(OsEnvStrings);
            }
            return FALSE;
        }

        CopyMemory(EnvStrings->StartOfString, OsEnvStrings, CharCount * sizeof(TCHAR));

        if (pFreeEnvironmentStringsW) {
            pFreeEnvironmentStringsW(OsEnvStrings);
        }

        return TRUE;
    }

    pGetEnvironmentStringsW = (PGET_ENVIRONMENT_STRINGSW)GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")), "GetEnvironmentStrings");
    OsEnvStringsA = (LPSTR)pGetEnvironmentStringsW();

    for (CharCount = 0; OsEnvStringsA[CharCount] != '\0' || OsEnvStringsA[CharCount + 1] != '\0'; CharCount++);

    CharCount += 2;
    if (!YoriLibAllocateString(EnvStrings, CharCount)) {
        return FALSE;
    }
    MultiByteToWideChar(CP_ACP, 0, OsEnvStringsA, CharCount, EnvStrings->StartOfString, CharCount);

    return TRUE;
}

// vim:sw=4:ts=4:et: