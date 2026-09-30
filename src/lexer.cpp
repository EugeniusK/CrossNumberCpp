#include "lexer_new.h"

Lexer::Lexer(std::string source) : src(std::move(source)) {};

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
  if (std::isalpha(c) || c == '_') {
    size_t start = pos;
    while (pos < src.size() && (std::isalnum(src[pos]) || src[pos] == '_'))
      pos++;
    std::string word = src.substr(start, pos - start);
    if (word == "true" || word == "false") {
      return {TokenType::Boolean, word};
    } else if (word == "let" || word == "if" || word == "else" ||
               word == "for" || word == "print") {
      return {TokenType::Keyword, word};
    }
    return {TokenType::Identifier, word};
  };

  // two character operations
  if (pos + 1 < src.size()) {
    std::string pair = src.substr(pos, 2);
    if (pair == "==" || pair == "!=" || pair == "<=" || pair == ">=" ||
        pair == "&&" || pair == "||") {
      pos += 2;
      return {TokenType::Operator, pair};
    }
  };

  pos++;

  if (c == ';') return {TokenType::SemiColon, ";"};
  if (c == '(') return {TokenType::OpenParen, "("};
  if (c == ')') return {TokenType::CloseParen, ")"};
  if (c == '{') return {TokenType::OpenBrace, "{"};
  if (c == '}') return {TokenType::CloseBrace, "}"};
  if (c == '[') return {TokenType::OpenBracket, "["};
  if (c == ']') return {TokenType::CloseBracket, "]"};
  if (c == ',') return {TokenType::Comma, ","};

  if (std::string("+-*/%=!<>").find(c) != std::string::npos) {
    return {TokenType::Operator, std::string(1, c)};
  };

  throw std::runtime_error(std::string("Unexpected character: ") + c);
}

std::string print_token(Token t) {
  std::string type;
  switch (static_cast<int>(t.type)) {
    case 0:
      return "Number[" + t.text + "]";
    case 1:
      return "Boolean[" + t.text + "]";
    case 2:
      return "Identifier[" + t.text + "]";
    case 3:
      return "Operator[" + t.text + "]";
    case 4:
      return "Keyword[" + t.text + "]";
    case 5:
      return "Newline";
    case 6:
      return "OpenParen";
    case 7:
      return "CloseParen";
    case 8:
      return "OpenBrace";
    case 9:
      return "ClosBrace";
    case 10:
      return "End";
    default:
      return "error";
  }
}
