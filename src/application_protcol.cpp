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
	std::variant<Processed, NeedMore, Discard>	result;
	std::optional<std::string>	line = extract_line(read_buf);
	if (!line.has_value()) return (NeedMore());
	const std::size_t consumed = line->length() + 1;
	std::vector<std::string> tokens = split_tokens(*line);
	if (tokens.empty()) return (Processed{"", consumed});
	const std::string& command = tokens[0];
	if (command == "HELLO") {
		if (tokens.size() == 1)
			return (Processed{"hello, Sir.\n", consumed});
		return (Processed{"hello, Mr." + tokens[1] + "!\n", consumed});
	}
	if (command == "BYE")
		return (Processed{"Good bye!\n", consumed});
	return (Processed{"Unknown command\nsupporting command\n\t- \"HELLO <name>\"\n\t- \"BYE\"\n", consumed});
}

#include "JSON.hpp"
#include <charconv>
#include <cmath>

namespace {
	std::string	frame_rpc_response(std::string json) {
		json.insert(json.begin(), '\x1E');
		json.push_back('\n');
		return (json);
	}

	double	call_function(const std::string& function_name, double params[2]) {
		if (function_name == "add")
			return (params[0] + params[1]);
		if (function_name == "sub")
			return (params[0] - params[1]);
		if (function_name == "mul")
			return (params[0] * params[1]);
		if (function_name == "div") {
			if (params[1] == 0)
				throw std::runtime_error("Division by zero");
			return (params[0] / params[1]);
		}
		throw (std::runtime_error("Unknown method"));
	}

	std::string	execute_rpc(const char* function_name, double arg_values[2]) {
		try {
			if (!std::isfinite(arg_values[0]) || !std::isfinite(arg_values[1]))
				throw std::invalid_argument("Parameters must be finite");
			double	result_value = call_function(function_name, arg_values);
			if (!std::isfinite(result_value))
				throw std::overflow_error("Floating-point overflow");
			char number[64];
			auto [end, ec] = std::to_chars(number, number + sizeof(number), result_value);
			if (ec != std::errc{})
				throw std::runtime_error("Failed to format numeric result");
			return ("{\"result\":" + std::string(number, end) + "}");
		} catch (const std::exception& e) {
			return ("{\"error\":\"" + std::string(e.what()) + "\"}");
		}
	}

	std::string	process_rpc_json(const std::string& json_text) {
		constexpr const char* invalid_request = "{\"error\":\"Invalid Request\"}";
		JSONDoc	json_doc(json_text);
		if (!json_doc) return (invalid_request);
		if (!json_is_object(json_doc.get())) return (invalid_request);
		t_json	*method = json_get_object_item(json_doc.get(), "method");
		t_json	*params = json_get_object_item(json_doc.get(), "params");
		if (!method || !params || !json_is_string(method) || !json_is_array(params))
			return (invalid_request);
		const char	*function_name = json_get_string(method);
		if (json_get_array_size(params) != 2)
			return (invalid_request);
		t_json	*val1, *val2;
		val1 = json_get_array_element(params, 0);
		val2 = json_get_array_element(params, 1);
		if (!json_is_number(val1) || !json_is_number(val2))
			return (invalid_request);
		double	arg_values[2];
		arg_values[0] = json_get_number(val1);
		arg_values[1] = json_get_number(val2);
		return (execute_rpc(function_name, arg_values));
	}
}

ProcessResult	mini_json_rpc(const std::string& input) {
	constexpr std::size_t MAX_FRAME_SIZE = 64 * 1024;
	if (input.empty()) return (NeedMore{});
	if (input.front() != '\x1E') {
		auto next = input.find('\x1E');
		return Discard{next == std::string::npos ? input.size() : next};
	}
	auto end = input.find('\x0A', 1);
	if (end == std::string::npos) {
		if (input.size() >= MAX_FRAME_SIZE)
			throw std::runtime_error("Request frame too large");
		return NeedMore{};
	}
	const std::size_t consumed = end + 1;
	if (consumed > MAX_FRAME_SIZE) throw std::runtime_error("Request frame too large");
	return (Processed{frame_rpc_response(process_rpc_json(input.substr(1, end - 1))), consumed});
}
