#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include "Transport.hpp"

using ClientHandler = std::function<std::optional<std::string>(const std::string&)>;

class Server {
	public:
		Server(std::unique_ptr<ITransport> transport, ClientHandler handler);
		void	run(void);
	private:
		std::unique_ptr<ITransport>		transport_;
		ClientHandler					handler_;
		std::unique_ptr<IConnection>	client_;
		static const std::size_t		BUF_SIZE = 10;
};

#endif
