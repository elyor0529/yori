/**
 * @file lib/call.c
 *
 * Yori call from modules into external API.  Functions in this file can only
 * be called from code running within the Yori process.
 *
 * Copyright (c) 2017-2018 Malcolm J. Smith
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

#include "yoripch.h"
#include "yorilib.h"

/**
 Prototype for the @ref YoriApiAddAlias function.
 */
typedef BOOL YORI_API_ADD_ALIAS(PYORI_STRING, PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiAddAlias function.
 */
typedef YORI_API_ADD_ALIAS *PYORI_API_ADD_ALIAS;

/**
 Pointer to the @ref YoriApiAddAlias function.
 */
PYORI_API_ADD_ALIAS pYoriApiAddAlias;

/**
 Add a new, or replace an existing, shell alias.

 @param Alias The alias to add or update.

 @param Value The value to assign to the alias.

 @return TRUE if the alias was successfully updated, FALSE if it was not.
 */
BOOL
YoriCallAddAlias(
    __in PYORI_STRING Alias,
    __in PYORI_STRING Value
    )
{
    if (pYoriApiAddAlias == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiAddAlias = (PYORI_API_ADD_ALIAS)GetProcAddress(hYori, "YoriApiAddAlias");
        if (pYoriApiAddAlias == NULL) {
            return FALSE;
        }
    }
    return pYoriApiAddAlias(Alias, Value);
}

/**
 Prototype for the @ref YoriApiBuiltinRegister function.
 */
typedef BOOL YORI_API_BUILTIN_REGISTER(PYORI_STRING, PYORI_CMD_BUILTIN);

/**
 Prototype for a pointer to the @ref YoriApiBuiltinRegister function.
 */
typedef YORI_API_BUILTIN_REGISTER *PYORI_API_BUILTIN_REGISTER;

/**
 Pointer to the @ref YoriApiBuiltinRegister function.
 */
PYORI_API_BUILTIN_REGISTER pYoriApiBuiltinRegister;

/**
 Add a new builtin callback.

 @param BuiltinCmd The builtin callback command to register.

 @param CallbackFn The callback function to invoke.

 @return TRUE if the callback was successfully updated, FALSE if it was not.
 */
BOOL
YoriCallBuiltinRegister(
    __in PYORI_STRING BuiltinCmd,
    __in PYORI_CMD_BUILTIN CallbackFn
    )
{
    if (pYoriApiBuiltinRegister == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiBuiltinRegister = (PYORI_API_BUILTIN_REGISTER)GetProcAddress(hYori, "YoriApiBuiltinRegister");
        if (pYoriApiBuiltinRegister == NULL) {
            return FALSE;
        }
    }
    return pYoriApiBuiltinRegister(BuiltinCmd, CallbackFn);
}

/**
 Prototype for the @ref YoriApiBuiltinUnregister function.
 */
typedef BOOL YORI_API_BUILTIN_UNREGISTER(PYORI_STRING, PYORI_CMD_BUILTIN);

/**
 Prototype for a pointer to the @ref YoriApiBuiltinUnregister function.
 */
typedef YORI_API_BUILTIN_UNREGISTER *PYORI_API_BUILTIN_UNREGISTER;

/**
 Pointer to the @ref YoriApiBuiltinUnregister function.
 */
PYORI_API_BUILTIN_UNREGISTER pYoriApiBuiltinUnregister;

/**
 Add a new builtin callback.

 @param BuiltinCmd The builtin callback command to register.

 @param CallbackFn The callback function to invoke.

 @return TRUE if the callback was successfully updated, FALSE if it was not.
 */
BOOL
YoriCallBuiltinUnregister(
    __in PYORI_STRING BuiltinCmd,
    __in PYORI_CMD_BUILTIN CallbackFn
    )
{
    if (pYoriApiBuiltinUnregister == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiBuiltinUnregister = (PYORI_API_BUILTIN_UNREGISTER)GetProcAddress(hYori, "YoriApiBuiltinUnregister");
        if (pYoriApiBuiltinUnregister == NULL) {
            return FALSE;
        }
    }
    return pYoriApiBuiltinUnregister(BuiltinCmd, CallbackFn);
}


/**
 Prototype for the @ref YoriApiDeleteAlias function.
 */
typedef BOOL YORI_API_DELETE_ALIAS(PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiDeleteAlias function.
 */
typedef YORI_API_DELETE_ALIAS *PYORI_API_DELETE_ALIAS;

/**
 Pointer to the @ref YoriApiDeleteAlias function.
 */
PYORI_API_DELETE_ALIAS pYoriApiDeleteAlias;

/**
 Delete an existing shell alias.

 @param Alias The alias to delete.

 @return TRUE if the alias was successfully deleted, FALSE if it was not
         found.
 */
BOOL
YoriCallDeleteAlias(
    __in PYORI_STRING Alias
    )
{
    if (pYoriApiDeleteAlias == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiDeleteAlias = (PYORI_API_DELETE_ALIAS)GetProcAddress(hYori, "YoriApiDeleteAlias");
        if (pYoriApiDeleteAlias == NULL) {
            return FALSE;
        }
    }
    return pYoriApiDeleteAlias(Alias);
}

/**
 Prototype for the @ref YoriApiExecuteBuiltin function.
 */
typedef BOOL YORI_API_EXECUTE_BUILTIN(PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiExecuteBuiltin function.
 */
typedef YORI_API_EXECUTE_BUILTIN *PYORI_API_EXECUTE_BUILTIN;

/**
 Pointer to the @ref YoriApiExecuteBuiltin function.
 */
PYORI_API_EXECUTE_BUILTIN pYoriApiExecuteBuiltin;

/**
 Parse and execute a string as a built in command.

 @param Expression The string to execute.

 @return TRUE to indicate it was successfully executed, FALSE otherwise.
 */
BOOL
YoriCallExecuteBuiltin(
    __in PYORI_STRING Expression
    )
{
    if (pYoriApiExecuteBuiltin == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiExecuteBuiltin = (PYORI_API_EXECUTE_BUILTIN)GetProcAddress(hYori, "YoriApiExecuteBuiltin");
        if (pYoriApiExecuteBuiltin == NULL) {
            return FALSE;
        }
    }
    return pYoriApiExecuteBuiltin(Expression);
}


/**
 Prototype for the @ref YoriApiExecuteExpression function.
 */
typedef BOOL YORI_API_EXECUTE_EXPRESSION(PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiExecuteExpression function.
 */
typedef YORI_API_EXECUTE_EXPRESSION *PYORI_API_EXECUTE_EXPRESSION;

/**
 Pointer to the @ref YoriApiExecuteExpression function.
 */
PYORI_API_EXECUTE_EXPRESSION pYoriApiExecuteExpression;

/**
 Parse and execute a command string.  This will internally perform parsing
 and redirection, as well as execute multiple subprocesses as needed.

 @param Expression The string to execute.

 @return TRUE to indicate it was successfully executed, FALSE otherwise.
 */
BOOL
YoriCallExecuteExpression(
    __in PYORI_STRING Expression
    )
{
    if (pYoriApiExecuteExpression == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiExecuteExpression = (PYORI_API_EXECUTE_EXPRESSION)GetProcAddress(hYori, "YoriApiExecuteExpression");
        if (pYoriApiExecuteExpression == NULL) {
            return FALSE;
        }
    }
    return pYoriApiExecuteExpression(Expression);
}

/**
 Prototype for the @ref YoriApiExpandAlias function.
 */
typedef BOOL YORI_API_EXPAND_ALIAS(PYORI_STRING, PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiExpandAlias function.
 */
typedef YORI_API_EXPAND_ALIAS *PYORI_API_EXPAND_ALIAS;

/**
 Pointer to the @ref YoriApiExpandAlias function.
 */
PYORI_API_EXPAND_ALIAS pYoriApiExpandAlias;

/**
 Expand a command that contains an alias into a new string with the alias
 expanded.

 @param CommandString The string to look for aliases to expand.

 @param ExpandedString The resulting string after alias expansion.

 @return TRUE to indicate expansion was performed, FALSE if it was not.
 */
BOOL
YoriCallExpandAlias(
    __in PYORI_STRING CommandString,
    __in PYORI_STRING ExpandedString
    )
{
    if (pYoriApiExpandAlias == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiExpandAlias = (PYORI_API_EXPAND_ALIAS)GetProcAddress(hYori, "YoriApiExpandAlias");
        if (pYoriApiExpandAlias == NULL) {
            return FALSE;
        }
    }
    return pYoriApiExpandAlias(CommandString, ExpandedString);
}

/**
 Prototype for the @ref YoriApiExitProcess function.
 */
typedef VOID YORI_API_EXIT_PROCESS(DWORD);

/**
 Prototype for a pointer to the @ref YoriApiExitProcess function.
 */
typedef YORI_API_EXIT_PROCESS *PYORI_API_EXIT_PROCESS;

/**
 Pointer to the @ref YoriApiExitProcess function.
 */
PYORI_API_EXIT_PROCESS pYoriApiExitProcess;

/**
 Terminates the currently running instance of Yori.

 @param ExitCode The exit code to return.
 */
VOID
YoriCallExitProcess(
    __in DWORD ExitCode
    )
{
    if (pYoriApiExitProcess == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiExitProcess = (PYORI_API_EXIT_PROCESS)GetProcAddress(hYori, "YoriApiExitProcess");
        if (pYoriApiExitProcess == NULL) {
            return;
        }
    }
    pYoriApiExitProcess(ExitCode);
}


/**
 Prototype for the @ref YoriApiFreeYoriString function.
 */
typedef VOID YORI_API_FREE_YORI_STRING(PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiFreeYoriString function.
 */
typedef YORI_API_FREE_YORI_STRING *PYORI_API_FREE_YORI_STRING;

/**
 Pointer to the @ref YoriApiFreeYoriString function.
 */
PYORI_API_FREE_YORI_STRING pYoriApiFreeYoriString;

/**
 Free a previously returned Yori string.

 @param String Pointer to the string to free.
 */
VOID
YoriCallFreeYoriString(
    __in PYORI_STRING String
    )
{
    if (pYoriApiFreeYoriString == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiFreeYoriString = (PYORI_API_FREE_YORI_STRING)GetProcAddress(hYori, "YoriApiFreeYoriString");
        if (pYoriApiFreeYoriString == NULL) {
            return;
        }
    }
    pYoriApiFreeYoriString(String);
}


/**
 Prototype for the @ref YoriApiGetAliasStrings function.
 */
typedef BOOL YORI_API_GET_ALIAS_STRINGS(PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiGetAliasStrings function.
 */
typedef YORI_API_GET_ALIAS_STRINGS *PYORI_API_GET_ALIAS_STRINGS;

/**
 Pointer to the @ref YoriApiGetAliasStrings function.
 */
PYORI_API_GET_ALIAS_STRINGS pYoriApiGetAliasStrings;

/**
 Build the complete set of aliases into a an array of key value pairs
 and return a pointer to the result.  This must be freed with a
 subsequent call to @ref YoriCallFreeYoriString .

 @param AliasStrings On successful completion, populated with alias strings.

 @return TRUE to indicate success, or FALSE to indicate failure.
 */
BOOL
YoriCallGetAliasStrings(
    __out PYORI_STRING AliasStrings
    )
{
    if (pYoriApiGetAliasStrings == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetAliasStrings = (PYORI_API_GET_ALIAS_STRINGS)GetProcAddress(hYori, "YoriApiGetAliasStrings");
        if (pYoriApiGetAliasStrings == NULL) {
            return FALSE;
        }
    }
    return pYoriApiGetAliasStrings(AliasStrings);
}

/**
 Prototype for the @ref YoriApiGetErrorLevel function.
 */
typedef DWORD YORI_API_GET_ERRORLEVEL();

/**
 Prototype for a pointer to the @ref YoriApiGetErrorLevel function.
 */
typedef YORI_API_GET_ERRORLEVEL *PYORI_API_GET_ERRORLEVEL;

/**
 Pointer to the @ref YoriApiGetErrorLevel function.
 */
PYORI_API_GET_ERRORLEVEL pYoriApiGetErrorLevel;

/**
 Return the previous errorlevel.

 @return The previous errorlevel.
 */
DWORD
YoriCallGetErrorLevel(
    )
{
    if (pYoriApiGetErrorLevel == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetErrorLevel = (PYORI_API_GET_ERRORLEVEL)GetProcAddress(hYori, "YoriApiGetErrorLevel");
        if (pYoriApiGetErrorLevel == NULL) {
            return 1;
        }
    }
    return pYoriApiGetErrorLevel();
}

/**
 Prototype for the @ref YoriApiGetJobInformation function.
 */
typedef BOOL YORI_API_GET_JOB_INFORMATION(DWORD, PBOOL, PBOOL, PDWORD, PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiGetJobInformation function.
 */
typedef YORI_API_GET_JOB_INFORMATION *PYORI_API_GET_JOB_INFORMATION;

/**
 Pointer to the @ref YoriApiGetJobInformation function.
 */
PYORI_API_GET_JOB_INFORMATION pYoriApiGetJobInformation;

/**
 Returns information associated with an executing or completed job ID.

 @param JobId The ID to query information for.

 @param HasCompleted On successful completion, set to TRUE if the job has
        finished executing; FALSE if the job is still executing.

 @param HasOutput On successful completion, set to TRUE if the job has
        output buffers retained that can be displayed as needed.
        This is only meaningful if the job has completed.

 @param ExitCode On successful completion, once the job has completed,
        contains the exit code from the job.

 @param Command On successful completion, updated to contain the command
        line used to initiate the job.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriCallGetJobInformation(
    __in DWORD JobId,
    __out PBOOL HasCompleted,
    __out PBOOL HasOutput,
    __out PDWORD ExitCode,
    __inout PYORI_STRING Command
    )
{
    if (pYoriApiGetJobInformation == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetJobInformation = (PYORI_API_GET_JOB_INFORMATION)GetProcAddress(hYori, "YoriApiGetJobInformation");
        if (pYoriApiGetJobInformation == NULL) {
            return FALSE;
        }
    }
    return pYoriApiGetJobInformation(JobId, HasCompleted, HasOutput, ExitCode, Command);
}

/**
 Prototype for the @ref YoriApiGetJobOutput function.
 */
typedef BOOL YORI_API_GET_JOB_OUTPUT(DWORD, PYORI_STRING, PYORI_STRING);

/**
 Prototype for a pointer to the @ref YoriApiGetJobOutput function.
 */
typedef YORI_API_GET_JOB_OUTPUT *PYORI_API_GET_JOB_OUTPUT;

/**
 Pointer to the @ref YoriApiGetJobOutput function.
 */
PYORI_API_GET_JOB_OUTPUT pYoriApiGetJobOutput;

/**
 Given a job ID, return the output buffer associated with the job.

 @param JobId The job ID to query the output buffer for.

 @param Output On successful completion, populated with the job output.

 @param Errors On successful completion, populated with the job errors.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriCallGetJobOutput(
    __in DWORD JobId,
    __out PYORI_STRING Output,
    __out PYORI_STRING Errors
    )
{
    if (pYoriApiGetJobOutput == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetJobOutput = (PYORI_API_GET_JOB_OUTPUT)GetProcAddress(hYori, "YoriApiGetJobOutput");
        if (pYoriApiGetJobOutput == NULL) {
            return 0;
        }
    }
    return pYoriApiGetJobOutput(JobId, Output, Errors);
}

/**
 Prototype for the @ref YoriApiGetNextJobId function.
 */
typedef DWORD YORI_API_GET_NEXT_JOB_ID(DWORD);

/**
 Prototype for a pointer to the @ref YoriApiGetNextJobId function.
 */
typedef YORI_API_GET_NEXT_JOB_ID *PYORI_API_GET_NEXT_JOB_ID;

/**
 Pointer to the @ref YoriApiGetNextJobId function.
 */
PYORI_API_GET_NEXT_JOB_ID pYoriApiGetNextJobId;

/**
 Given a previous job ID, return the next ID that is currently executing.
 To commence from the beginning, specify a PreviousJobId of zero.

 @param PreviousJobId The ID that was previously returned, or zero to
        commence from the beginning.

 @return The next job ID, or zero if all have been enumerated.
 */
DWORD
YoriCallGetNextJobId(
    __in DWORD PreviousJobId
    )
{
    if (pYoriApiGetNextJobId == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetNextJobId = (PYORI_API_GET_NEXT_JOB_ID)GetProcAddress(hYori, "YoriApiGetNextJobId");
        if (pYoriApiGetNextJobId == NULL) {
            return 0;
        }
    }
    return pYoriApiGetNextJobId(PreviousJobId);
}

/**
 Prototype for the @ref YoriApiGetYoriVersion function.
 */
typedef BOOL YORI_API_GET_YORI_VERSION(PDWORD, PDWORD);

/**
 Prototype for a pointer to the @ref YoriApiGetYoriVersion function.
 */
typedef YORI_API_GET_YORI_VERSION *PYORI_API_GET_YORI_VERSION;

/**
 Pointer to the @ref YoriApiGetYoriVersion function.
 */
PYORI_API_GET_YORI_VERSION pYoriApiGetYoriVersion;

/**
 Returns the version number associated with this build of the Yori shell.

 @param MajorVersion On successful completion, updated to contain the major
        version number.

 @param MinorVersion On successful completion, updated to contain the minor
        version number.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriCallGetYoriVersion(
    __out PDWORD MajorVersion,
    __out PDWORD MinorVersion
    )
{
    if (pYoriApiGetYoriVersion == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiGetYoriVersion = (PYORI_API_GET_YORI_VERSION)GetProcAddress(hYori, "YoriApiGetYoriVersion");
        if (pYoriApiGetYoriVersion == NULL) {
            return FALSE;
        }
    }
    return pYoriApiGetYoriVersion(MajorVersion, MinorVersion);
}

/**
 Prototype for the @ref YoriApiPipeJobOutput function.
 */
typedef BOOL YORI_API_PIPE_JOB_OUTPUT(DWORD, HANDLE, HANDLE);

/**
 Prototype for a pointer to the @ref YoriApiPipeJobOutput function.
 */
typedef YORI_API_PIPE_JOB_OUTPUT *PYORI_API_PIPE_JOB_OUTPUT;

/**
 Pointer to the @ref YoriApiPipeJobOutput function.
 */
PYORI_API_PIPE_JOB_OUTPUT pYoriApiPipeJobOutput;

/**
 Given a job ID, push data in the output buffers associated with a job
 through a pipe.

 @param JobId The job ID to query the output buffer for.

 @param hPipeOutput Writable end of a pipe to push output buffers through.

 @param hPipeErrors Writable end of a pipe to push errors through.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriCallPipeJobOutput(
    __in DWORD JobId,
    __in_opt HANDLE hPipeOutput,
    __in_opt HANDLE hPipeErrors
    )
{
    if (pYoriApiPipeJobOutput == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiPipeJobOutput = (PYORI_API_GET_JOB_OUTPUT)GetProcAddress(hYori, "YoriApiPipeJobOutput");
        if (pYoriApiPipeJobOutput == NULL) {
            return 0;
        }
    }
    return pYoriApiPipeJobOutput(JobId, hPipeOutput, hPipeErrors);
}


/**
 Prototype for the YoriApiSetDefaultColor function.
 */
typedef VOID YORI_API_SET_DEFAULT_COLOR(WORD);

/**
 Prototype for a pointer to the YoriApiSetDefaultColor function.
 */
typedef YORI_API_SET_DEFAULT_COLOR *PYORI_API_SET_DEFAULT_COLOR;

/**
 Pointer to the @ref YoriApiSetDefaultColor function.
 */
PYORI_API_SET_DEFAULT_COLOR pYoriApiSetDefaultColor;

/**
 Set the default color for the process.  The default color is the one that
 will be used when a reset command is issued to the terminal.  For most
 processes, this is the color that was active when the process started,
 but it can be changed later with this function.

 @param NewDefaultColor The new default color, in Win32 format.

 @return TRUE to indicate success, FALSE to indicate failure.
 */
BOOL
YoriCallSetDefaultColor(
    __in WORD NewDefaultColor
    )
{
    if (pYoriApiSetDefaultColor == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiSetDefaultColor = (PYORI_API_SET_DEFAULT_COLOR)GetProcAddress(hYori, "YoriApiSetDefaultColor");
        if (pYoriApiSetDefaultColor == NULL) {
            return FALSE;
        }
    }
    pYoriApiSetDefaultColor(NewDefaultColor);
    return TRUE;
}

/**
 Prototype for the @ref YoriApiSetJobPriority function.
 */
typedef BOOL YORI_API_SET_JOB_PRIORITY(DWORD, DWORD);

/**
 Prototype for a pointer to the @ref YoriApiSetJobPriority function.
 */
typedef YORI_API_SET_JOB_PRIORITY *PYORI_API_SET_JOB_PRIORITY;

/**
 Pointer to the @ref YoriApiSetJobPriority function.
 */
PYORI_API_SET_JOB_PRIORITY pYoriApiSetJobPriority;

/**
 Sets the priority associated with a job.

 @param JobId The ID to set priority for.

 @param PriorityClass The new priority class.

 @return TRUE to indicate that the priority class was changed, FALSE if it
         was not.
 */
BOOL
YoriCallSetJobPriority(
    __in DWORD JobId,
    __in DWORD PriorityClass
    )
{
    if (pYoriApiSetJobPriority == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiSetJobPriority = (PYORI_API_SET_JOB_PRIORITY)GetProcAddress(hYori, "YoriApiSetJobPriority");
        if (pYoriApiSetJobPriority == NULL) {
            return FALSE;
        }
    }
    return pYoriApiSetJobPriority(JobId, PriorityClass);
}

/**
 Prototype for the @ref YoriApiTerminateJob function.
 */
typedef BOOL YORI_API_TERMINATE_JOB(DWORD);

/**
 Prototype for a pointer to the @ref YoriApiTerminateJob function.
 */
typedef YORI_API_TERMINATE_JOB *PYORI_API_TERMINATE_JOB;

/**
 Pointer to the @ref YoriApiTerminateJob function.
 */
PYORI_API_TERMINATE_JOB pYoriApiTerminateJob;

/**
 Terminates a specified job.

 @param JobId The job to terminate.

 @return TRUE to indicate that the job was requested to terminate, FALSE if it
         was not.
 */
BOOL
YoriCallTerminateJob(
    __in DWORD JobId
    )
{
    if (pYoriApiTerminateJob == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiTerminateJob = (PYORI_API_TERMINATE_JOB)GetProcAddress(hYori, "YoriApiTerminateJob");
        if (pYoriApiTerminateJob == NULL) {
            return FALSE;
        }
    }
    return pYoriApiTerminateJob(JobId);
}

/**
 Prototype for the @ref YoriApiWaitForJob function.
 */
typedef BOOL YORI_API_WAIT_FOR_JOB(DWORD);

/**
 Prototype for a pointer to the @ref YoriApiWaitForJob function.
 */
typedef YORI_API_WAIT_FOR_JOB *PYORI_API_WAIT_FOR_JOB;

/**
 Pointer to the @ref YoriApiWaitForJob function.
 */
PYORI_API_WAIT_FOR_JOB pYoriApiWaitForJob;

/**
 Waits until the specified job ID is no longer active.

 @param JobId The ID to wait for.
 */
VOID
YoriCallWaitForJob(
    __in DWORD JobId
    )
{
    if (pYoriApiWaitForJob == NULL) {
        HMODULE hYori;

        hYori = GetModuleHandle(NULL);
        pYoriApiWaitForJob = (PYORI_API_WAIT_FOR_JOB)GetProcAddress(hYori, "YoriApiWaitForJob");
        if (pYoriApiWaitForJob == NULL) {
            return;
        }
    }
    pYoriApiWaitForJob(JobId);
}

// vim:sw=4:ts=4:et: