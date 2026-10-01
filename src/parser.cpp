#include "builtin.h"
#include "parser_new.h"
void Parser::advance() { curr = lexer.next_token(); }

void Parser::reset() {
  lexer.reset();
  advance();
}
void Parser::expect(const std::string& text) {
  if (curr.text != text)
    throw std::runtime_error("Expected '" + text + "', got '" + curr.text +
                             "'");
  advance();
};

Parser::Parser(Lexer l) : lexer(std::move(l)) {
  advance();
  has_dependencies = false;
}

std::string Parser::print_list_variables() {
  std::string output = "";
  for (auto it = list_variables.begin(); it != list_variables.end(); it++) {
    output = output + *it + " ";
  };
  return output;
}

std::unique_ptr<BlockStmtNode> Parser::parse_program() {
  auto prog = std::make_unique<BlockStmtNode>();
  while (curr.type != TokenType::End) {
    prog->add_statement(parse_statement());
  }
  return prog;
}

std::unique_ptr<StmtNode> Parser::parse_statement() {
  if (curr.text == "let") {
    return parse_var_decl();
  }
  if (curr.text == "if") {
    return parse_if_stmt();
  }
  if (curr.text == "for") {
    return parse_for_stmt();
  }
  if (curr.text == "while") {
    return parse_while_stmt();
  }
  if (curr.text == "print") {
    return parse_print_stmt();
  }
  if (curr.type == TokenType::OpenBrace) {
    return parse_block();
  }

  std::string name = curr.text;
  expect(curr.text);

  if (curr.text == "[") {
    advance();
    auto idx = parse_expr();
    expect("]");
    expect("=");
    auto val = parse_expr();
    expect(";");
    return std::make_unique<IndexAssignStmtNode>(name, std::move(idx),
                                                 std::move(val));
  } else {
    // Case 2: Regular variable assignment -> x = expr;
    expect("=");
    auto expr = parse_expr();
    expect(";");
    return std::make_unique<AssignStmtNode>(name, std::move(expr));
  }
}

std::unique_ptr<StmtNode> Parser::parse_block() {
  expect("{");
  auto block = std::make_unique<BlockStmtNode>();
  while (curr.type != TokenType::CloseBrace && curr.type != TokenType::End) {
    block->add_statement(parse_statement());
  }
  expect("}");
  return block;
}

