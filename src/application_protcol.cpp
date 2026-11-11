#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace {
	std::optional<std::string>	extract_line(std::string& buffer) {
		std::string line;
		std::size_t	newline_pos = buffer.find(';');
		if (newline_pos == std::string::npos)
			return (std::nullopt);
		line = buffer.substr(0, newline_pos);
		buffer.erase(0, newline_pos + 1);
		return (line);
	}
}

std::optional<std::string>	echo_protcol(const std::string& read_data) {
	static	std::string	request;
	request += read_data;
	std::optional<std::string>	line = extract_line(request);
	if (!line.has_value()) return (std::nullopt);
	return ("OK " + line.value() + "\n");
}

/*
	"HELLO" <name> -> "hello, Mr.<name>!"
	"BYE"          -> "Good bye!"
	else           -> "Unknown command\nsupporting command\n\t\"HELLO <name>\"\n\t\"BYE\"\n"
*/
std::vector<std::string> split_tokens(const std::string& str) {
	std::istringstream iss(str);
	std::vector<std::string> tokens;
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

std::optional<std::string>	mini_greeting_protcol(const std::string& read_data) {
	static	std::string	request;
	request += read_data;
	std::optional<std::string>	line = extract_line(request);
	if (!line.has_value()) return (std::nullopt);
	std::vector<std::string> tokens = split_tokens(*line);
	if (tokens.empty()) return (std::nullopt);
	const std::string& command = tokens[0];
	if (command == "HELLO") {
		if (tokens.size() == 1)
			return ("hello, Sir.\n");
		return ("hello, Mr." + tokens[1] + "!\n");
	}
	if (command == "BYE")
		return ("Good bye!\n");
	return ("Unknown command\nsupporting command\n\t- \"HELLO <name>\"\n\t- \"BYE\"\n");
}

