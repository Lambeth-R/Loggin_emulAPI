#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"

Logger::Logger()
{
	std::time_t time = std::time(nullptr);
	logfile.append("LOG_");
	std::string temp = std::asctime(std::localtime(&time));
	size_t index = 0;
	while (true) {
		index = temp.find(":", index);
		if (index == std::string::npos) break;
		temp.replace(index, 1, "_");
		index++;
	}
	logfile.append(temp.substr(11, 8));
	logfile.append(".log");
	
	F = fopen(logfile.c_str(),"wb");
}
void Logger::log_event(std::vector<char> eve)
{
	for (int i = 0; i < eve.size(); i++)
		fwrite(&eve[i], 1, 1, F);
}
Logger::~Logger()
{
	fclose(F);
}