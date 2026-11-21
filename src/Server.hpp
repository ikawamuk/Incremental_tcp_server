#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <map>
# include "Transport.hpp"
# include "Poller.hpp"
# include "Client.hpp"

using EventHandler = std::function<void(Event&)>;
using ShouldStop = std::function<bool(void)>;

class Reactor {
	public:
		explicit Reactor(std::unique_ptr<IPoller> poller);
		void	run(const ShouldStop& should_stop);
		void	register_handler(IPollable* target, int interests, EventHandler handler);
		void	unregister_handler(IPollable* target);
		void	modify_handler(IPollable* target, int interests);
	private:
		void	handle_event(Event& event);
		std::unique_ptr<IPoller>		poller_;
		std::map<Handle, EventHandler>	event_handlers_;
};



struct ServerConfig {
	std::unique_ptr<ITransport>	transport;
	RequestProcessor			handler;
	ShouldStop					should_stop;
	std::unique_ptr<IPoller>	poller;
};

class Server {
	public:
		explicit Server(ServerConfig server_config);
		void	run(void);
	private:
		void	handle_client_event(Event& event);
		void	handle_listening_event(Event& event);
		void	accept_client(void);
		void	handle_client_state(Client& client);
		void	remove_client(Client& client);
		std::unique_ptr<ITransport>		transport_;
		std::map<Handle, Client>		clients_;
		RequestProcessor				handler_;
		ShouldStop						should_stop_;
		Reactor							reactor_;
};

#endif
