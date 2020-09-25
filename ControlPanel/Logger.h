#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
ref class Logger
{
public:
	Logger() {
		std::time_t time = std::time(nullptr);
		logfile->append("LOG_");
		logfile->append(std::asctime(std::localtime(&time)));
		logfile->append(".log");
		FILE->open(&logfile->c_str);
	}
	int log_event(std::string eve) {
		*FILE << eve << std::endl;
	}
	~Logger() {
		FILE->close();
	}
private:
	std::ofstream* FILE;
	std::string* logfile;

};

