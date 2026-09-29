#ifndef LEXER_H
#define LEXER_H

#include <string>

enum class TokenType {
  Number,
  Identifier,
  Operator,
  OpenParen,
  CloseParen,
  End
};

struct Token {
  TokenType type;
  std::string text;
};

class Lexer {
 public:
  Lexer(std::string source);
  Token next_token();

 private:
  std::string src;
  size_t pos = 0;
};

std::string print_token(Token t);

#endif