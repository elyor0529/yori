/*
 * YORI.H
 *
 * Yori shell master header file
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
 Information about each argument in an enumerated list of arguments.
 */
typedef struct _YORI_ARG_CONTEXT {

    /**
     TRUE if the argument is enclosed in quotes.
     */
    BOOL Quoted;

} YORI_ARG_CONTEXT, *PYORI_ARG_CONTEXT;

/**
 A command line that has been broken up into a series of arguments.
 */
typedef struct _YORI_CMD_CONTEXT {

    /**
     The number of arguments.
     */
    DWORD argc;

    /**
     An array of pointers to each argument.  Each of these arguments has been
     referenced and should be dereferenced when no longer needed.
     */
    PYORI_STRING ysargv;

    /**
     An array of information about each argument, including the object that
     was referenced for each.
     */
    PYORI_ARG_CONTEXT ArgContexts;

    /**
     When generating the command context, if a string offset is specified,
     this value contains the argument that the string offset would correspond
     to.
     */
    DWORD CurrentArg;

    /**
     Memory to dereference when the context is torn down.  Typically this
     single allocation backs the argv and ArgContexts array, and often backs
     the contents of each of the arguments also.
     */
    PVOID MemoryToFree;
} YORI_CMD_CONTEXT, *PYORI_CMD_CONTEXT;

/**
 Information about how to execute a single program.  The program may be
 internal or external.
 */
typedef struct _YORI_SINGLE_EXEC_CONTEXT {

    /**
     The set of arguments to invoke the program with.
     */
    YORI_CMD_CONTEXT CmdToExec;

    /**
     Pointer to the next program in an execution chain or NULL if there is
     no next program.
     */
    struct _YORI_SINGLE_EXEC_CONTEXT * NextProgram;

    /**
     Specifies the type of the next program and the conditions under which
     it should execute.
     */
    enum {
        NextProgramTypeNone = 0,
        NextProgramExecUnconditionally = 1,
        NextProgramExecConcurrently = 2,
        NextProgramExecOnFailure = 3,
        NextProgramExecOnSuccess = 4,
        NextProgramExecNever = 5
    } NextProgramType;

    /**
     Specifies the origin of stdin when invoking the program.
     */
    enum {
        StdInTypeDefault = 1,
        StdInTypeFile = 2,
        StdInTypeNull = 3,
        StdInTypePipe = 4
    } StdInType;

    /**
     Extra information specific to each type of stdin origin.
     */
    union {
        struct {
            YORI_STRING FileName;
        } File;
        struct {
            HANDLE PipeFromPriorProcess;
        } Pipe;
    } StdIn;

    /**
     Specifies the target of stdout when invoking the program.
     */
    enum {
        StdOutTypeDefault = 1,
        StdOutTypeOverwrite = 2,
        StdOutTypeAppend = 3,
        StdOutTypeNull = 4,
        StdOutTypePipe = 5,
        StdOutTypeBuffer = 6
    } StdOutType;

    /**
     Extra information specific to each type of stdout target.
     */
    union {
        struct {
            YORI_STRING FileName;
        } Overwrite;
        struct {
            YORI_STRING FileName;
        } Append;
        struct {
            HANDLE PipeFromProcess;
            PVOID ProcessBuffers;
            BOOLEAN RetainBufferData;
        } Buffer;
    } StdOut;

    /**
     Specifies the target of stderr when invoking the program.
     */
    enum {
        StdErrTypeDefault = 1,
        StdErrTypeOverwrite = 2,
        StdErrTypeAppend = 3,
        StdErrTypeStdOut = 4,
        StdErrTypeNull = 5,
        StdErrTypeBuffer = 6
    } StdErrType;

    /**
     Extra information specific to each type of stderr target.
     */
    union {
        struct {
            YORI_STRING FileName;
        } Overwrite;
        struct {
            YORI_STRING FileName;
        } Append;
        struct {
            HANDLE PipeFromProcess;
            PVOID ProcessBuffers;
            BOOLEAN RetainBufferData;
        } Buffer;
    } StdErr;

    /**
     TRUE if when the program is executed we should wait for it to complete.
     If FALSE, execution can resume immediately, either executing the next
     program or returning to the user for more input.

     MSFIX This needs to be split between individual subcommand and entire
     string.  Depending on the operator, either the next command should be
     launched immediately or only after the previous one has completed,
     but this has nothing to do with returning to the shell.
     */
    BOOLEAN WaitForCompletion;

    /**
     TRUE if the program should be executed on a different console to the
     one the user is operating on.

     MSFIX This can't really be a property of an individual command, it has
     to be a property of the set; but if it is, we need a subshell to do the
     work because a single process can only be associated with a single
     console.
     */
    BOOLEAN RunOnSecondConsole;

} YORI_SINGLE_EXEC_CONTEXT, *PYORI_SINGLE_EXEC_CONTEXT;

/**
 When programs are executed, they temporarily modify the stdin/stdout/stderr
 of the shell process.  This structure contains information needed to revert
 back to the previous behavior.
 */
