#ifndef TCPTRANSPORT_HPP_
# define TCPTRANSPORT_HPP_

# include "Server.hpp"

class TCPByteStream: public IByteStream {
	public:
		TCPByteStream(int client_fd) noexcept;
		~TCPByteStream() override;
		Handle	handle(void) const noexcept override;
		std::optional<size_t>	read(void* buf, size_t size) override;
		std::optional<size_t>	write(const void* buf, size_t size) override;
	private:
		int	client_fd_;
};

class TCPTransport: public ITransport {
	public:
		explicit TCPTransport(int port);
		~TCPTransport() override;
		Handle	handle(void) const noexcept override;
		std::unique_ptr<IByteStream> accept() override;
	private:
		int					server_fd_;
		static const int	LISTEN_QUEUE_NUMBER = 10;
};

#endif
