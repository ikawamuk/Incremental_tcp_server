#include <iostream>
#include <stdexcept>
#include <memory>
#include "Server.hpp"
#include "TCPTransport.hpp"

std::optional<std::string>	echo_protcol(const std::string& read_data);
std::optional<std::string>	mini_greeting_protcol(const std::string& read_data);

Server	make_tcp_mini_greeting_server(int port) {
	std::unique_ptr<ITransport>	tcp = std::make_unique<TCPTransport>(port);
	ClientHandler	handler = mini_greeting_protcol;
	return (Server(std::move(tcp), handler));
}

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
		return (1);
	}
	try {
		Server	server = make_tcp_mini_greeting_server(std::atoi(argv[1]));
		server.run();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}


