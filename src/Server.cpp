#include "Server.hpp"
#include <optional>
#include <stdexcept>
#include <iostream>

Reactor::Reactor(std::unique_ptr<IPoller> poller)
:poller_(std::move(poller)) {
	if (!poller_) throw std::invalid_argument("Poller is null");
}

void	Reactor::run(const ShouldStop& should_stop) {
	while (!should_stop()) {
		try {
			std::vector<Event>	events = poller_->poll(1000);
			for (std::vector<Event>::iterator ev = events.begin(); ev != events.end(); ++ev)
				handle_event(*ev);
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) continue ;
			throw ;
		}
	}
}

void	Reactor::unregister_handler(IPollable* target) {
	poller_->unregister_target(target);
	event_handlers_.erase(target->handle());
}

void Reactor::register_handler(IPollable* target, int interests, EventHandler handler) {
	auto [it, inserted] = event_handlers_.try_emplace(
		target->handle(), std::move(handler));
	if (!inserted)
		throw std::logic_error("Duplicate event handler");
	try {
		poller_->register_target(target, interests);
	} catch (...) {
		event_handlers_.erase(it);
		throw;
	}
}

void	Reactor::handle_event(Event& event) {
	const auto it = event_handlers_.find(event.handle);
	if (it == event_handlers_.end()) return ;
	auto handler = it->second;
	handler(event);
}

void	Reactor::modify_handler(IPollable* target, int interests) {
	poller_->modify_target(target, interests);
}

Server::Server(ServerConfig server_config)
:transport_(std::move(server_config.transport)),
clients_(), handler_(server_config.handler),
should_stop_(std::move(server_config.should_stop)),
reactor_(std::move(server_config.poller)) {
	if (!transport_) throw std::invalid_argument("Transport is null");
	if (!should_stop_) throw std::invalid_argument("ShouldStop is empty");
}

void	Server::accept_client(void) {
	std::unique_ptr<IByteStream>	new_stream = transport_->accept();
	if (new_stream != nullptr) {
		auto [it, inserted] = clients_.try_emplace(new_stream->handle(), std::move(new_stream), handler_);
		if (!inserted) throw std::logic_error("Duplicate handle");
		try {
			Client& client = it->second;
			reactor_.register_handler(client.pollable(), client.registered_interests(), [this](Event& event) {
				handle_client_event(event);
			});
		} catch (...) {
			clients_.erase(it);
			throw;
		}
	}
}

void	Server::remove_client(Client& client) {
	const Handle handle = client.handle();
	reactor_.unregister_handler(client.pollable());
	clients_.erase(handle);
}

void	Server::handle_client_event(Event& event) {
	std::map<Handle, Client>::iterator it = clients_.find(event.handle);
	if (it == clients_.end()) return ;
	if (event.event_flag & IPoller::EVENT_ERROR) {
		remove_client(it->second);
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
		remove_client(client);
		return ;
	} catch (std::runtime_error &e) {
		std::cerr << "Client Error: " << e.what() << std::endl;
		remove_client(client);
		return ;
	}
	handle_client_state(client);
}

void	Server::handle_listening_event(Event& event) {
	if (event.event_flag & IPoller::EVENT_READABLE)
		accept_client();
}

void	Server::handle_client_state(Client& client) {
	unsigned int interests = 0;
	if (client.need_read())
		interests |= IPoller::EVENT_READABLE;
	if (client.need_write())
		interests |= IPoller::EVENT_WRITABLE;
	if (interests == 0) {
		remove_client(client);
		return ;
	}
	if (interests == client.registered_interests())
		return;
	reactor_.modify_handler(client.pollable(), interests);
	client.set_registered_interests(interests);
}
void	Server::run(void) {
	reactor_.register_handler(transport_.get(), IPoller::EVENT_READABLE, [this](Event& event) {
		handle_listening_event(event);
	});
	reactor_.run(should_stop_);
	std::cout << "\rServer terminated" << std::endl;
}
