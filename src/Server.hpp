#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include <vector>
# include "Transport.hpp"
# include "RequestProcessor.hpp"

using ShouldStop = std::function<bool(void)>;

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, RequestProcessor h);
		void	try_read(void);
		void	try_write(void);
		bool	is_finished(void) const;
	private:
		std::unique_ptr<IByteStream>	stream_;
		RequestProcessor				handler_;
		bool							read_closed_;
		std::string						proceccing_buf_;
		std::string						write_buf_;
		static const std::size_t		BUF_SIZE = 2;
};

class Server {
	public:
		Server(std::unique_ptr<ITransport> transport, RequestProcessor handler, ShouldStop should_stop);
		void	run(void);
	private:
		void	handle_client(Client& client);
		std::unique_ptr<ITransport>	transport_;
		std::vector<Client>			clients_;
		RequestProcessor			handler_;
		ShouldStop					should_stop_;
};

#endif
