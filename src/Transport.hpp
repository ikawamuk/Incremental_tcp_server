#ifndef TRANSPORT_HPP_
#define TRANSPORT_HPP_

# include <cstddef>
# include <unistd.h>
# include <memory>

class IByteStream {
	public:
		virtual ~IByteStream() = default;
		virtual ssize_t read(void* buf, size_t size) = 0;
		virtual ssize_t write(const void* buf, size_t size) = 0;
};

class ITransport {
	public:
		virtual ~ITransport() = default;
		virtual std::unique_ptr<IByteStream> accept() = 0;
};

#endif
