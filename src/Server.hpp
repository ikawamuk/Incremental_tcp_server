#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <cstddef>
# include <string>
# include <functional>
# include <optional>
# include <memory>
# include <map>
# include "RequestProcessor.hpp"
# include "Poller.hpp"

class IByteStream: public IPollable {
	public:
		virtual ~IByteStream() = default;
		virtual Handle	handle(void) const noexcept = 0;
		virtual std::optional<size_t> read(void* buf, size_t size) = 0;
		virtual std::optional<size_t> write(const void* buf, size_t size) = 0;
};

class ITransport: public IPollable {
	public:
		virtual ~ITransport() = default;
		virtual Handle	handle(void) const noexcept = 0;
		virtual std::unique_ptr<IByteStream> accept() = 0;
};

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, RequestProcessor h);
		Handle	handle(void) { return (stream_->handle()); }
		IPollable*  pollable(void) { return (stream_.get()); }
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
