#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <map>
# include "Client.hpp"
# include "Reactor.hpp"

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
