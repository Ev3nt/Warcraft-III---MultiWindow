#include "CmdParser.h"
#include <sstream>

CmdParser::CmdParser(const std::string& commands) {
	Parse(commands);
}

CmdParser::CmdParser() {

}

void CmdParser::Parse(const std::string& commands) {
	m_data.clear();

	std::istringstream stream(commands);

	while (!stream.eof()) {
		std::string key, value;
		stream >> key;

		if (key[0] != '-') {
			return;
		}

		key = key.substr(1);
		auto prevPos = stream.tellg();

		stream >> value;

		if (value.empty() || value[0] == '-') {
			m_data[key] = "true";
			stream.seekg(prevPos);

			continue;
		}

		auto iter = value.find('"');
		auto valueEnd = value.find('"', iter + 1);
		if (iter == std::string::npos || (iter == 0 && valueEnd == value.size() - 1)) {
			m_data[key] = iter != std::string::npos ? value.substr(1, valueEnd - 1) : value;
		}

		if (iter != 0) {
			return;
		}

		auto valueBeg = commands.find_first_not_of(' ', (size_t)prevPos) + 1;

		while (!stream.eof()) {
			std::string temp;
			stream >> temp;

			auto iter = temp.find('"');
			if (iter == std::string::npos) {
				continue;
			}

			if (iter != temp.size() - 1) {
				return;
			}

			auto valueEnd = commands.find_last_not_of(' ', (size_t)stream.tellg());

			m_data[key] = commands.substr(valueBeg, valueEnd - valueBeg);

			break;
		}
	}
}

//std::string CmdParser::Get(std::string key) {
//	return m_data[key];
//}

bool CmdParser::IsString(const std::string& key) {
	auto iter = m_data.find(key);
	
	return iter != m_data.end();
}

bool CmdParser::IsInteger(const std::string& key) {
	auto iter = m_data.find(key);
	if (iter == m_data.end()) {
		return false;
	}

	std::string temp = !iter->second.compare(0, 2, "0x") || !iter->second.compare(0, 2, "0X") ? iter->second.substr(2) : iter->second;

	return temp.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos;
}

bool CmdParser::IsFloat(const std::string& key) {
	auto iter = m_data.find(key);
	if (iter == m_data.end()) {
		return false;
	}

	return iter->second.find_first_not_of("0123456789.") == std::string::npos;
}

bool CmdParser::IsBool(const std::string& key) {
	auto iter = m_data.find(key);
	if (iter == m_data.end()) {
		return false;
	}

	return IsInteger(key) || iter->second == "true" || iter->second == "false";
}