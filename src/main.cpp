#include <iostream>
#include <stdexcept>
#include <memory>
#include <csignal>
#include <system_error>
#include <charconv>
#include <string_view>
#include "Server.hpp"
#include "TCPTransport.hpp"
#include "RequestProcessor.hpp"
#include "EpollPoller.hpp"

std::optional<std::string>	echo_protcol(const std::string& read_data);
// ProcessResult				mini_greeting_protcol(const std::string& read_data);
ProcessResult				mini_json_rpc(const std::string& input);
	
namespace {
	volatile sig_atomic_t	recived_signal = 0;

	void	signal_handler(int sig) {
		recived_signal = sig;
	}

	void	set_signal_handler(void) {
		struct sigaction sa = {};
		sa.sa_handler = signal_handler;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIGINT, &sa, nullptr) == -1 ||
			sigaction(SIGTERM, &sa, nullptr) == -1)
			throw std::system_error(errno, std::generic_category(), "sigaction");
	}

	// Server	make_tcp_mini_greeting_server(int port) {
	// 	std::unique_ptr<ITransport>	tcp = std::make_unique<TCPTransport>(port);
	// 	RequestProcessor	handler{Parser{mini_greeting_protcol}};
	// 	return (Server(ServerConfig{
	// 		.transport = std::move(tcp),
	// 		.handler = handler, 
	// 		.should_stop = []() { return (recived_signal == SIGINT || recived_signal == SIGTERM); },
	// 		.poller = std::make_unique<EpollPoller>()}));
	// }

	Server	make_tcp_mini_json_rpc_server(int port) {
		std::unique_ptr<ITransport>	tcp = std::make_unique<TCPTransport>(port);
		RequestProcessor	handler{Parser{mini_json_rpc}};
		return (Server(ServerConfig{
			.transport = std::move(tcp),
			.handler = handler, 
			.should_stop = []() { return (recived_signal == SIGINT || recived_signal == SIGTERM); },
			.poller = std::make_unique<EpollPoller>()}));
	}

	int parse_port(std::string_view input) {
		int port = 0;
		auto [end, ec] = std::from_chars(
			input.data(), input.data() + input.size(), port);

		if (ec != std::errc{} ||
			end != input.data() + input.size() ||
			port < 1 || port > 65535)
			throw std::invalid_argument("Port must be an integer from 1 to 65535");

		return port;
	}
}

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
		return (1);
	}
	try {
		set_signal_handler();
		Server	server = make_tcp_mini_json_rpc_server(parse_port(argv[1]));
		server.run();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
