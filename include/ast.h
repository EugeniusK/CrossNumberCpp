#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "builtin.h"

class Environment {
 public:
  Environment();

  // Slot-based indexing
  int get_or_create_slot(const std::string& id);
  int find_slot(const std::string& id) const;

  int get_slot_value(int slot) const {
    return slots_[slot];
  }

  void set_slot_value(int slot, int val) {
    slots_[slot] = val;
  }

  // Name-based API
  bool has_var(const std::string& id) const;
  int get_var(const std::string& id) const;
  void set_var(const std::string& id, int val);

  std::unordered_map<std::string, int>::iterator find_var(const std::string& id);
  std::unordered_map<std::string, int>::iterator var_end();

  void initialise_output_array(int len, int val = 0);
  void reset_output_array();
  int get_output_array(int idx);
  void set_output_array(int idx, int val);
  void initialise_tmp_array(int len, int val = 0);
  void reset_tmp_array();
  int get_tmp_array(int idx);
  void set_tmp_array(int idx, int val);
  const std::vector<int>& get_written_output_indices() const {
    return written_output_indices;
  }

 private:
  std::unordered_map<std::string, int> var_to_slot_;
  std::vector<int> slots_;
  std::vector<int> output_array;
  std::vector<int> tmp_array;
  std::vector<int> written_output_indices;
  std::vector<int> written_tmp_indices;
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

// "x"
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

// tmp[x] or output[x]
class IndexReadNode : public ExprNode {
 public:
  IndexReadNode(std::string n, std::unique_ptr<ExprNode> i);
  int evaluate(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> expr;
};

// func(x,y)
class FunctionCallNode : public ExprNode {
 public:
  FunctionCallNode(std::string n, std::vector<std::unique_ptr<ExprNode>> a);
  int evaluate(Environment& env) override;

 private:
  std::string name;
  std::vector<std::unique_ptr<ExprNode>> args;
  BuiltinFn fn_;
  mutable std::vector<int> evaluated_args;
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

// let x = 5;
class VarDeclNode : public StmtNode {
 public:
  VarDeclNode(std::string n, std::unique_ptr<ExprNode> i);
  void execute(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> init;
};

// x = 5;
class AssignStmtNode : public StmtNode {
 public:
  AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e);
  void execute(Environment& env);

 private:
  std::string name;
  std::unique_ptr<ExprNode> expr;
};

// tmp[x] = 5;
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

// print(x);
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

class WhileStmtNode : public StmtNode {
 public:
  WhileStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b);
  void execute(Environment& env);

 private:
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> body;
};

#endif