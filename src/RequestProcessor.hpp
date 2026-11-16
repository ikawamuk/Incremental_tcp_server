#ifndef REQUESTPROCESSOR_HPP_
# define REQUESTPROCESSOR_HPP_

# include <string>
# include <functional>
# include <optional>

struct ProcessResult {
	ProcessResult(const std::string& r, std::size_t c): response(r), consumed(c) {}
	std::string response;
	std::size_t consumed;
};

using RequestProcessor = std::function<std::optional<ProcessResult>(const std::string&)>;

#endif
