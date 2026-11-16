#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <optional>
#include <stdexcept>
#include <iostream>

Server::Server(std::unique_ptr<ITransport> transport, RequestProcessor handler, ShouldStop should_stop)
:transport_(std::move(transport)), clients_(), handler_(handler), should_stop_(should_stop) {}

Client::Client(std::unique_ptr<IByteStream> s, RequestProcessor h)
:stream_(std::move(s)), handler_(h) {}

void	Client::try_read(void) {
	char	read_buf[BUF_SIZE];
	ssize_t	rd_bytes = stream_->read(read_buf, sizeof(read_buf));
	proceccing_buf_ += std::string(read_buf, rd_bytes);
	for (;;) {
		std::optional<ProcessResult>	process_result = handler_(proceccing_buf_);
		if (!process_result.has_value()) return ;
		proceccing_buf_.erase(0, process_result.value().consumed);
		write_buf_ += process_result.value().response;
	}
}

void	Client::try_write(void) {
	if (write_buf_.empty()) return ;
	ssize_t	wr_bytes = stream_->write(write_buf_.data(), write_buf_.length());
	write_buf_.erase(0, wr_bytes);
}

void	Server::run(void) {
	while (!should_stop_()) {
		try {
			std::unique_ptr<IByteStream>	new_stream = transport_->accept();
			if (new_stream != nullptr)
				clients_.push_back(Client(std::move(new_stream), handler_));
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) continue ;
			throw ;
		}
		try {
			// ここでpollingする
			for (std::vector<Client>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
				it->try_read();
				it->try_write();
			}
		} catch (std::system_error& e) {
			std::cerr << "Client Error: " << e.what() << std::endl;
		}
	}
	std::cout << "\rServer terminetted" << std::endl;
}
