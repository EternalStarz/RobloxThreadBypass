#pragma once

struct _OBJECT_ATTRIBUTES
{
    ULONG Length;                                                           
    VOID* RootDirectory;                                                    
    struct _UNICODE_STRING* ObjectName;                                     
    ULONG Attributes;                                                       
    VOID* SecurityDescriptor;                                               
    VOID* SecurityQualityOfService;                                        
};


struct _CLIENT_ID
{
    HANDLE UniqueProcess;                                                   
    HANDLE UniqueThread;                                                     
};

typedef enum THREADINFO {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger
};

typedef NTSYSAPI NTSTATUS(NTAPI* _NtOpenThread)(PHANDLE hThread, ACCESS_MASK DesiredAccess, _OBJECT_ATTRIBUTES* Objectattributes, _CLIENT_ID* pCLientId);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtSuspendThread)(HANDLE hThread, PULONG suspendcount);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtQueryInformationThread)(
    HANDLE          ThreadHandle,
    THREADINFO InformationClass,
    PVOID           ThreadInformation,
    ULONG           ThreadInformationLength,
    PULONG          ReturnLength_
);