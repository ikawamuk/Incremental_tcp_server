#ifndef CLIENT_HPP_
# define CLIENT_HPP_

# include <memory>
# include "ByteStream.hpp"
# include "RequestProcessor.hpp"

class Client {
	public:
		Client(std::unique_ptr<IByteStream> s, RequestProcessor h);
		Handle			handle(void) { return (stream_->handle()); }
		IPollable*		pollable(void) { return (stream_.get()); }
		void			try_read(void);
		void			try_write(void);
		void			process_buffered_requests(void);
		bool			need_read(void) const;
		bool			need_write(void) const;
		unsigned int	registered_interests(void) const { return (registered_interests_); }
		void			set_registered_interests(unsigned int interests) {registered_interests_ = interests;}
	private:
		std::unique_ptr<IByteStream>	stream_;
		RequestProcessor				handler_;
		bool							read_closed_;
		std::string						proceccing_buf_;
		std::string						write_buf_;
		bool							read_paused_;
		unsigned int					registered_interests_;
		static const std::size_t		BUF_SIZE = 4 * 1024;
		static constexpr std::size_t	PAUSE_WRITE_BYTES = 48 * 1024;
		static constexpr std::size_t	RESUME_WRITE_BYTES = 16 * 1024;
		
};

#endif
