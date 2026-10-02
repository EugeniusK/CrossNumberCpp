#include "ast.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "builtin.h"

std::unordered_map<std::string, int>::iterator Environment::find_var(
    std::string id) {
  return named_variable.find(id);
};
std::unordered_map<std::string, int>::iterator Environment::var_end() {
  return named_variable.end();
}
int Environment::get_var(std::string id) { return named_variable[id]; };
void Environment::set_var(std::string id, int val) {
  if (id == "OUTPUT_ARRAY_LENGTH" || id == "TMP_ARRAY_LENGTH") {
    throw std::runtime_error("Cannot modify reserved variable " + id);
  };
  named_variable[id] = val;
};

void Environment::initialise_tmp_array(int len, int val) {
  tmp_array.assign(len, val);
  written_tmp_indices.clear();
  named_variable["TMP_ARRAY_LENGTH"] = len;
}

void Environment::reset_output_array() {
  for (int idx : written_output_indices) {
    output_array[idx] = 0;
  }
  written_output_indices.clear();
}

int Environment::get_tmp_array(int idx) {
  if (idx < 0 || idx >= static_cast<int>(tmp_array.size())) {
    throw std::runtime_error(
        "Attempt to access outsize allowed range for tmp_array");
  };
  return tmp_array[idx];
}

void Environment::set_tmp_array(int idx, int val) {
  if (idx < 0 || idx >= static_cast<int>(tmp_array.size())) {
    throw std::runtime_error(
        "Attempt to set outsize allowed range for tmp_array");
  };
  if (tmp_array[idx] == 0 && val != 0) {
    written_tmp_indices.push_back(idx);
  }
  tmp_array[idx] = val;
}

void Environment::initialise_output_array(int len, int val) {
  output_array.assign(len, val);
  written_output_indices.clear();
  named_variable["OUTPUT_ARRAY_LENGTH"] = len;
}

void Environment::reset_tmp_array() {
  for (int idx : written_tmp_indices) {
    tmp_array[idx] = 0;
  }
  written_tmp_indices.clear();
}

int Environment::get_output_array(int idx) {
  if (idx < 0 || idx >= static_cast<int>(output_array.size())) {
    throw std::runtime_error(
        "Attempt to access outsize allowed range for output_array");
  };
  return output_array[idx];
}

void Environment::set_output_array(int idx, int val) {
  if (idx < 0 || idx >= static_cast<int>(output_array.size())) {
    throw std::runtime_error(
        "Attempt to set outsize allowed range for output_array");
  };
  if (output_array[idx] == 0 && val != 0) {
    written_output_indices.push_back(idx);
  }
  output_array[idx] = val;
}

namespace {
bool is_reserved_variable_name(std::string_view name) {
  if ((name[0] == 'a' || name[0] == 'd') && name.size() > 1) {
    return std::all_of(name.begin() + 1, name.end(), [](unsigned char c) {
      return std::isdigit(c);
    });
  }
  return name[0] == 'c' && name.size() == 2 &&
         std::isdigit(static_cast<unsigned char>(name[1]));
}
}  // namespace

LiteralNode::LiteralNode(int val) : value(val) {}
int LiteralNode::evaluate(Environment& env) { return value; }

VariableNode::VariableNode(std::string n) : name(std::move(n)) {}
int VariableNode::evaluate(Environment& env) {
  auto result = env.find_var(name);
  if (result == env.var_end()) {
    throw std::runtime_error("Undefined variable: " + name);
  }
  return env.get_var(name);
}

UnaryOpNode::UnaryOpNode(std::string o, std::unique_ptr<ExprNode> expr)
    : op(std::move(o)), operand(std::move(expr)) {};
int UnaryOpNode::evaluate(Environment& env) {
  int val = operand->evaluate(env);
  if (op == "+") return +val;
  if (op == "-") return -val;
  if (op == "!") return val == 0 ? 1 : 0;
  throw std::runtime_error("Unknown unary operator: " + op);
}

BinaryOpNode::BinaryOpNode(std::string o, std::unique_ptr<ExprNode> expr1,
                           std::unique_ptr<ExprNode> expr2)
    : op(std::move(o)),
      operand1(std::move(expr1)),
      operand2(std::move(expr2)) {};
