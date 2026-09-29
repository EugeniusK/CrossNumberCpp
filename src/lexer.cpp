#include "lexer.h"

#include <string>

Lexer::Lexer(std::string source) : src(source) {};

Token Lexer::next_token() {
  while (pos < src.size() && std::isspace(src[pos])) pos++;
  if (pos >= src.size()) return {TokenType::End, ""};

  char c = src[pos];

  // number case
  if (std::isdigit(c)) {
    size_t start = pos;
    while (pos < src.size() && std::isdigit(src[pos])) pos++;
    return {TokenType::Number, src.substr(start, pos - start)};
  }

  // identifiers
  if (std::isalpha(c)) {
    size_t start = pos;
    while (pos < src.size() && std::isalpha(src[pos])) pos++;
    return {TokenType::Identifier, src.substr(start, pos - start)};
  }

  // two character operations
  if (pos + 1 < src.size()) {
    std::string pair = src.substr(pos, 2);
    if (pair == "==" || pair == "!=" || pair == "<=" || pair == ">=" ||
        pair == "&&" || pair == "||") {
      pos += 2;
      return {TokenType::Operator, pair};
    }
  }

  // one character operations
  if (std::string("+-*/%<>").find(c) != std::string::npos) {
    pos++;
    return {TokenType::Operator, std::string(1, c)};
  }

  // parentheses
  if (c == '(') {
    pos++;
    return {TokenType::OpenParen, "("};
  }
  if (c == ')') {
    pos++;
    return {TokenType::CloseParen, ")"};
  }
}

std::string print_token(Token t) {
  std::string type;
  switch (static_cast<int>(t.type)) {
    case 0:
      return "Number( " + t.text + " )";
    case 1:
      return "Identifier( " + t.text + " )";
    case 2:
      return "Operator( " + t.text + " )";
    case 3:
      return "OpenParam";
    case 4:
      return "CloseParam";
    case 5:
      return "End";
    default:
      return "error";
  }
}
