#ifndef BYTESTREAM_HPP_
# define BYTESTREAM_HPP_

# include <optional>
# include "Poller.hpp"

class IByteStream: public IPollable {
	public:
		virtual ~IByteStream() = default;
		virtual Handle	handle(void) const noexcept = 0;
		virtual std::optional<size_t> read(void* buf, size_t size) = 0;
		virtual std::optional<size_t> write(const void* buf, size_t size) = 0;
};

#endif
