#include <iostream>
#include <stdexcept>
#include "Server.hpp"

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <port number>" << std::endl;
		return (1);
	}
	try {
		Server	server(std::atoi(argv[1]));
		server.run();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
