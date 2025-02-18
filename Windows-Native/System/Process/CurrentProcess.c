﻿#include "CurrentProcess.h"


BOOL CurrentProcess_DetectDebugger(void)
{
    CONTEXT ctx = { 0 };
    static NTSTATUS(__stdcall* NtGetContextThread)(HANDLE ThreadHandle, PCONTEXT pContext) = NULL;
    if (!NtGetContextThread) NtGetContextThread = NativeLib.Library.GetModuleFunction(L"ntdll.dll", "NtGetContextThread");
    bool isDebugged = NtGetPeb()->BeingDebugged;
    if (isDebugged) return isDebugged;
    isDebugged = NtGetPeb()->ProcessHeap->ForceFlags != 0;
    if (isDebugged) return isDebugged;

    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    if (NT_SUCCESS(NtGetContextThread(NtCurrentThread(), &ctx)))
        isDebugged = ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3 || ctx.Dr6 || ctx.Dr7;
    if (isDebugged) return isDebugged;
    isDebugged = ((PKUSER_SHARED_DATA)0x7FFE0000)->KdDebuggerEnabled;
    //NtQueryInformationProcess() // ProcessDebugPort
    return isDebugged;
}

UINT64 CurrentProcess_GetId(void)
{
    return (UINT64)NtGetPid();
}

struct CurrentProcess CurrentProcess = {
    .GetCurrentId = CurrentProcess_GetId,
    .DetectDebugger = CurrentProcess_DetectDebugger
};
