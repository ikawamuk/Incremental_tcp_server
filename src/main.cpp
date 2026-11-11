#include <iostream>
#include <stdexcept>
#include <memory>
#include <csignal>
#include <system_error>
#include "Server.hpp"
#include "TCPTransport.hpp"

std::optional<std::string>	echo_protcol(const std::string& read_data);
std::optional<std::string>	mini_greeting_protcol(const std::string& read_data);

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

Server	make_tcp_mini_greeting_server(int port) {
	std::unique_ptr<ITransport>	tcp = std::make_unique<TCPTransport>(port);
	ClientHandler	handler = mini_greeting_protcol;
	return (Server(std::move(tcp), handler, 
		[]() { return (recived_signal == SIGINT || recived_signal == SIGTERM); }));
}

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
		return (1);
	}
	try {
		set_signal_handler();
		Server	server = make_tcp_mini_greeting_server(std::atoi(argv[1]));
		server.run();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}


