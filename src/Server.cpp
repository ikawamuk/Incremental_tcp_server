#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <optional>
#include <stdexcept>
#include <iostream>

Server::Server(std::unique_ptr<ITransport> transport, ClientHandler handler, ShouldStop should_stop)
:transport_(std::move(transport)), handler_(handler), client_(nullptr), should_stop_(should_stop) {}

void	Server::handle_client(void) {
	for (;;) {
		char	read_buf[BUF_SIZE];
		ssize_t	rd_bytes = client_->read(read_buf, sizeof(read_buf));
		if (rd_bytes == -1) continue ;
		if (rd_bytes == 0) break ;
		std::string	received(read_buf, rd_bytes);
		std::optional<std::string>	responce = handler_(received);
		if (!responce.has_value()) continue ;
		ssize_t	wr_bytes = 0;
		for (size_t sent = 0; sent < responce->length(); sent += static_cast<size_t>(wr_bytes)) {
			wr_bytes = client_->write(responce->data() + sent, responce->length() - sent);
		}
	}
}

void	Server::run(void) {
	while (!should_stop_()) {
		try {
			client_ = transport_->accept();
			if (client_ == nullptr) continue ;
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) continue ;
			throw ;
		}
		try {
			handle_client();
			client_.reset();
		} catch (std::system_error& e) {
			std::cerr << "Client Error: " << e.what() << std::endl;
		}
	}
	std::cout << "\rServer terminetted" << std::endl;
}
