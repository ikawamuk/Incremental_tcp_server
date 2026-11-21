#include "Server.hpp"
#include <optional>
#include <stdexcept>
#include <iostream>

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
