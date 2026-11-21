#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <map>
# include "Transport.hpp"
# include "Poller.hpp"
# include "Client.hpp"

using ShouldStop = std::function<bool(void)>;

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
		void	handle_event(Event& event);
		void	accept_client(void);
		void	handle_client_state(Client& client);
		void	remove_client(std::map<Handle, Client>::iterator it);
		std::unique_ptr<ITransport>	transport_;
		std::map<Handle, Client>	clients_;
		RequestProcessor			handler_;
		ShouldStop					should_stop_;
		std::unique_ptr<IPoller>	poller_;
};

#endif
