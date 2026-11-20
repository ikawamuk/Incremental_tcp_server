#ifndef TRANSPORT_HPP_
# define TRANSPORT_HPP_

# include <memory>
# include "Poller.hpp"
# include "ByteStream.hpp"

class ITransport: public IPollable {
	public:
		virtual ~ITransport() = default;
		virtual Handle	handle(void) const noexcept = 0;
		virtual std::unique_ptr<IByteStream> accept(void) = 0;
};

#endif
