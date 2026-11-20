#include "Client.hpp"

Client::Client(std::unique_ptr<IByteStream> s, RequestProcessor h)
:stream_(std::move(s)), handler_(h), read_closed_(false) {}

void	Client::try_read(void) {
	char	read_buf[BUF_SIZE];
	std::optional<size_t> rd_bytes = stream_->read(read_buf, sizeof(read_buf));
	if (!rd_bytes.has_value()) return ;
	if (rd_bytes.value() == 0) {
		read_closed_ = true;
		return ;
	}
	proceccing_buf_ += std::string(read_buf, rd_bytes.value());
	for (;;) {
		ProcessResult	process_result = handler_.process(proceccing_buf_);
		if (std::get_if<ParseNeedMore>(&process_result)) return ;
		if (auto err = std::get_if<ParseError>(&process_result)) throw std::runtime_error(err->message);
		if (auto success = std::get_if<ParseSuccess>(&process_result)) {
			if (success->consumed == 0 || success->consumed > proceccing_buf_.size())
				throw std::logic_error("Invalid consumed size");
			proceccing_buf_.erase(0, success->consumed);
			write_buf_ += success->response;
			continue ;
		}
		std::logic_error("Unknown ParseResult");
	}
}

void	Client::try_write(void) {
	if (write_buf_.empty()) return ;
	std::optional<size_t>	wr_bytes = stream_->write(write_buf_.data(), write_buf_.length());
	if (!wr_bytes.has_value()) return ;
	write_buf_.erase(0, wr_bytes.value());
}

bool	Client::is_finished(void) const {
	return (read_closed_ /* && write_buf_.empty() // 未完成のデータをアプリケーションプロトコル側がエラー判定してくれるならこの条件ONでもいい*/);
}