std::unique_ptr<StmtNode> Parser::parse_var_decl() {
  advance();  // consume 'let'
  std::string name = curr.text;
  expect(curr.text);
  expect("=");
  auto expr = parse_expr();
  expect(";");
  return std::make_unique<VarDeclNode>(name, std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parse_assign_stmt() {
  std::string name = curr.text;
  expect(curr.text);
  expect("=");
  auto expr = parse_expr();
  expect(";");
  return std::make_unique<AssignStmtNode>(name, std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parse_print_stmt() {
  advance();  // consume 'print'
  expect("(");
  auto expr = parse_expr();
  expect(")");
  expect(";");
  return std::make_unique<PrintStmtNode>(std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parse_if_stmt() {
  advance();  // consume 'if'
  expect("(");
  auto cond = parse_expr();
  expect(")");
  auto then_branch = parse_statement();
  std::unique_ptr<StmtNode> else_branch = nullptr;

  if (curr.text == "else") {
    advance();
    else_branch = parse_statement();
  }
  return std::make_unique<IfStmtNode>(std::move(cond), std::move(then_branch),
                                      std::move(else_branch));
}

std::unique_ptr<StmtNode> Parser::parse_for_stmt() {
  advance();  // consume 'for'
  expect("(");

  // 1. Init (let x = 0; OR x = 0;)
  std::unique_ptr<StmtNode> init = nullptr;
  if (curr.text == "let")
    init = parse_var_decl();
  else
    init = parse_assign_stmt();

  // 2. Condition (x < 10)
  auto cond = parse_expr();
  expect(";");

  // 3. Step/Update without trailing semicolon (x = x + 1)
  std::string update_var = curr.text;
  expect(curr.text);
  expect("=");
  auto update_expr = parse_expr();
  auto update =
      std::make_unique<AssignStmtNode>(update_var, std::move(update_expr));

  expect(")");

  // 4. Body
  auto body = parse_statement();

  return std::make_unique<ForStmtNode>(std::move(init), std::move(cond),
                                       std::move(update), std::move(body));
}

std::unique_ptr<StmtNode> Parser::parse_while_stmt() {
  advance();  // consume 'while'
  expect("(");
  auto cond = parse_expr();
  expect(")");
  auto body = parse_statement();
  return std::make_unique<WhileStmtNode>(std::move(cond), std::move(body));
}

std::unique_ptr<ExprNode> Parser::parse_expr() { return parse_precedence_15(); }
std::unique_ptr<ExprNode> Parser::parse_precedence_15() {
  auto node = parse_precedence_14();
  while (curr.text == "||") {
    advance();
    node = std::make_unique<BinaryOpNode>("||", std::move(node),
                                          parse_precedence_14());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_14() {
  auto node = parse_precedence_10();
  while (curr.text == "&&") {
    advance();
    node = std::make_unique<BinaryOpNode>("&&", std::move(node),
                                          parse_precedence_10());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_10() {
  auto node = parse_precedence_09();
  while (curr.text == "==" || curr.text == "!=") {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_precedence_09());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_09() {
  auto node = parse_precedence_06();
  while (curr.text == "<" || curr.text == "<=" || curr.text == ">" ||
         curr.text == ">=") {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_precedence_06());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_06() {
  auto node = parse_precedence_05();
  while (curr.text == "+" || curr.text == "-") {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_precedence_05());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_05() {
  auto node = parse_precedence_03();
  while (curr.text == "*" || curr.text == "/" || curr.text == "%") {
    std::string op = curr.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node),
                                          parse_precedence_03());
  }
  return node;
}
std::unique_ptr<ExprNode> Parser::parse_precedence_03() {
  if (curr.text == "+" || curr.text == "-" || curr.text == "!") {
    std::string op = curr.text;
    advance();
    return std::make_unique<UnaryOpNode>(op, parse_precedence_03());
  }
  return parse_primary();
}
std::unique_ptr<ExprNode> Parser::parse_primary() {
  if (curr.type == TokenType::Number) {
    int val = std::stoi(curr.text);
    advance();
    return std::make_unique<LiteralNode>(val);
  }
  if (curr.type == TokenType::Boolean) {
    int val = (curr.text == "true") ? 1 : 0;
    advance();
    return std::make_unique<LiteralNode>(val);
  }
  if (curr.type == TokenType::Identifier) {
    std::string id = curr.text;
    advance();
    if (curr.text == "(") {
      advance();
      std::vector<std::unique_ptr<ExprNode>> args;
      if (curr.text != ")") {
        args.push_back(parse_expr());
        while (curr.text == ",") {
          advance();  // consume ','
          args.push_back(parse_expr());
        }
      }

      expect(")");
      return std::make_unique<FunctionCallNode>(id, std::move(args));
    }

    list_variables.insert(id);
    if ((id[0] == 'a' || id[0] == 'd') && id.size() > 1) {
      bool reserved = true;
      for (int i = 1; i < id.size() - 1; i++) {
        if (!std::isdigit(id[i])) {
          reserved = false;
          break;
        }
      }
      if (reserved) {
        has_dependencies = true;
        list_dependencies.insert(id);
      };
    }

    if (id[0] == 'c' && id.size() == 2 && std::isdigit(id[1])) {
      has_dependencies = true;
      list_dependencies.insert(id);
    }

    return std::make_unique<VariableNode>(id);
  }
  if (curr.type == TokenType::OpenParen) {
    advance();
    auto expr = parse_expr();
    expect(")");
    return expr;
  }
  throw std::runtime_error("Unexpected token in expression: " + curr.text);
}
