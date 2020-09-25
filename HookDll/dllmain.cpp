#define _CRT_SECURE_NO_WARNINGS	
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "..\..\Detour-master\include\detours.h"
#define SIZE 6
// Объявления функций и кастомных типов
typedef int (WINAPI* pMessageBoxW)(HWND, LPCWSTR, LPCWSTR, UINT);
/*typedef*/BOOL (WINAPI* True_ReadFile)(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED) = ReadFile;
__declspec(dllexport) BOOL WINAPI LMBT_ReadFile(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
__declspec(dllexport) BOOL WINAPI emul_LMBT_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
int WINAPI MyMessageBoxW(HWND, LPCWSTR, LPCWSTR, UINT);
void BeginRedirect(LPVOID, LPVOID);
int what_do_we_do = 0;

std::vector<char> typedef STRING;

pMessageBoxW pOrigMBAddress = NULL;
//pLMBT_ReadFile pLOrigReadFileAddress = NULL;
BYTE oldBytes[SIZE] = { 0 };
BYTE JMP[SIZE] = { 0 };
DWORD oldProtect, myProtect = PAGE_EXECUTE_READWRITE;
HANDLE pipe_command, pipe_log;
DWORD packID;

std::vector<char> PIHAY_V_VECTOR(char* str, int size)
{
	std::vector<char> buff;
	buff.assign(str, str + size);
	return buff;
}

struct sMessage {
	const char start_seq[2] = { '2','2' };
	int int_size;
	char packORD[4] = { 48,48,48,48 }, CMD[3], size[3];
	char char_string[1012];
};

bool parse_messge(STRING rcv, sMessage* result) {
	if (rcv[0] != result->start_seq[0] || rcv[1] != result->start_seq[1]) return 0;
	memcpy(result->packORD, &rcv[2], 4);
	memcpy(result->CMD, &rcv[6], 3);
	memcpy(result->size, &rcv[9], 3);
	char temp[10];
	memcpy(temp, result->size, 3);
	temp[3] = '\0';
	sscanf(temp, "%x", &result->int_size);
	memcpy(result->char_string, &rcv[12], result->int_size);
	return 1;
}

STRING make_msg(char snd[1012], int sz, const char command[3] = "SND") {
	std::vector<char> res, t;
	sMessage* one = new sMessage;
	if (sz == 1024) sz -= 12;
	res = PIHAY_V_VECTOR((char*)one->start_seq, 2);
	if (packID > 9999) packID = 0;

	int temp = packID, i = 3;
	while (temp != 0) {
		one->packORD[i] = temp % 10 + 48;
		i--;
		temp -= temp % 10;
		temp /= 10;
	}
	t = PIHAY_V_VECTOR(one->packORD, 4);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	strcpy(one->CMD, command);
	t = PIHAY_V_VECTOR(one->CMD, 3);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	sprintf(one->size, "%x", sz);
	for (int j = 0; j < 3; j++)
	{
		res.push_back(one->size[j]);
	}
	t = PIHAY_V_VECTOR(snd, 1012);
	for (auto j = t.begin(); j < t.end(); j++)
	{
		res.push_back(*j);
	}
	packID++;
	return res;
}

bool equal_strings(char* st, char* nd, int size) {
	for (int i = 0; i < size; i++)
		if (st[i] != nd[i]) return 0;
	return 1;
}


int WINAPI HookThread(HMODULE hModule)
{
	sMessage init_msg;
	packID = 0;
	DWORD dwWritten;
	while (!pipe_command) {
		pipe_command = CreateFile(TEXT("\\\\.\\pipe\\lmbtpipe"),
			GENERIC_READ | GENERIC_WRITE,
			0, NULL,
			OPEN_EXISTING,
			0, NULL);
	}
	char buffer[1024];
	DWORD dwRead;
	ReadFile(pipe_command, buffer, 1024, &dwRead, NULL);
	WriteFile(pipe_command, buffer, 1024, &dwRead, NULL);
	parse_messge(PIHAY_V_VECTOR(buffer, dwRead), &init_msg);
	if (equal_strings(init_msg.char_string, (char*)"Listen", 7)) {
		//
		what_do_we_do = 11;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)True_ReadFile, LMBT_ReadFile);
		DetourTransactionCommit();
	}
	if (equal_strings(init_msg.char_string, (char*)"DropOld", 8)) {
		//		
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		if (what_do_we_do == 11) DetourDetach(&(PVOID&)True_ReadFile, LMBT_ReadFile);
		if (what_do_we_do == 22) DetourDetach(&(PVOID&)True_ReadFile, emul_LMBT_ReadFile);
		what_do_we_do = 33;
		DetourTransactionCommit();
	}
	if (equal_strings(init_msg.char_string, (char*)"Emulate", 8)) {
		//
		what_do_we_do = 22;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)True_ReadFile, emul_LMBT_ReadFile);
		DetourTransactionCommit();
	}
	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HookThread, hModule, 0, 0);

		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			if (what_do_we_do == 11) DetourDetach(&(PVOID&)True_ReadFile, LMBT_ReadFile);
			if (what_do_we_do == 22) DetourDetach(&(PVOID&)True_ReadFile, emul_LMBT_ReadFile);
			DetourTransactionCommit();
			break;
		}
	}
	return TRUE;
}
__declspec(dllexport) BOOL WINAPI LMBT_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	while (pipe_command == NULL) {
		Sleep(50);
	}
	pipe_log = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe_log"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	bool retValue = 0;
	retValue = True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	DWORD dwWritten, dwRead;
	std::string t = "Listen_";
	t += std::to_string(*lpNumberOfBytesRead);
	STRING init_msg = make_msg((char*)t.c_str(), t.size(), "ini");
	WriteFile(pipe_log,
		init_msg.data(),
		1024,   // = length of string + terminating '\0' !!!
		&dwRead,
		NULL);		
	char msg[256];
	std::string out;
	out += "ReadFile(";
	sprintf(msg, "%p", hFile);
	out += msg;
	out += "|||";
	for (int i = 0; i < *lpNumberOfBytesRead; i++)
	{
		out += ((char*)lpBuffer)[i];
	}
	out += "|||";
	out += std::to_string(nNumberOfBytesToRead);
	out += "|||";
	out += std::to_string(*lpNumberOfBytesRead);
	out += "|||";
	out += "UNSUPPORTED";
	out += "|||";
	//Бить на пакеты!
	for (int i = 0; i < out.size(); i += 1012) {
		int sz_to_send = 1012;
		if (i + 1024 > out.size())
			sz_to_send = out.size() - i;
		STRING packet = make_msg((char*)out.substr(i, i + 1012).c_str(), sz_to_send);
		WriteFile(pipe_log,
			packet.data(),
			1024,
			&dwWritten,
			NULL);
	}
	DisconnectNamedPipe(pipe_log);
	return retValue;
	
}

__declspec(dllexport) BOOL WINAPI emul_LMBT_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	while (pipe_command == NULL) {
		Sleep(50);
	}
	std::vector<STRING> backpack;
	char buffer[1024];
	DWORD dwRead;
	while (true)
	{
		True_ReadFile(pipe_command, buffer, 1024, &dwRead, NULL);
		backpack.push_back(PIHAY_V_VECTOR(buffer, dwRead));
		if (equal_strings(&buffer[12], (char*)"END",4)) break;
	}
	char* Buffer = new char[1];

	return 1;
}