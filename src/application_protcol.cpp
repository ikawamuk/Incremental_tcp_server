#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "RequestProcessor.hpp"

namespace {
	std::optional<std::string>	extract_line(const std::string& buffer) {
		std::string line;
		std::size_t	newline_pos = buffer.find(';');
		if (newline_pos == std::string::npos)
			return (std::nullopt);
		line = buffer.substr(0, newline_pos);
		return (line);
	}
}

// std::optional<std::string>	echo_protcol(const std::string& read_data) {
// 	static	std::string	request;
// 	request += read_data;
// 	std::optional<std::string>	line = extract_line(request);
// 	if (!line.has_value()) return (std::nullopt);
// 	return ("OK " + line.value() + "\n");
// }

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

ProcessResult	mini_greeting_protcol(const std::string& read_buf) {
	std::variant<ParseSuccess, ParseNeedMore, ParseError>	result;
	std::optional<std::string>	line = extract_line(read_buf);
	if (!line.has_value()) return (ParseNeedMore());
	const std::size_t consumed = line->length() + 1;
	std::vector<std::string> tokens = split_tokens(*line);
	if (tokens.empty()) return (ParseSuccess{"", consumed});
	const std::string& command = tokens[0];
	if (command == "HELLO") {
		if (tokens.size() == 1)
			return (ParseSuccess{"hello, Sir.\n", consumed});
		return (ParseSuccess{"hello, Mr." + tokens[1] + "!\n", consumed});
	}
	if (command == "BYE")
		return (ParseSuccess{"Good bye!\n", consumed});
	return (ParseSuccess{"Unknown command\nsupporting command\n\t- \"HELLO <name>\"\n\t- \"BYE\"\n", consumed});
}
