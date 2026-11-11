#include "Server.hpp"
#include <sys/socket.h>
#include <system_error>
#include <cerrno>
#include <netinet/in.h>
#include <unistd.h>
#include <optional>

Server::Server(int port, ClientHandler handler)
: server_fd_(-1), client_fd_(-1), handler_(handler)
{
	server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
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

Server::~Server() {
	if (server_fd_ >= 0)
		close(server_fd_);
	if (client_fd_ >= 0)
		close(client_fd_);
}

void	Server::run(void) {
	
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
		std::string	received(read_buf, rd_bytes);
		std::optional<std::string>	responce = handler_(received);
		if (!responce.has_value()) continue ;
		ssize_t	wr_bytes = 0;
		for (size_t sent = 0; sent < responce->length(); sent += static_cast<size_t>(wr_bytes)) {
			wr_bytes = write(client_fd_, responce->data() + sent, responce->length() - sent);
			if (wr_bytes <= 0)
				throw std::system_error(errno, std::generic_category(), "write");
		}
	}
}
