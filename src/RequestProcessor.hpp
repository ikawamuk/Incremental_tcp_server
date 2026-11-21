#ifndef REQUESTPROCESSOR_HPP_
# define REQUESTPROCESSOR_HPP_

# include <string>
# include <functional>
# include <optional>
# include <variant>

struct ParseSuccess {
	std::string response;
	std::size_t consumed;
};

struct ParseNeedMore {};

struct ParseError {
	std::string	message;
	std::size_t consumed;
};

using ProcessResult = std::variant<ParseSuccess, ParseNeedMore, ParseError>;

using Parser = std::function<ProcessResult(const std::string&)>;

class RequestProcessor {
	public:
		explicit RequestProcessor(Parser p):parser_(p) {}
		ProcessResult	process(const std::string& input) { return (parser_(input)); }
	private:
		Parser	parser_;
};

#endif
