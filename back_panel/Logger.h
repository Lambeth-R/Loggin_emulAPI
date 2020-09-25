#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <Windows.h>

#define LMBT_ReadFile 0x1


class Logger
{
public:
	Logger();
	void log_event(std::vector<char> eve);
	~Logger();
private:
	FILE* F;
	std::string logfile;

};