int BinaryOpNode::evaluate(Environment& env) {
  if (op == "&&") {
    int left = operand1->evaluate(env);
    if (left == 0) return 0;
    return operand2->evaluate(env) != 0 ? 1 : 0;
  }
  if (op == "||") {
    int left = operand1->evaluate(env);
    if (left != 0) return 1;
    return operand2->evaluate(env) != 0 ? 1 : 0;
  }

  int left = operand1->evaluate(env);
  int right = operand2->evaluate(env);

  if (op == "+") return left + right;
  if (op == "-") return left - right;
  if (op == "*") return left * right;
  if (op == "/") return right != 0 ? left / right : 0;
  if (op == "%") return right != 0 ? left % right : 0;
  if (op == "==") return left == right ? 1 : 0;
  if (op == "!=") return left != right ? 1 : 0;
  if (op == "<") return left < right ? 1 : 0;
  if (op == "<=") return left <= right ? 1 : 0;
  if (op == ">") return left > right ? 1 : 0;
  if (op == ">=") return left >= right ? 1 : 0;

  throw std::runtime_error("Unknown binary operator: " + op);
}

IndexReadNode::IndexReadNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), expr(std::move(i)) {}
int IndexReadNode::evaluate(Environment& env) {
  if (name != "tmp" && name != "output") {
    throw std::runtime_error("Read from invalid array: " + name);
  } else {
    int idx = expr->evaluate(env);
    if (name == "tmp") {
      return env.get_tmp_array(idx);
    } else {
      return env.get_output_array(idx);
    }
  }
}

FunctionCallNode::FunctionCallNode(std::string n,
                                   std::vector<std::unique_ptr<ExprNode>> a)
    : name(std::move(n)), args(std::move(a)) {}

int FunctionCallNode::evaluate(Environment& env) {
  const auto& table = get_builtin_functions();
  auto it = table.find(name);
  if (it == table.end()) {
    throw std::runtime_error("Unknown function: " + name);
  }

  std::vector<int> evaluated_args;
  evaluated_args.reserve(args.size());
  for (const auto& arg : args) {
    evaluated_args.push_back(arg->evaluate(env));
  }
  return it->second(evaluated_args);
};

void BlockStmtNode::add_statement(std::unique_ptr<StmtNode> stmt) {
  statements.push_back(std::move(stmt));
}
void BlockStmtNode::execute(Environment& env) {
  for (auto& s : statements) s->execute(env);
}

VarDeclNode::VarDeclNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), init(std::move(i)) {}
void VarDeclNode::execute(Environment& env) {
  if (is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot declare to reserved variable: " + name);
  }
  env.set_var(name, init->evaluate(env));
}

AssignStmtNode::AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e)
    : name(std::move(n)), expr(std::move(e)) {}
void AssignStmtNode::execute(Environment& env) {
  if (env.find_var(name) == env.var_end()) {
    throw std::runtime_error("Assignment to undeclared: " + name);
  }
  if (is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot assign to reserved variable: " + name);
  }
  env.set_var(name, expr->evaluate(env));
}

IndexAssignStmtNode::IndexAssignStmtNode(std::string n,
                                         std::unique_ptr<ExprNode> i,
                                         std::unique_ptr<ExprNode> v)
    : name(std::move(n)), idx_expr(std::move(i)), val_expr(std::move(v)) {}
void IndexAssignStmtNode::execute(Environment& env) {
  int idx = idx_expr->evaluate(env);
  int val = val_expr->evaluate(env);
  if (name != "tmp" && name != "output") {
    throw std::runtime_error("Assignment to invalid array: " + name);
  } else {
    if (name == "tmp") {
      env.set_tmp_array(idx, val);
    } else {
      env.set_output_array(idx, val);
    }
  }
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> e)
    : expr(std::move(e)) {}
void PrintStmtNode::execute(Environment& env) {
  std::cout << "printed: " << expr->evaluate(env) << "\n";
};

IfStmtNode::IfStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t,
                       std::unique_ptr<StmtNode> e)
    : cond(std::move(c)),
      then_branch(std::move(t)),
      else_branch(std::move(e)) {}
void IfStmtNode::execute(Environment& env) {
  if (cond->evaluate(env) != 0) {
    then_branch->execute(env);
  } else if (else_branch) {
    else_branch->execute(env);
  }
};

ForStmtNode::ForStmtNode(std::unique_ptr<StmtNode> i,
                         std::unique_ptr<ExprNode> c,
                         std::unique_ptr<StmtNode> u,
                         std::unique_ptr<StmtNode> b)
    : init(std::move(i)),
      condition(std::move(c)),
      update(std::move(u)),
      body(std::move(b)) {}
void ForStmtNode::execute(Environment& env) {
  if (init) init->execute(env);
  while (condition->evaluate(env) != 0) {
    body->execute(env);
    if (update) update->execute(env);
  }
};

WhileStmtNode::WhileStmtNode(std::unique_ptr<ExprNode> cond,
                             std::unique_ptr<StmtNode> b)
    : condition(std::move(cond)), body(std::move(b)) {}
void WhileStmtNode::execute(Environment& env) {
  while (condition->evaluate(env) != 0) {
    body->execute(env);
  }
};