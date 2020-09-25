#include <Windows.h>
#include <vector>
#include <string>
#include "Logger.h"
#include <iostream>
#pragma once

__declspec(selectany) std::vector<std::string>* from_pipe = new std::vector<std::string>();

ref class Hooking_master {
public:
	Hooking_master() {

	}
	int pipe_worker();
private:
	
};

int Hooking_master::pipe_worker()
{
	HANDLE pipe_handler;
	char buffer[1024];
	DWORD dwRead;
	pipe_handler = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe"),
		PIPE_ACCESS_DUPLEX |      // read/write access 
		FILE_FLAG_OVERLAPPED,     // overlapped mode 
		PIPE_TYPE_MESSAGE |       // message-type pipe 
		PIPE_READMODE_MESSAGE |   // message read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // unlimited instances 
		4096 * sizeof(TCHAR),    // output buffer size 
		4096 * sizeof(TCHAR),    // input buffer size 
		5000,             // client time-out 
		NULL);
	return 1;
	while (pipe_handler != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(pipe_handler, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(pipe_handler, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				/* add terminating zero */
				buffer[dwRead] = '\0';

				/* do something with data in buffer */
				from_pipe->push_back(buffer);
			}
		}

		DisconnectNamedPipe(pipe_handler);
	}
	return 0;
}