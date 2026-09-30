#ifndef AST_H
#define AST_H

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "builtin.h"

class Environment {
 public:
  std::unordered_map<std::string, int>::iterator find_var(std::string id);
  int get_var(std::string id);
  void set_var(std::string id, int val);
  void initialise_output_array(int len, int val);
  int get_output_array(int idx);
  void set_output_array(int idx, int val);
  void initialise_tmp_array(int len, int val);
  int get_tmp_array(int idx);
  void set_tmp_array(int idx, int val);
  std::unordered_map<std::string, int>::iterator invalid_var;

 private:
  std::unordered_map<std::string, int> named_variable;
  std::vector<int> output_array;
  std::vector<int> tmp_array;
};

class ExprNode {
 public:
  virtual ~ExprNode() = default;
  virtual int evaluate(Environment& env) = 0;
};

class LiteralNode : public ExprNode {
 public:
  LiteralNode(int val);
  int evaluate(Environment& env);

 private:
  int value;
};

class VariableNode : public ExprNode {
 public:
  VariableNode(std::string n);
  int evaluate(Environment& env);

 private:
  std::string name;
};

class UnaryOpNode : public ExprNode {
 public:
  UnaryOpNode(std::string o, std::unique_ptr<ExprNode> expr);
  int evaluate(Environment& env);

 private:
  std::string op;
  std::unique_ptr<ExprNode> operand;
};

class BinaryOpNode : public ExprNode {
 public:
  BinaryOpNode(std::string o, std::unique_ptr<ExprNode> expr1,
               std::unique_ptr<ExprNode> expr2);
  int evaluate(Environment& env);

 private:
  std::string op;
  std::unique_ptr<ExprNode> operand1;
  std::unique_ptr<ExprNode> operand2;
};

class IndexReadNode : public ExprNode {
 public:
  IndexReadNode(std::string n, std::unique_ptr<ExprNode> i);
  int evaluate(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> expr;
};

class FunctionCallNode : public ExprNode {
 public:
  FunctionCallNode(std::string n, std::vector<std::unique_ptr<ExprNode>> a);
  int evaluate(Environment& env);

 private:
  std::string name;
  std::vector<std::unique_ptr<ExprNode>> args;
};

class StmtNode {
 public:
  virtual ~StmtNode() = default;
  virtual void execute(Environment& env) = 0;
};

class BlockStmtNode : public StmtNode {
 public:
  void add_statement(std::unique_ptr<StmtNode> stmt);
  void execute(Environment& env);

 private:
  std::vector<std::unique_ptr<StmtNode>> statements;
};

class VarDeclNode : public StmtNode {
 public:
  VarDeclNode(std::string n, std::unique_ptr<ExprNode> i);
  void execute(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> init;
};

class AssignStmtNode : public StmtNode {
 public:
  AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e);
  void execute(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> expr;
};

class IndexAssignStmtNode : public StmtNode {
 public:
  IndexAssignStmtNode(std::string n, std::unique_ptr<ExprNode> i,
                      std::unique_ptr<ExprNode> v);
  void execute(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> idx_expr;
  std::unique_ptr<ExprNode> val_expr;
};

class PrintStmtNode : public StmtNode {
 public:
  explicit PrintStmtNode(std::unique_ptr<ExprNode> e);
  void execute(Environment& env);

 private:
  std::unique_ptr<ExprNode> expr;
};

class IfStmtNode : public StmtNode {
 public:
  IfStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t,
             std::unique_ptr<StmtNode> e);

  void execute(Environment& env);

 private:
  std::unique_ptr<ExprNode> cond;
  std::unique_ptr<StmtNode> then_branch;
  std::unique_ptr<StmtNode> else_branch;
};

class ForStmtNode : public StmtNode {
 public:
  ForStmtNode(std::unique_ptr<StmtNode> i, std::unique_ptr<ExprNode> c,
              std::unique_ptr<StmtNode> u, std::unique_ptr<StmtNode> b);

  void execute(Environment& env);

 private:
  std::unique_ptr<StmtNode> init;
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> update;
  std::unique_ptr<StmtNode> body;
};

#endif