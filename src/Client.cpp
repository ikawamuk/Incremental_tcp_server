#include "Client.hpp"
#include <stdexcept>

Client::Client(std::unique_ptr<IByteStream> s, RequestProcessor h)
:stream_(std::move(s)), handler_(h), read_closed_(false), read_paused_(false) {}

void	Client::try_read(void) {
	char	read_buf[BUF_SIZE];
	std::optional<size_t> rd_bytes = stream_->read(read_buf, sizeof(read_buf));
	if (!rd_bytes.has_value()) return ;
	if (rd_bytes.value() == 0) {
		read_closed_ = true;
		return ;
	}
	proceccing_buf_ += std::string(read_buf, rd_bytes.value());
	process_buffered_requests();
}

void	Client::try_write(void) {
	if (write_buf_.empty()) return ;
	std::optional<size_t>	wr_bytes = stream_->write(write_buf_.data(), write_buf_.length());
	if (!wr_bytes.has_value()) return ;
	write_buf_.erase(0, wr_bytes.value());
	if (read_paused_ && write_buf_.size() <= RESUME_WRITE_BYTES) {
		read_paused_ = false;
		process_buffered_requests();
	}
}

void	Client::process_buffered_requests(void) {
	for (;;) {
		ProcessResult	process_result = handler_.process(proceccing_buf_);
		if (std::get_if<NeedMore>(&process_result)) return ;
		if (auto err = std::get_if<Discard>(&process_result)) {
			proceccing_buf_.erase(0, err->consumed);
			continue ;
		}
		if (auto success = std::get_if<Processed>(&process_result)) {
			if (success->consumed == 0 || success->consumed > proceccing_buf_.size())
				throw std::logic_error("Invalid consumed size");
			constexpr std::size_t MAX_PENDING_BYTES = 64 * 1024;
			if (success->response.size() > MAX_PENDING_BYTES - write_buf_.size())
				throw std::runtime_error("Send buffer limit exceeded");
			write_buf_ += success->response;
			proceccing_buf_.erase(0, success->consumed);
			if (write_buf_.size() >= PAUSE_WRITE_BYTES) {
				read_paused_ = true;
				return;
			}
			continue ;
		}
		std::logic_error("Unknown ParseResult");
	}
}

bool	Client::need_read(void) const { return (!read_closed_ && !read_paused_); }

bool	Client::need_write(void) const { return (!write_buf_.empty()); }
