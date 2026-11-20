#ifndef JSONRPC_HPP_
# define JSONRPC_HPP_

# include <string>
extern "C" {
# include "rosetta_json/json.h"
}
class JSONDoc {
	public:
		explicit JSONDoc(const std::string& text) noexcept;
		~JSONDoc();
		JSONDoc(const JSONDoc&) = delete;
		JSONDoc& operator=(const JSONDoc&) = delete;
		t_json* get() const noexcept { return ptr_; }
		explicit operator bool() const noexcept { return ptr_ != nullptr; }
	private:
		t_json	*ptr_ = nullptr;
};

class JSONString {
	public:
		JSONString(const JSONDoc& item) noexcept;
		~JSONString();
		JSONString(const JSONString&) = delete;
		JSONString& operator=(const JSONString&) = delete;
		const char* c_str() const noexcept;
		bool valid() const noexcept;
	private:
		char* ptr_ = nullptr;
};

#endif
