#include "lexer.h"

#include <cctype>
#include <stdexcept>
#include <string_view>
#include <utility>
Lexer::Lexer(std::string source) : src(std::move(source)) {}
void Lexer::reset() { pos = 0; }
Token Lexer::next_token() {
  while (pos < src.size()) {
    if (std::isspace(static_cast<unsigned char>(src[pos]))) {
      pos++;
    } else if (pos + 1 < src.size() && src[pos] == '/' && src[pos + 1] == '/') {
      pos += 2;
      while (pos < src.size() && src[pos] != '\n') {
        pos++;
      }
    } else {
      break;
    }
  }
  if (pos >= src.size()) return {TokenType::End, ""};

  char c = src[pos];

  // number case
  if (std::isdigit(static_cast<unsigned char>(c))) {
    size_t start = pos;
    while (pos < src.size() &&
           std::isdigit(static_cast<unsigned char>(src[pos])))
      pos++;
    return {TokenType::Number, src.substr(start, pos - start)};
  }

  // identifiers
  if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
    size_t start = pos;
    while (pos < src.size() &&
           (std::isalnum(static_cast<unsigned char>(src[pos])) ||
            src[pos] == '_'))
      pos++;
    std::string word = src.substr(start, pos - start);
    if (word == "true" || word == "false") {
      return {TokenType::Boolean, word};
    } else if (word == "let" || word == "if" || word == "else" ||
               word == "for" || word == "while" || word == "print") {
      return {TokenType::Keyword, word};
    }
    return {TokenType::Identifier, word};
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

  pos++;

  if (c == ';') return {TokenType::SemiColon, ";"};
  if (c == '(') return {TokenType::OpenParen, "("};
  if (c == ')') return {TokenType::CloseParen, ")"};
  if (c == '{') return {TokenType::OpenBrace, "{"};
  if (c == '}') return {TokenType::CloseBrace, "}"};
  if (c == '[') return {TokenType::OpenBracket, "["};
  if (c == ']') return {TokenType::CloseBracket, "]"};
  if (c == ',') return {TokenType::Comma, ","};

  if (std::string_view("+-*/%=!<>").find(c) != std::string_view::npos) {
    return {TokenType::Operator, std::string(1, c)};
  }

  throw std::runtime_error(std::string("Unexpected character: ") + c);
}

std::string print_token(Token t) {
  switch (t.type) {
    case TokenType::Number:
      return "Number[" + t.text + "]";
    case TokenType::Boolean:
      return "Boolean[" + t.text + "]";
    case TokenType::Identifier:
      return "Identifier[" + t.text + "]";
    case TokenType::Operator:
      return "Operator[" + t.text + "]";
    case TokenType::Keyword:
      return "Keyword[" + t.text + "]";
    case TokenType::SemiColon:
      return "SemiColon";
    case TokenType::OpenParen:
      return "OpenParen";
    case TokenType::CloseParen:
      return "CloseParen";
    case TokenType::OpenBrace:
      return "OpenBrace";
    case TokenType::CloseBrace:
      return "CloseBrace";
    case TokenType::OpenBracket:
      return "OpenBracket";
    case TokenType::CloseBracket:
      return "CloseBracket";
    case TokenType::Comma:
      return "Comma";
    case TokenType::End:
      return "End";
  }
  return "error";
}
