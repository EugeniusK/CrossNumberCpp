#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>

#include "lexer.h"
#include "runtime.h"
class ExprNode {
 public:
  virtual ~ExprNode() = default;
  virtual int evaluate(const Context& ctx) = 0;
};

class NumberNode : public ExprNode {
 public:
  NumberNode(int val);
  int evaluate(const Context&) override;

 private:
  int value;
};

class VariableNode : public ExprNode {
 public:
  VariableNode(std::string var_name);
  int evaluate(const Context&) override;

 private:
  std::string name;
};

class BinaryOpNode : public ExprNode {
 public:
  BinaryOpNode(std::string oper, std::unique_ptr<ExprNode> l,
               std::unique_ptr<ExprNode> r);
  int evaluate(const Context&) override;

 private:
  std::string op;
  std::unique_ptr<ExprNode> lhs;
  std::unique_ptr<ExprNode> rhs;
};

class Parser {
 public:
  Parser(Lexer l);
  std::unique_ptr<ExprNode> parse();

 private:
  Lexer lexer;
  Token curr;
  void advance();

  std::unique_ptr<ExprNode> parse_logical_or();
  std::unique_ptr<ExprNode> parse_logical_and();
  std::unique_ptr<ExprNode> parse_equality();
  std::unique_ptr<ExprNode> parse_additive();
  std::unique_ptr<ExprNode> parse_multiplicative();
  std::unique_ptr<ExprNode> parse_primary();
};
#endif