typedef struct _YORI_PREVIOUS_REDIRECT_CONTEXT {

    /**
     TRUE if stdin needs to be reset.
     */
    BOOLEAN ResetInput;

    /**
     TRUE if stdout needs to be reset.
     */
    BOOLEAN ResetOutput;

    /**
     TRUE if stderr needs to be reset.
     */
    BOOLEAN ResetError;

    /**
     TRUE if stdout and stderr have been modified to refer to the same
     location.
     */
    BOOLEAN StdErrAndOutSame;

    /**
     A handle to the original stdin.
     */
    HANDLE StdInput;

    /**
     A handle to the original stdout.
     */
    HANDLE StdOutput;

    /**
     A handle to the original stderr.
     */
    HANDLE StdError;
} YORI_PREVIOUS_REDIRECT_CONTEXT, *PYORI_PREVIOUS_REDIRECT_CONTEXT;

/**
 A plan to execute multiple programs.
 */
typedef struct _YORI_EXEC_PLAN {

    /**
     Pointer to the first program to execute.  It will link to subsequent
     programs to execute.
     */
    PYORI_SINGLE_EXEC_CONTEXT FirstCmd;

    /**
     The total number of programs in the program list.
     */
    ULONG NumberCommands;
} YORI_EXEC_PLAN, *PYORI_EXEC_PLAN;

/**
 Information about a previous command executed by the user.
 */
typedef struct _YORI_HISTORY_ENTRY {

    /**
     The links for this history entry.
     */
    YORI_LIST_ENTRY ListEntry;

    /**
     The command that was executed by the user.
     */
    YORI_STRING CmdLine;
} YORI_HISTORY_ENTRY, *PYORI_HISTORY_ENTRY;

/**
 List of command history.
 */
extern YORI_LIST_ENTRY YoriShCommandHistory;

/**
 Number of elements in command history.
 */
extern DWORD YoriShCommandHistoryCount;

/**
 The exit code ("error level") of the previous process to complete.
 */
extern DWORD g_ErrorLevel;

extern DWORD g_PreviousJobId;

extern BOOL g_ExitProcess;

extern DWORD g_ExitProcessExitCode;

// *** ALIAS.C ***

BOOL
YoriShAddAlias(
    __in PYORI_STRING Alias,
    __in PYORI_STRING Value,
    __in BOOL Internal
    );

BOOL
YoriShAddAliasLiteral(
    __in LPTSTR Alias,
    __in LPTSTR Value,
    __in BOOL Internal
    );

BOOL
YoriShDeleteAlias(
    __in PYORI_STRING Alias
    );

BOOL
YoriShExpandAlias(
    __inout PYORI_CMD_CONTEXT CmdContext
    );

BOOL
YoriShExpandAliasFromString(
    __in PYORI_STRING CommandString,
    __out PYORI_STRING ExpandedString
    );

VOID
YoriShClearAllAliases();

BOOL
YoriShGetAliasStrings(
    __in BOOL IncludeInternal,
    __in PYORI_STRING AliasStrings
    );

BOOL
YoriShLoadSystemAliases();

// *** BUILTIN.C ***

extern CONST LPTSTR YoriShBuiltins;

DWORD
YoriShBuckPass (
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext,
    __in DWORD ExtraArgCount,
    ...
    );

BOOL
YoriShExecuteNamedModuleInProc(
    __in LPTSTR ModuleFileName,
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext,
    __out PDWORD ExitCode
    );

