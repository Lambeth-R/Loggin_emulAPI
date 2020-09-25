#pragma once

#include "Logger.h"
extern Logger mLog;
std::vector<char> typedef STRING;
STRING PIHAY_V_VECTOR(char* str, int size);

struct sMessage {
	const char start_seq[2] = { '2','2' };
	int int_size;
	char packORD[4] = { 48,48,48,48 }, CMD[3], size[3];
	char char_string[1012];
};

class lisenedFile
{
//public:

private:
	char fname[20];
	std::vector <STRING> readed_calls;

};


STRING make_msg(char snd[1012], int sz, const char command[3] = "SND");

bool parse_messge(STRING rcv, sMessage* result);

auto control_worker = []() {
	HANDLE pipe_handler;
	char buffer[1024];
	DWORD dwRead;
	pipe_handler = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	if (pipe_handler == INVALID_HANDLE_VALUE) {

		mLog.log_event(PIHAY_V_VECTOR((char*)"Cannot open PIPE :c\n", 21));
		return 0;
	}
	else
	{
		std::vector<STRING> test;
		printf("Pipe created!\n");
		while (true)
		{
			if (ConnectNamedPipe(pipe_handler, NULL) != FALSE)   // wait for someone to connect to the pipe
			{
				
				STRING init_msg = make_msg((char*)"Listen", 7, "ini");
				WriteFile(pipe_handler,
					init_msg.data(),
					1024,   // = length of string + terminating '\0' !!!
					&dwRead,
					NULL);
				printf("Connected!!!\n");
				while (ReadFile(pipe_handler, buffer, 1024, &dwRead, NULL) != FALSE)
				{
					STRING B = PIHAY_V_VECTOR(buffer, 1024);
					sMessage readed_message;
					parse_messge(B, &readed_message);
					B = PIHAY_V_VECTOR((char*)readed_message.char_string, readed_message.int_size);
					test.push_back(B);
					mLog.log_event(B);
				}
			}
		}
	}
	return 0; 
};

auto log_worker = []() {
	HANDLE pipe_log = NULL;
	char buffer[1024];
	DWORD dwRead;
	while (!pipe_log || pipe_log == INVALID_HANDLE_VALUE) {
		pipe_log = CreateFile(TEXT("\\\\.\\pipe\\lmbtpipe_log"),
			GENERIC_READ | GENERIC_WRITE,
			0, NULL,
			OPEN_EXISTING,
			0, NULL);
		Sleep(100);
	}
	if (pipe_log == INVALID_HANDLE_VALUE) {

		mLog.log_event(PIHAY_V_VECTOR((char*)"Cannot open PIPE :c\n", 21));
		return 0;
	}
	else
	{
		std::vector<STRING> test;
		printf("Log Pipe opened!\n");
		while (true)
		{
			STRING init_msg = make_msg((char*)"Listen", 7, "ini");
			WriteFile(pipe_log,
				init_msg.data(),
				1024,   // = length of string + terminating '\0' !!!
				&dwRead,
				NULL);
			printf("Log Connected!!!\n");
			while (ReadFile(pipe_log, buffer, 1024, &dwRead, NULL) != FALSE)
			{
				STRING B = PIHAY_V_VECTOR(buffer, 1024);
				sMessage readed_message;
				parse_messge(B, &readed_message);
				B = PIHAY_V_VECTOR((char*)readed_message.char_string, readed_message.int_size);
				test.push_back(B);
				mLog.log_event(B);
			}
		}
	}
	return 0;
};