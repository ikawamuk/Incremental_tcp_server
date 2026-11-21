#include "Server.hpp"
#include <optional>
#include <stdexcept>
#include <iostream>

Server::Server(ServerConfig server_config)
:transport_(std::move(server_config.transport)),
clients_(), handler_(server_config.handler),
should_stop_(std::move(server_config.should_stop)),
poller_(std::move(server_config.poller)) {
	if (!transport_) throw std::invalid_argument("Transport is null");
	if (!poller_) throw std::invalid_argument("Poller is null");
	if (!should_stop_) throw std::invalid_argument("ShouldStop is empty");
}

void	Server::accept_client(void) {
	std::unique_ptr<IByteStream>	new_stream = transport_->accept();
	if (new_stream != nullptr) {
		auto [it, inserted] = clients_.try_emplace(new_stream->handle(), std::move(new_stream), handler_);
		if (!inserted) throw std::logic_error("Duplicate handle");
		try {
			Client& client = it->second;
			poller_->register_target(client.pollable(), client.registered_interests());
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
	if (event.event_flag & IPoller::EVENT_ERROR) {
		remove_client(it);
		return ;
	}
	Client& client = it->second;
	try {
		if (event.event_flag & IPoller::EVENT_READABLE && client.need_read())
			client.try_read();
		if (event.event_flag & IPoller::EVENT_WRITABLE && client.need_write())
			client.try_write();
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
	if (interests == 0) {
		poller_->unregister_target(client.pollable());
		clients_.erase(client.handle());
		return ;
	}
	if (interests == client.registered_interests())
		return;
	poller_->modify_target(client.pollable(), interests);
	client.set_registered_interests(interests);
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
