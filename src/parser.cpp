#include "parser.h"

#include "runtime.h"
NumberNode::NumberNode(int val) : ExprNode() { this->value = val; };

int NumberNode::evaluate(const Context&) { return value; };

VariableNode::VariableNode(std::string var_name) : ExprNode() {
  this->name = var_name;
}

int VariableNode::evaluate(const Context& ctx) {
  auto it = ctx.find(name);
  if (it == ctx.end()) {
    throw std::runtime_error("Undefined variable: " + name);
  }
  return it->second;
}

BinaryOpNode::BinaryOpNode(std::string oper, std::unique_ptr<ExprNode> l,
                           std::unique_ptr<ExprNode> r) {
  this->op = oper;
  this->lhs = std::move(l);
  this->rhs = std::move(r);
}

int BinaryOpNode::evaluate(const Context& ctx) {
  int left = lhs->evaluate(ctx);
  int right = rhs->evaluate(ctx);
  if (op == "+") return left + right;
  if (op == "-") return left - right;
  if (op == "*") return left * right;
  if (op == "/") return right != 0 ? left / right : 0;
  if (op == "%") return right != 0 ? left % right : 0;
  if (op == "==") return left == right;
  if (op == "!=") return left != right;
  if (op == "<") return left < right;
  if (op == ">") return left > right;
  if (op == "<=") return left <= right;
  if (op == ">=") return left >= right;
  if (op == "&&") return left && right;
  if (op == "||") return left || right;

  throw std::runtime_error("Unknown operator: " + op);
}

std::unique_ptr<ExprNode> Parser::parse_logical_or() {
  auto node = parse_logical_and();
  while (curr.type == TokenType::Operator && curr.text == "||") {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_logical_and());
  }
  return node;
}

// Precedence level 2: &&
std::unique_ptr<ExprNode> Parser::parse_logical_and() {
  auto node = parse_equality();
  while (curr.type == TokenType::Operator && curr.text == "&&") {
    std::string op = curr.text;
    advance();
    node =
        std::make_unique<BinaryOpNode>(op, std::move(node), parse_equality());
  }
  return node;
}

// Precedence level 3: ==, !=, <, >, <=, >=
std::unique_ptr<ExprNode> Parser::parse_equality() {
  auto node = parse_additive();
  while (curr.type == TokenType::Operator &&
         (curr.text == "==" || curr.text == "!=" || curr.text == "<" ||
          curr.text == ">" || curr.text == "<=" || curr.text == ">=")) {
    std::string op = curr.text;
    advance();
    node =
        std::make_unique<BinaryOpNode>(op, std::move(node), parse_additive());
  }
  return node;
}

// Precedence level 4: +, -
std::unique_ptr<ExprNode> Parser::parse_additive() {
  auto node = parse_multiplicative();
  while (curr.type == TokenType::Operator &&
         (curr.text == "+" || curr.text == "-")) {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_multiplicative());
  }
  return node;
}

// Precedence level 5: *, /, %
std::unique_ptr<ExprNode> Parser::parse_multiplicative() {
  auto node = parse_primary();
  while (curr.type == TokenType::Operator &&
         (curr.text == "*" || curr.text == "/" || curr.text == "%")) {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node), parse_primary());
  }
  return node;
}

std::unique_ptr<ExprNode> Parser::parse_primary() {
  if (curr.type == TokenType::Number) {
    int val = std::stoi(curr.text);
    advance();
    return std::make_unique<NumberNode>(val);
  }

  if (curr.type == TokenType::Identifier) {
    std::string id = curr.text;
    advance();

    // // Function call: func(arg)
    // if (curr.type == TokenType::OpenParen) {
    //   advance();  // consume '('
    //   auto arg = parse();
    //   if (curr.type != TokenType::CloseParen) {
    //     throw std::runtime_error(
    //         "Expected closing parenthesis after function argument");
    //   }
    //   advance();  // consume ')'
    //   return std::make_unique<FunctionNode>(id, std::move(arg));
    // }

    return std::make_unique<VariableNode>(id);
  }

  if (curr.type == TokenType::OpenParen) {
    advance();
    auto node = parse();
    if (curr.type != TokenType::CloseParen) {
      throw std::runtime_error("Missing closing parenthesis");
    }
    advance();
    return node;
  }

  throw std::runtime_error("Syntax error near token: " + curr.text);
}

Parser::Parser(Lexer l) : lexer(std::move(l)) { this->advance(); };

std::unique_ptr<ExprNode> Parser::parse() { return parse_logical_or(); }

void Parser::advance() { curr = lexer.next_token(); }
