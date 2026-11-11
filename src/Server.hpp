#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>

class Server {
	public:
		Server(int port);
		~Server();
		void	run(void);
	private:
		int	server_fd_;
		int	client_fd_;
		static const int			LISTEN_QUEUE_NUMBER = 10;
		static const std::size_t	BUF_SIZE			= 10;
};

#endif
