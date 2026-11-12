#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include <vector>
# include "Transport.hpp"

using ClientHandler = std::function<std::optional<std::string>(const std::string&)>;
using ShouldStop = std::function<bool(void)>;

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, ClientHandler h);
		void	handle_client(void);
	private:
		std::unique_ptr<IByteStream>	stream_;
		ClientHandler					handler_;
		std::string						write_buf_;
		static const std::size_t		BUF_SIZE = 2;
};

class Server {
	public:
		Server(std::unique_ptr<ITransport> transport, ClientHandler handler, ShouldStop should_stop);
		void	run(void);
	private:
		void	handle_client(Client& client);
		std::unique_ptr<ITransport>	transport_;
		std::vector<Client>			clients_;
		ClientHandler				handler_;
		ShouldStop					should_stop_;
};

#endif
