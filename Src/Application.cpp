#include "Application.h"

#include "War3.h"

Application& Application::getInstance() {
	static Application application;
	
	return application;
}

Application::Application() {

}

void Application::run(const std::string& cmdLine) {
	m_commands.Parse(cmdLine);

	std::string key = "mouseclip";

	War3& war3 = War3::getInstance();
	war3.launch(m_commands.IsBool(key) && m_commands.Get<bool>(key));
}