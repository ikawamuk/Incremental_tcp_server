#include "JSON.hpp"

JSONDoc::JSONDoc(const std::string& text) noexcept : ptr_(json_parse(text.c_str())) {}

JSONDoc::~JSONDoc() {
	if (ptr_ != nullptr) json_delete(ptr_);
}

JSONString::JSONString(const JSONDoc& item) noexcept
: ptr_(item ? json_generate_unformatted(item.get()) : nullptr) {}

JSONString::~JSONString() { std::free(ptr_); }

const char*	JSONString::c_str() const noexcept { return (ptr_ != nullptr ? ptr_ : "null"); }

bool	JSONString::valid() const noexcept { return (ptr_ != nullptr); }


