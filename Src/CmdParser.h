#pragma once

#include <string>
#include <map>

class CmdParser {
	std::map<std::string, std::string> m_data;
public:
	CmdParser(const std::string& commands);
	CmdParser(const CmdParser&) = delete;
	CmdParser();
	void Parse(const std::string& commands);
	//static std::string Get(std::string key);

	bool IsString(const std::string& key);
	bool IsInteger(const std::string& key);
	bool IsFloat(const std::string& key);
	bool IsBool(const std::string& key);

	template<typename ValueType>
	ValueType Get(const std::string& key) {
		const std::string& value = m_data[key];

		if constexpr (std::is_same_v<std::decay_t<ValueType>, std::string>) {
			return value;
		}
		else if constexpr (std::is_same_v<std::decay_t<ValueType>, int>) {
			int beg = !value.compare(0, 2, "0x") || !value.compare(0, 2, "0X") ? 2 : 0;

			return std::stoi(value.substr(beg), nullptr, beg ? 16 : 10);
		}
		else if constexpr (std::is_same_v<std::decay_t<ValueType>, float>) {
			return std::stof(value);
		}
		else if constexpr (std::is_same_v<std::decay_t<ValueType>, bool>) {
			int beg = !value.compare(0, 2, "0x") || !value.compare(0, 2, "0X") ? 2 : 0;

			return value == "true" || std::stoi(value.substr(beg), nullptr, beg ? 16 : 10);
		}
		
		return ValueType();
	}
};