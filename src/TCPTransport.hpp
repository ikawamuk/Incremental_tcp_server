#ifndef TCPTRANSPORT_HPP_
# define TCPTRANSPORT_HPP_

# include "Transport.hpp"

class TCPByteStream: public IByteStream {
	public:
		TCPByteStream(int client_fd) noexcept;
		~TCPByteStream() override;
		ssize_t	read(void* buf, size_t size) override;
		ssize_t	write(const void* buf, size_t size) override;
	private:
		int	client_fd_;
};

class TCPTransport: public ITransport {
	public:
		explicit TCPTransport(int port);
		~TCPTransport() override;
		std::unique_ptr<IByteStream> accept() override;
	private:
		int					server_fd_;
		static const int	LISTEN_QUEUE_NUMBER = 10;
};

#endif
