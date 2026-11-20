#include "Server.hpp"
#include <optional>
#include <stdexcept>
#include <iostream>

Server::Server(ServerConfig server_config)
:transport_(std::move(server_config.transport)),
clients_(), handler_(server_config.handler),
should_stop_(server_config.should_stop),
poller_(std::move(server_config.poller)) {
	if (!transport_) throw std::invalid_argument("Transport is null");
	if (!poller_) throw std::invalid_argument("Poller is null");
	if (!should_stop_) throw std::invalid_argument("ShouldStop is empty");
}

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
		ProcessResult	process_result = handler_.process(proceccing_buf_);
		if (std::get_if<ParseNeedMore>(&process_result)) return ;
		if (auto err = std::get_if<ParseError>(&process_result)) throw std::runtime_error(err->message);
		if (auto success = std::get_if<ParseSuccess>(&process_result)) {
			if (success->consumed == 0 || success->consumed > proceccing_buf_.size())
				throw std::logic_error("Invalid consumed size");
			proceccing_buf_.erase(0, success->consumed);
			write_buf_ += success->response;
			continue ;
		}
		std::logic_error("Unknown ParseResult");
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
		auto [it, inserted] = clients_.try_emplace(new_stream->handle(), std::move(new_stream), handler_);
		if (!inserted) throw std::logic_error("Duplicate handle");
		try {
			poller_->register_target(it->second.pollable(), IPoller::EVENT_READABLE);
		} catch (...) {
			clients_.erase(it);
			throw;
		}
	}
}

void	Server::remove_client(std::map<Handle, Client>::iterator it) {
	poller_->unregister_target(it->second.pollable());
	clients_.erase(it);
}

void	Server::handle_event(Event& event) {
	if (event.handle == transport_->handle()) {
		if (event.event_flag & IPoller::EVENT_READABLE)
			accept_client();
		return ;
	}
	std::map<Handle, Client>::iterator it = clients_.find(event.handle);
	if (it == clients_.end()) return ;
	Client& client = it->second;
	try {
		if (event.event_flag & IPoller::EVENT_READABLE && client.need_read())
			client.try_read();
		if (event.event_flag & IPoller::EVENT_WRITABLE && client.need_write())
			client.try_write();
		// if (event.event_flag & IPoller::EVENT_ERROR) ;
	} catch (std::system_error& e) {
		if (e.code() == std::errc::interrupted) throw ;
		std::cerr << "Client Error: " << e.what() << std::endl;
		remove_client(it);
		return ;
	} catch (std::runtime_error &e) {
		std::cerr << "Client Error: " << e.what() << std::endl;
		remove_client(it);
		return ;
	}
	handle_client_state(client);
}

void	Server::handle_client_state(Client& client) {
	unsigned int interests = 0;
	if (client.need_read())
		interests |= IPoller::EVENT_READABLE;
	if (client.need_write())
		interests |= IPoller::EVENT_WRITABLE;
	if (interests != 0)
		poller_->modify_target(client.pollable(), interests);
	else {
		poller_->unregister_target(client.pollable());
		clients_.erase(client.handle());
	}
}
void	Server::run(void) {
	poller_->register_target(transport_.get(), IPoller::EVENT_READABLE);
	while (!should_stop_()) {
		try {
			std::vector<Event>	events = poller_->poll(1000);
			for (std::vector<Event>::iterator ev = events.begin(); ev != events.end(); ++ev)
				handle_event(*ev);
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) continue ;
			throw ;
		}
	}
	std::cout << "\rServer terminated" << std::endl;
}
