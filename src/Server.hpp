#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include <vector>
# include "RequestProcessor.hpp"

class IByteStream {
	public:
		virtual ~IByteStream() = default;
		virtual std::optional<size_t> read(void* buf, size_t size) = 0;
		virtual std::optional<size_t> write(const void* buf, size_t size) = 0;
};

class ITransport {
	public:
		virtual ~ITransport() = default;
		virtual std::unique_ptr<IByteStream> accept() = 0;
};

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, RequestProcessor h);
		void	try_read(void);
		void	try_write(void);
		bool	is_finished(void) const;
		bool	need_read(void) const { return (!read_closed_); }
		bool	need_write(void) const { return (!write_buf_.empty()); }
	private:
		std::unique_ptr<IByteStream>	stream_;
		RequestProcessor				handler_;
		bool							read_closed_;
		std::string						proceccing_buf_;
		std::string						write_buf_;
		static const std::size_t		BUF_SIZE = 2;
};

using ShouldStop = std::function<bool(void)>;

class Server {
	public:
		Server(std::unique_ptr<ITransport> transport, RequestProcessor handler, ShouldStop should_stop);
		void	run(void);
	private:
		void	accept_client(void);
		void	prcess_clients(void);
		std::unique_ptr<ITransport>	transport_;
		std::vector<Client>			clients_;
		RequestProcessor			handler_;
		ShouldStop					should_stop_;
};

#endif
