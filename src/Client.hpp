#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <memory>
# include "ByteStream.hpp"
# include "RequestProcessor.hpp"

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, RequestProcessor h);
		Handle		handle(void) { return (stream_->handle()); }
		IPollable*	pollable(void) { return (stream_.get()); }
		void		try_read(void);
		void		try_write(void);
		bool		is_finished(void) const;
		bool		need_read(void) const { return (!read_closed_); }
		bool		need_write(void) const { return (!write_buf_.empty()); }
	private:
		std::unique_ptr<IByteStream>	stream_;
		RequestProcessor				handler_;
		bool							read_closed_;
		std::string						proceccing_buf_;
		std::string						write_buf_;
		static const std::size_t		BUF_SIZE = 2;
};

#endif
