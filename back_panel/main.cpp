#define _CRT_SECURE_NO_WARNINGS
#include "Hooking_master.h"
#include <thread>


int Wait()
{
	char a;
	printf("Press any key to exit");
	a = getchar();
	return 0;
}

DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

bool inject_dll(const char* dllPath)
{
	const char* procName = "SumatraPDF-3.2-64.exe";
	char fullpath[MAX_PATH];
	GetFullPathName(dllPath, MAX_PATH, fullpath, nullptr);
	DWORD procId = 0;
	while (!procId)
	{
		procId = GetProcId(procName);
		Sleep(30);
	}
	LPCSTR DllPath = fullpath;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1,
		MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath,
		strlen(DllPath) + 1, 0);
	HANDLE hLoadThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
			"LoadLibraryA"), pDllPath, 0, 0);
	WaitForSingleObject(hLoadThread, INFINITE);
	std::cout << "Dll path allocated at: " << std::hex << pDllPath << std::endl;
	VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);
	return 1;
}


int main()
{
	std::thread thread1(control_worker);
	std::thread thread2(log_worker);
	thread1.detach();
	//log_worker();
	thread2.detach();
	DWORD err = GetLastError();
	//TerminateThread(hLOG, 0);
	inject_dll("HookDll.dll");
	Wait();
}