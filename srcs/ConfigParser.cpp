#include "../incl/ConfigParser.hpp"

ConfigParser::ConfigParser() : _lexer(NULL) {}

ServerConfig ConfigParser::parse(const std::string &filename) {
	Lexer lexer(filename);
	_lexer = &lexer;
	_cur = _lexer->next();
	if (_lexer->isEnd()) {
		throw std::runtime_error(lineAndCol(_cur) + "Config file is empty");
	}
	if (!(_cur.type == TOKEN_WORD && _cur.value == "server")) {
		throw std::runtime_error(lineAndCol(_cur) + "Config must start with 'server'");
	}
	ServerConfig server;
	parseServer(server);
	if (!_lexer->isEnd()) {
		throw std::runtime_error(lineAndCol(_cur) + "Unexpected tokens after 'server' block");
	}
	return server;
}

// helper
void ConfigParser::advance() {
	_cur = _lexer->next();
}

// helper
void ConfigParser::expect(TokenType type, const std::string &msg, const Token &t) {
	if (_cur.type != type) {
		throw std::runtime_error(lineAndCol(t) + msg);
	}
}

// helper
std::string ConfigParser::lineAndCol(const Token &t) {
    std::ostringstream oss;
    oss << "line " << t.line << ", column " << t.column << ": ";
    return oss.str();
}

int ConfigParser::parseInt(const std::string &s, const Token &t) {
    char *end = 0;
    long v = std::strtol(s.c_str(), &end, 10);
    if (*end || v < 0 || v > 65535) {
        throw std::runtime_error(lineAndCol(t) + "Invalid integer: " + s);
    }
    return static_cast<int>(v);
}

// pas de K/M, juste en bytes
size_t ConfigParser::parseSize(const std::string &s, const Token &t) {
    char *end = 0;
    long v = std::strtol(s.c_str(), &end, 10);
    if (*end || v < 0) {
        throw std::runtime_error(lineAndCol(t) + "Invalid size: " + s);
    }
    return static_cast<size_t>(v);
}

void ConfigParser::parseServer(ServerConfig &server) {
    advance(); // consomme server
    expect(TOKEN_LBRACE, "Expected '{' after 'server'", _cur);
    advance(); // consomme la brace
	while (!_lexer->isEnd() && _cur.type != TOKEN_RBRACE) {
        if (_cur.type != TOKEN_WORD) {
            throw std::runtime_error(lineAndCol(_cur) + "Unexpected token in server block");
        }
        parseServerDirective(server);
    }
	expect(TOKEN_RBRACE, "Missing '}' at end of server block", _cur);
    advance(); // consomme rbrace pour etre à end
}

void ConfigParser::parseServerDirective(ServerConfig &server) {
    std::string name = _cur.value;
    advance();

    if (name == "listen") {
		expect(TOKEN_WORD, "Expected value after 'listen'", _cur);
        std::string value = _cur.value;
        advance();
        expect(TOKEN_SEMICOLON, "Missing ';' after listen", _cur);
        advance();

        ListenAddress la;
        std::string::size_type pos = value.find(':');
        if (pos == std::string::npos) {
            // pas de host
            la.host = "";
            la.port = parseInt(value, _cur);
        } else {
            la.host = value.substr(0, pos);
            la.port = parseInt(value.substr(pos + 1), _cur);
        }
        server.listens.push_back(la);
    } else if (name == "root") {
		expect(TOKEN_WORD, "Expected path after 'root'", _cur);
        server.root = _cur.value;
        advance();
        expect(TOKEN_SEMICOLON, "Missing ';' after root", _cur);
        advance();
    } else if (name == "index") {
		expect(TOKEN_WORD, "Expected at least one file after 'index'", _cur);
        while (_cur.type == TOKEN_WORD) {
            server.indexFiles.push_back(_cur.value);
            advance();
        }
        expect(TOKEN_SEMICOLON, "Missing ';' after index", _cur);
        advance();
    } else if (name == "client_max_body_size") {
		expect(TOKEN_WORD, "Expected size after 'client_max_body_size'", _cur);
        server.clientMaxBodySize = parseSize(_cur.value, _cur);
        advance();
        expect(TOKEN_SEMICOLON, "Missing ';' after client_max_body_size", _cur);
        advance();
    } else if (name == "error_page") {
        std::vector<int> codes;
        while (_cur.type == TOKEN_WORD) {
            const std::string &v = _cur.value;
            bool isNum = true;
            for (size_t i = 0; i < v.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(v[i]))) {
                    isNum = false;
                    break;
                }
            }
            if (!isNum)
                break;
            codes.push_back(parseInt(v, _cur));
            advance();
        }
        if (codes.empty()) {
            throw std::runtime_error(lineAndCol(_cur) + "Expected error code(s) after 'error_page'");
        }

		expect(TOKEN_WORD, "Expected path after error codes", _cur);
        std::string path = _cur.value;
        advance();
        expect(TOKEN_SEMICOLON, "Missing ';' after error_page", _cur);
        advance();
        for (size_t i = 0; i < codes.size(); ++i) {
            server.errorPages[codes[i]] = path;
        }
    } else if (name == "location") {
		// ! WARN ! à faire plus tard
		advance();
		expect(TOKEN_LBRACE, "Expected '{' after 'location <name>'", _cur);
		advance();
		while (_cur.type != TOKEN_RBRACE) {
			advance();
		}
		expect(TOKEN_RBRACE, "Missing '}' at end of location block", _cur);
		advance();
	} else {
        throw std::runtime_error(lineAndCol(_cur) + "Unknown server directive: '" + name + "'");
    }
}
