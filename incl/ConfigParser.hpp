#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Lexer.hpp"
#include "Config.hpp"
#include "lib.hpp"

class ConfigParser {
public:
	ConfigParser();
	ServerConfig parse(const std::string &filename);

private:
	Lexer *_lexer;
	Token  _cur;

	void   advance();
	void   expect(TokenType type, const std::string &msg, const Token &t);
	std::string lineAndCol(const Token &t);

	void   parseServer(ServerConfig &server);
	void   parseServerDirective(ServerConfig &server);

	int    parseInt(const std::string &s, const Token &t);
	size_t parseSize(const std::string &s, const Token &t);
};

#endif