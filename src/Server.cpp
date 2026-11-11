#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <optional>

Server::Server(std::unique_ptr<ITransport> transport, ClientHandler handler)
:transport_(std::move(transport)), handler_(handler), client_(nullptr) {}

void	Server::run(void) {
	client_ = transport_->accept();
	for (;;) {
		char	read_buf[BUF_SIZE];
		ssize_t	rd_bytes = client_->read(read_buf, sizeof(read_buf));
		if (rd_bytes <= 0)
			break ;
		std::string	received(read_buf, rd_bytes);
		std::optional<std::string>	responce = handler_(received);
		if (!responce.has_value()) continue ;
		ssize_t	wr_bytes = 0;
		for (size_t sent = 0; sent < responce->length(); sent += static_cast<size_t>(wr_bytes))
			wr_bytes = client_->write(responce->data() + sent, responce->length() - sent);
	}
}
