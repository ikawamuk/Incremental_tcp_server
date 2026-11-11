#include "TCPTransport.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <system_error>

TCPByteStream::TCPByteStream(int client_fd) noexcept 
:client_fd_(client_fd) {}

TCPByteStream::~TCPByteStream() {
	if (client_fd_ >= 0)
		close(client_fd_);
}

ssize_t	TCPByteStream::read(void* read_buf, size_t size) {
	ssize_t	rd_bytes = ::recv(client_fd_, read_buf, size, 0);
		if (rd_bytes < 0)
			throw std::system_error(errno, std::generic_category(), "read");
		return (rd_bytes);
}

ssize_t	TCPByteStream::write(const void* write_buf, size_t size) {
	ssize_t	wr_bytes = ::send(client_fd_, write_buf, size, MSG_NOSIGNAL);
	if (wr_bytes <= 0 && (size_t)wr_bytes != size)
		throw std::system_error(errno, std::generic_category(), "write");
	return (wr_bytes);
}

TCPTransport::TCPTransport(int port)
: server_fd_(-1)
{
	server_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server_fd_ < 0)
		throw std::system_error(errno, std::generic_category(), "socket");
	struct sockaddr_in	serv_addr = {};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = htons(port);
	if (bind(server_fd_, (const struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
		int err = errno;
		close(server_fd_);
		throw std::system_error(err, std::generic_category(), "bind");
	}
	if (listen(server_fd_, LISTEN_QUEUE_NUMBER) < 0) {
		int err = errno;
		close(server_fd_);
		throw std::system_error(err, std::generic_category(), "listen");
	}
}

TCPTransport::~TCPTransport() {
	if (server_fd_ >= 0)
		close(server_fd_);
}

std::unique_ptr<IByteStream>	TCPTransport::accept() {
	int	client_fd = ::accept4(server_fd_, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (client_fd < 0)
		throw std::system_error(errno, std::generic_category(), "accept");
	return (std::make_unique<TCPByteStream>(client_fd));
}
