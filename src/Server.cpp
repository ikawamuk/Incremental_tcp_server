#include "Server.hpp"
#include <sys/socket.h>
#include <system_error>
#include <cerrno>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(int port)
: server_fd_(-1), client_fd_(-1)
{
	server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd_ < 0)
		throw std::system_error(errno, std::generic_category(), "socket");
	struct sockaddr_in	serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = htons(port);
	if (bind(server_fd_, (const struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
		close(server_fd_);
		throw std::system_error(errno, std::generic_category(), "bind");
	}
}

Server::~Server() {
	if (server_fd_ >= 0)
		close(server_fd_);
	if (client_fd_ >= 0)
		close(client_fd_);
}

void	Server::run(void) {
	if (listen(server_fd_, LISTEN_QUEUE_NUMBER) < 0)
		throw std::system_error(errno, std::generic_category(), "listen");
	client_fd_ = accept(server_fd_, nullptr, nullptr);
	if (client_fd_ < 0)
		throw std::system_error(errno, std::generic_category(), "accept");
	for (;;) {
		char	read_buf[BUF_SIZE];
		ssize_t	rd_bytes = read(client_fd_, read_buf, sizeof(read_buf));
		if (rd_bytes < 0)
			throw std::system_error(errno, std::generic_category(), "read");
		if (rd_bytes == 0)
			break ;
		ssize_t	wr_bytes;
		for (ssize_t sent = 0; sent < rd_bytes; sent += wr_bytes) {
			wr_bytes = write(client_fd_, read_buf + sent, rd_bytes - sent);
			if (wr_bytes <= 0)
				throw std::system_error(errno, std::generic_category(), "write");
		}
	}
}
