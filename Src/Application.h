#pragma once

#include "CmdParser.h"

class Application {
public:
	static Application& getInstance();

	void run(const std::string& cmdLine);

private:
	Application();

	CmdParser m_commands;
};