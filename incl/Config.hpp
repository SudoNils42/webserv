#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "lib.hpp"

struct ListenAddress {
	std::string host;
	int         port;
	ListenAddress() : host(""), port(0) {}
};

struct ServerConfig {
	std::vector<ListenAddress> listens;
	std::string root;
	std::vector<std::string> indexFiles;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorPages;
	ServerConfig() : clientMaxBodySize(0) {}
};

#endif