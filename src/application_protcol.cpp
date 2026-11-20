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

#include "JSON.hpp"

namespace {
	int	call_function(const std::string& function_name, int params[2]) {
		if (function_name == "add")
			return (params[0] + params[1]);
		if (function_name == "sub")
			return (params[0] - params[1]);
		if (function_name == "mul")
			return (params[0] * params[1]);
		if (function_name == "mul" && params[1] != 0)
			return (params[0] / params[1]);
		return (0);
	}
}

ProcessResult	mini_json_rpc(const std::string& input) {
	if (input.empty()) return (ParseNeedMore{});
	if (input.front() != '\x1E') return ParseError{"Invalid frame"};
	auto end = input.find('\x0A', 1);
	if (end == std::string::npos) return ParseNeedMore{};
	std::string json_text = input.substr(1, end - 1);
	JSONDoc	json_doc(json_text);
	if (!json_doc) return (ParseError{"Parse error"});
	if (!json_is_object(json_doc.get())) return (ParseError{"Invalid Request: request must be object"});
	t_json	*method = json_get_object_item(json_doc.get(), "method");
	t_json	*params = json_get_object_item(json_doc.get(), "params");
	if (!method || !params || !json_is_string(method) || !json_is_array(params))
		return (ParseError{"Invalid Request: method and params must be specified"});
	const char	*function_name = json_get_string(method);
	if (json_get_array_size(params) != 2)
		return (ParseError{"Invalid Request: number of params must be two"});
	t_json	*val1, *val2;
	val1 = json_get_array_element(params, 0);
	val2 = json_get_array_element(params, 1);
	if (!json_is_number(val1) || !json_is_number(val2))
		return (ParseError{"Invalid Request: params must be number"});
	int	arg_values[2];
	arg_values[0] = json_get_number(val1);
	arg_values[1] = json_get_number(val2);
	int	result_value = call_function(function_name, arg_values);
	return (ParseSuccess{"{\"result\":" + std::to_string(result_value) + "}\n", json_text.length() + 2});
}