DWORD
YoriShBuiltIn (
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

BOOL
YoriShExecuteBuiltinString(
    __in PYORI_STRING Expression
    );

BOOL
YoriShBuiltinRegister(
    __in PYORI_STRING BuiltinCmd,
    __in PYORI_CMD_BUILTIN CallbackFn
    );

BOOL
YoriShBuiltinUnregister(
    __in PYORI_STRING BuiltinCmd,
    __in PYORI_CMD_BUILTIN CallbackFn
    );

// *** CLIP.C ***

BOOL
YoriShPasteText(
    __inout PYORI_STRING Buffer
    );

// *** CMDBUF.C ***

BOOL
YoriShCreateNewProcessBuffer(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

BOOL
YoriShAppendToExistingProcessBuffer(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

BOOL
YoriShForwardProcessBufferToNextProcess(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

VOID
YoriShDereferenceProcessBuffer(
    __in PVOID ThisBuffer
    );

VOID
YoriShReferenceProcessBuffer(
    __in PVOID ThisBuffer
    );

BOOL
YoriShGetProcessOutputBuffer(
    __in PVOID ThisBuffer,
    __out PYORI_STRING String
    );

BOOL
YoriShGetProcessErrorBuffer(
    __in PVOID ThisBuffer,
    __out PYORI_STRING String
    );

BOOL
YoriShScanProcessBuffersForTeardown();

BOOL
YoriShWaitForProcessBufferToFinalize(
    __in PVOID ThisBuffer
    );

BOOL
YoriShPipeProcessBuffers(
    __in PVOID ThisBuffer,
    __in HANDLE hPipeOutput,
    __in HANDLE hPipeErrors
    );

// *** ENV.C ***

BOOL
YoriShIsEnvironmentVariableChar(
    __in TCHAR Char
    );

DWORD
YoriShGetEnvironmentVariable(
    __in LPCTSTR Name,
    __out_opt LPTSTR Variable,
    __in DWORD Size
    );

BOOL
YoriShExpandEnvironmentVariables(
    __in PYORI_STRING Expression,
    __out PYORI_STRING ResultingExpression
    );

// *** EXEC.C ***
BOOL
YoriShInitializeRedirection(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext,
    __in BOOL PrepareForBuiltIn,
    __out PYORI_PREVIOUS_REDIRECT_CONTEXT PreviousRedirectContext
    );

VOID
YoriShRevertRedirection(
    __in PYORI_PREVIOUS_REDIRECT_CONTEXT PreviousRedirectContext
    );

DWORD
YoriShExecuteSingleProgram(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

BOOL
YoriShExpandBackquotes(
    __in PYORI_STRING Expression,
    __out PYORI_STRING ResultingExpression
    );

BOOL
YoriShExecuteExpression(
    __in PYORI_STRING Expression
    );

// *** HISTORY.C ***

BOOL
YoriShAddToHistory(
    __in PYORI_STRING NewCmd
    );

VOID
YoriShClearAllHistory();

BOOL
YoriShLoadHistoryFromFile();

BOOL
YoriShSaveHistoryToFile();

// *** INPUT.C ***

BOOL
YoriShGetExpression(
    __inout PYORI_STRING Expression
    );

// *** JOB.C ***

BOOL
YoriShCreateNewJob(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext,
    __in HANDLE hProcess,
    __in DWORD dwProcessId
    );

BOOL
YoriShScanJobsReportCompletion();

DWORD
YoriShGetNextJobId(
    __in DWORD PreviousJobId
    );

BOOL
YoriShJobSetPriority(
    __in DWORD JobId,
    __in DWORD PriorityClass
    );

BOOL
YoriShTerminateJob(
    __in DWORD JobId
    );

VOID
YoriShJobWait(
    __in DWORD JobId
    );

BOOL
YoriShGetJobOutput(
    __in DWORD JobId,
    __inout PYORI_STRING Output,
    __inout PYORI_STRING Errors
    );

BOOL
YoriShPipeJobOutput(
    __in DWORD JobId,
    __in_opt HANDLE hPipeOutput,
    __in_opt HANDLE hPipeErrors
    );

BOOL
YoriShGetJobInformation(
    __in DWORD JobId,
    __out PBOOL HasCompleted,
    __out PBOOL HasOutput,
    __out PDWORD ExitCode,
    __inout PYORI_STRING Command
    );

// *** PARSE.C ***

BOOL
YoriShParseCmdlineToCmdContext(
    __in PYORI_STRING CmdLine,
    __in DWORD CurrentOffset,
    __out PYORI_CMD_CONTEXT CmdContext
    );

LPTSTR
YoriShBuildCmdlineFromCmdContext(
    __in PYORI_CMD_CONTEXT CmdContext,
    __in BOOL RemoveEscapes,
    __out_opt PDWORD BeginCurrentArg,
    __out_opt PDWORD EndCurrentArg
    );

BOOL
YoriShRemoveEscapesFromCmdContext(
    __in PYORI_CMD_CONTEXT CmdContext
    );

VOID
YoriShCopyArg(
    __in PYORI_CMD_CONTEXT SrcCmdContext,
    __in DWORD SrcArgument,
    __in PYORI_CMD_CONTEXT DestCmdContext,
    __in DWORD DestArgument
    );

VOID
YoriShCheckIfArgNeedsQuotes(
    __in PYORI_CMD_CONTEXT CmdContext,
    __in DWORD ArgIndex
    );

VOID
YoriShFreeCmdContext(
    __in PYORI_CMD_CONTEXT CmdContext
    );

DWORD
YoriShParseCmdContextToExecContext(
    __in PYORI_CMD_CONTEXT CmdContext,
    __in ULONG InitialArgument,
    __out PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

VOID
YoriShFreeExecContext(
    __in PYORI_SINGLE_EXEC_CONTEXT ExecContext
    );

VOID
YoriShFreeExecPlan(
    __in PYORI_EXEC_PLAN ExecPlan
    );

BOOL
YoriShParseCmdContextToExecPlan(
    __in PYORI_CMD_CONTEXT CmdContext,
    __out PYORI_EXEC_PLAN ExecPlan
    );

BOOL
YoriShDoesExpressionSpecifyPath(
    __in PYORI_STRING SearchFor
    );

// *** PROMPT.C ***
BOOL
YoriShDisplayPrompt();

#ifndef ERROR_ELEVATION_REQUIRED
/**
 Define for the error indicating than an executable needs to be launched with
 ShellExecute so the user can be prompted for elevation.
 */
#define ERROR_ELEVATION_REQUIRED 740
#endif

// vim:sw=4:ts=4:et: