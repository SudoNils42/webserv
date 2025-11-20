#ifndef LEXER_HPP
#define LEXER_HPP

#include "lib.hpp"

enum TokenType {
	TOKEN_WORD,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_SEMICOLON,
	TOKEN_END
};

struct Token {
	TokenType	type;
	std::string	value;
	size_t		line;
	size_t		column;
};


class Lexer {
	public:
		Lexer(const std::string &filename);

		const Token &peek() const; // regarder le token courant
		Token next(); // avancer et retourner le token courant
		bool isEnd() const;

	private:
		std::vector<Token>	_tokens;
		size_t				_index;

		void tokenize(const std::string &filename);
};

#endif