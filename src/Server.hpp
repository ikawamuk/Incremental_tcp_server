#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include "Transport.hpp"

using ClientHandler = std::function<std::optional<std::string>(const std::string&)>;
using ShouldStop = std::function<bool(void)>;

class Server {
	public:
		Server(std::unique_ptr<ITransport> transport, ClientHandler handler, ShouldStop should_stop);
		void	run(void);
	private:
		void	handle_client(void);
		std::unique_ptr<ITransport>		transport_;
		ClientHandler					handler_;
		std::unique_ptr<IByteStream>	client_;
		ShouldStop						should_stop_;
		static const std::size_t		BUF_SIZE = 10;
};

#endif
