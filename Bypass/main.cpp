#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "NTAPI.h"

_NtQueryInformationThread NtQueryInformationThread = nullptr;
_NtOpenThread NtOpenThread = nullptr;
_NtSuspendThread NtSuspendThread = nullptr;

bool GetFunctions()
{
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");

    if (ntdll > 0) {
        NtQueryInformationThread = (_NtQueryInformationThread)GetProcAddress(ntdll, "NtQueryInformationThread");
        NtOpenThread = (_NtOpenThread)GetProcAddress(ntdll, "NtOpenThread");
        NtSuspendThread = (_NtSuspendThread)GetProcAddress(ntdll, "NtSuspendThread");
    }

    if (NtQueryInformationThread == nullptr || NtOpenThread == nullptr || NtSuspendThread == nullptr) {
        return false;
    }
    return true;
}

DWORD GetRobloxPID()
{
    HANDLE hSnap = NULL;
    DWORD ProcId = NULL;
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hSnap != NULL)
    {
        PROCESSENTRY32 pEntry;
        pEntry.dwSize = sizeof(pEntry);

        if (Process32First(hSnap, &pEntry))
        {
            do
            {
                if (!_wcsicmp(pEntry.szExeFile, L"RobloxPlayerBeta.exe"))
                {
                    ProcId = pEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &pEntry));
        }
        CloseHandle(hSnap);
    }
    return ProcId;
}

uintptr_t GetRobloxModuleBase(DWORD RobloxPID)
{
    HANDLE hSnap = NULL;
    uintptr_t ModuleBaseAddr = NULL;
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, RobloxPID);
    if (hSnap != NULL)
    {
        MODULEENTRY32 mEntry;
        mEntry.dwSize = sizeof(mEntry);
        if (Module32First(hSnap, &mEntry))
        {
            do
            {
                if (!_wcsicmp(mEntry.szModule, L"RobloxPlayerBeta.exe"))
                {
                    ModuleBaseAddr = (uintptr_t)mEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &mEntry));
        }
        CloseHandle(hSnap);
    }
    return ModuleBaseAddr;
}

void SuspendThreads(DWORD RobloxPID, uintptr_t ModuleBaseAddr)
{
    HANDLE hSnap = NULL;
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
    if (hSnap != NULL)
    {
        THREADENTRY32 tEntry;
        tEntry.dwSize = sizeof(tEntry);
        if (Thread32First(hSnap, &tEntry))
        {
            HANDLE hCurThread = NULL;
            _OBJECT_ATTRIBUTES TAttributes;
            _CLIENT_ID ClientId;
            ClientId.UniqueProcess = (HANDLE)RobloxPID;

            TAttributes.Length = sizeof(TAttributes);
            TAttributes.Attributes = NULL;
            TAttributes.ObjectName = NULL;
            TAttributes.RootDirectory = NULL;
            TAttributes.SecurityDescriptor = NULL;
            TAttributes.SecurityQualityOfService = NULL;
            uintptr_t threadinfo;
            do
            {
                if (tEntry.th32OwnerProcessID == RobloxPID)
                {
                    ClientId.UniqueThread = (HANDLE)tEntry.th32ThreadID;
                    NtOpenThread(&hCurThread, THREAD_ALL_ACCESS, &TAttributes, &ClientId);
                    if (hCurThread != NULL)
                    {
                        NtQueryInformationThread(hCurThread, ThreadQuerySetWin32StartAddress, &threadinfo, sizeof(uintptr_t), nullptr);
                        if (threadinfo == ModuleBaseAddr + 0x3972B0 || threadinfo == ModuleBaseAddr + 0x397840)
                        {
                            NtSuspendThread(hCurThread, nullptr);
                        }
                    }
                }
            } while (Thread32Next(hSnap, &tEntry));
            if (hCurThread != NULL)
                CloseHandle(hCurThread);
        }
        CloseHandle(hSnap);
    }
}

int main()
{
    if (!GetFunctions()) {
        return -1;
    }

    DWORD RobloxID = GetRobloxPID();

    if (RobloxID == NULL) {
        return -1;
    }

    std::cout << "RobloxPlayerBeta PID: " << RobloxID << std::endl;

    uintptr_t RobloxClientAddr = GetRobloxModuleBase(RobloxID);

    if (RobloxClientAddr == NULL) {
        return -1;
    }

    std::cout << "ModuleBase " << std::hex << std::uppercase << RobloxClientAddr << std::endl;

    SuspendThreads(RobloxID, RobloxClientAddr);

    std::cout << "Done, if Bypass Doesn't Work Roblox Probably Updated!" << std::endl;
    Sleep(3000);
    return 0;
}
