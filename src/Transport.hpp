#ifndef TRANSPORT_HPP_
#define TRANSPORT_HPP_

# include <cstddef>
# include <optional>
# include <memory>

class IByteStream {
	public:
		virtual ~IByteStream() = default;
		virtual std::optional<size_t> read(void* buf, size_t size) = 0;
		virtual std::optional<size_t> write(const void* buf, size_t size) = 0;
};

class ITransport {
	public:
		virtual ~ITransport() = default;
		virtual std::unique_ptr<IByteStream> accept() = 0;
};

#endif
