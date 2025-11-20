#include "../incl/Lexer.hpp"

static Token makeToken(TokenType type, const std::string &value, size_t line, size_t column) {
	Token t;
	t.type = type;
	t.value = value;
	t.line = line;
	t.column = column;
	return t;
}

Lexer::Lexer(const std::string &filename) : _index(0) {
	tokenize(filename);
}

const Token &Lexer::peek() const {
	if (_index >= _tokens.size())
		return _tokens[_tokens.size() - 1]; // le dernier doit être TOKEN_END
	return _tokens[_index];
}

Token Lexer::next() {
	if (_index >= _tokens.size())
		return _tokens[_tokens.size() - 1]; // TOKEN_END
	return _tokens[_index++];
}

bool Lexer::isEnd() const {
	return _index >= _tokens.size() || _tokens[_index].type == TOKEN_END;
}

void Lexer::tokenize(const std::string &filename) {
	std::ifstream in(filename.c_str());
	if (!in)
		throw std::runtime_error("Cannot open config file: " + filename);

	std::string lineStr;
	size_t lineNum = 0;

	while (std::getline(in, lineStr)) {
		++lineNum;
		size_t i = 0;

		while (i < lineStr.size()) {
			char c = lineStr[i];

			// Sauter les espaces
			if (std::isspace(static_cast<unsigned char>(c))) {
				++i;
				continue;
			}

			// Commentaire : # jusqu'à fin de ligne
			if (c == '#') {
				break; // ignore le reste de la ligne
			}

			// Symboles simples
			if (c == '{') {
				_tokens.push_back(makeToken(TOKEN_LBRACE, "{", lineNum, i + 1));
				++i;
				continue;
			} else if (c == '}') {
				_tokens.push_back(makeToken(TOKEN_RBRACE, "}", lineNum, i + 1));
				++i;
				continue;
			} else if (c == ';') {
				_tokens.push_back(makeToken(TOKEN_SEMICOLON, ";", lineNum, i + 1));
				++i;
				continue;
			}

			// Sinon : c'est un WORD
			size_t startCol = i;
			std::string word;

			while (i < lineStr.size()) {
				char d = lineStr[i];

				if (std::isspace(static_cast<unsigned char>(d)) || // autre token
					d == '{' || d == '}' || d == ';' || d == '#') {
					break;
				}
				word += d;
				++i;
			}

			if (!word.empty()) {
				_tokens.push_back(makeToken(TOKEN_WORD, word, lineNum, startCol + 1));
			}
		}
	}

	// Ajouter un token de fin
	_tokens.push_back(makeToken(TOKEN_END, "", lineNum + 1, 1));
}