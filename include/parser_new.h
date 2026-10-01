#ifndef PARSER_NEW_H
#define PARSER_NEW_H

#include <set>

#include "ast.h"
#include "lexer_new.h"

class Parser {
 public:
  Parser(Lexer l);
  std::unique_ptr<BlockStmtNode> parse_program();
  void reset();
  std::set<std::string> list_variables;
  std::string print_list_variables();

 private:
  Lexer lexer;
  Token curr;
  void advance();
  void expect(const std::string& text);

  std::unique_ptr<StmtNode> parse_statement();
  std::unique_ptr<StmtNode> parse_var_decl();
  std::unique_ptr<StmtNode> parse_if_stmt();
  std::unique_ptr<StmtNode> parse_for_stmt();
  std::unique_ptr<StmtNode> parse_while_stmt();
  std::unique_ptr<StmtNode> parse_print_stmt();
  std::unique_ptr<StmtNode> parse_block();
  std::unique_ptr<StmtNode> parse_assign_stmt();

  std::unique_ptr<ExprNode> parse_expr();
  std::unique_ptr<ExprNode> parse_precedence_15();
  std::unique_ptr<ExprNode> parse_precedence_14();
  std::unique_ptr<ExprNode> parse_precedence_10();
  std::unique_ptr<ExprNode> parse_precedence_09();
  std::unique_ptr<ExprNode> parse_precedence_06();
  std::unique_ptr<ExprNode> parse_precedence_05();
  std::unique_ptr<ExprNode> parse_precedence_03();
  std::unique_ptr<ExprNode> parse_primary();

 protected:
  bool has_dependencies;
  std::set<std::string> list_dependencies;
};

#endif