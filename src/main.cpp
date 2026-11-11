#include <iostream>
#include <stdexcept>
#include "Server.hpp"

std::optional<std::string>	echo_protcol(const std::string& read_data);
std::optional<std::string>	mini_greeting_protcol(const std::string& read_data);

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
		return (1);
	}
	try {
		ClientHandler	handler = mini_greeting_protcol;
		Server	server(std::atoi(argv[1]), handler);
		server.run();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
