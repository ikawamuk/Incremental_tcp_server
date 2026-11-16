#include "Server.hpp"
#include <optional>
#include <stdexcept>
#include <iostream>

Server::Server(std::unique_ptr<IServerConfig> server_config)
:transport_(std::move(server_config->transport)),
clients_(), handler_(server_config->handler),
should_stop_(server_config->should_stop),
poller_(std::move(server_config->poller)) {}

Client::Client(std::unique_ptr<IByteStream> s, RequestProcessor h)
:stream_(std::move(s)), handler_(h), read_closed_(false) {}

void	Client::try_read(void) {
	char	read_buf[BUF_SIZE];
	std::optional<size_t> rd_bytes = stream_->read(read_buf, sizeof(read_buf));
	if (!rd_bytes.has_value()) return ;
	if (rd_bytes.value() == 0) {
		read_closed_ = true;
		return ;
	}
	proceccing_buf_ += std::string(read_buf, rd_bytes.value());
	for (;;) {
		std::optional<ProcessResult>	process_result = handler_(proceccing_buf_);
		if (!process_result.has_value()) return ;
		proceccing_buf_.erase(0, process_result.value().consumed);
		write_buf_ += process_result.value().response;
	}
}

void	Client::try_write(void) {
	if (write_buf_.empty()) return ;
	std::optional<size_t>	wr_bytes = stream_->write(write_buf_.data(), write_buf_.length());
	if (!wr_bytes.has_value()) return ;
	write_buf_.erase(0, wr_bytes.value());
}

bool	Client::is_finished(void) const {
	return (read_closed_ /* && write_buf_.empty() // 未完成のデータをアプリケーションプロトコル側がエラー判定してくれるならこの条件ONでもいい*/);
}

void	Server::accept_client(void) {
	std::unique_ptr<IByteStream>	new_stream = transport_->accept();
	if (new_stream != nullptr) {
		poller_->register_target(new_stream.get(), POLL_IN);
		clients_.try_emplace(new_stream->handle(), std::move(new_stream), handler_);
	}
}

void	Server::handle_event(Event& event) {
	if (event.handle == transport_->handle()) {
		accept_client();
		return ;
	}
	std::map<Handle, Client>::iterator it = clients_.find(event.handle);
	if (it == clients_.end()) return ;
	try {
		Client& client = it->second;
		if (event.event_flag & POLL_IN && client.need_read())
			client.try_read();
		if (event.event_flag & POLL_OUT && client.need_write())
			client.try_write();
		if (event.event_flag & POLL_ERROR)
			;
		handle_client_state(client);
	} catch (std::system_error& e) {
		if (e.code() == std::errc::interrupted) throw ;
		std::cerr << "Client Error: " << e.what() << std::endl;
		poller_->unregister_target(it->second.pollable());
		clients_.erase(it);
	}
}

void	Server::handle_client_state(Client& client) {
	if (client.need_read() && client.need_write())
		poller_->modify_target(client.pollable(), POLL_IN | POLL_OUT);
	else if (!client.need_read() && client.need_write())
		poller_->modify_target(client.pollable(), POLL_OUT);
	else if (client.need_read() && !client.need_write())
		poller_->modify_target(client.pollable(), POLL_IN);
	else {
		poller_->unregister_target(client.pollable());
		clients_.erase(client.handle());
	}
}

void	Server::run(void) {
	while (!should_stop_()) {
		try {
			std::vector<Event>	events = poller_->poll(1000);
			for (std::vector<Event>::iterator ev = events.begin(); ev != events.end(); ++ev)
				handle_event(*ev);
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) break ;
			throw ;
		}
	}
	std::cout << "\rServer terminated" << std::endl;
}
