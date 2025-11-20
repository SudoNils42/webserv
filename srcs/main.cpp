#include "../incl/lib.hpp"
#include "../incl/SocketManager.hpp"
#include "../incl/Config.hpp"
#include "../incl/ConfigParser.hpp"

int main(int ac, char ** av) {
	if (ac != 2) {
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}
	ConfigParser parser;
	ServerConfig config = parser.parse(av[1]);
    SocketManager server(config.listens[0].port);
    server.run();
    return 0;
}