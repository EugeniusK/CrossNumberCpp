#ifndef LEXER_H
#define LEXER_H

#include <string>

enum class TokenType {
  Number,
  Boolean,
  Identifier,
  Operator,
  Keyword,
  SemiColon,
  OpenParen,
  CloseParen,
  OpenBrace,
  CloseBrace,
  OpenBracket,
  CloseBracket,
  Comma,
  End
};

struct Token {
  TokenType type;
  std::string text;
};

class Lexer {
 public:
  Lexer(std::string source);
  void reset();
  Token next_token();

 private:
  std::string src;
  size_t pos = 0;
};

std::string print_token(Token t);

#endif