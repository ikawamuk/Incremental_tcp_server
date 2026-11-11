#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>

using ClientHandler = std::function<std::optional<std::string>(const std::string&)>;

class Server {
	public:
		Server(int port, ClientHandler handler);
		~Server();
		void	run(void);
	private:
		int							server_fd_;
		int							client_fd_;
		ClientHandler				handler_;
		static const int			LISTEN_QUEUE_NUMBER = 10;
		static const std::size_t	BUF_SIZE			= 10;
};

#endif
