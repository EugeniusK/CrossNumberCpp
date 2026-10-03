#include "ast.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "builtin.h"

Environment::Environment() {
  get_or_create_slot("OUTPUT_ARRAY_LENGTH");
  get_or_create_slot("TMP_ARRAY_LENGTH");
}

int Environment::get_or_create_slot(const std::string& id) {
  auto it = var_to_slot_.find(id);
  if (it != var_to_slot_.end()) {
    return it->second;
  }
  int slot = static_cast<int>(slots_.size());
  var_to_slot_.emplace(id, slot);
  slots_.push_back(0);
  return slot;
}

int Environment::find_slot(const std::string& id) const {
  auto it = var_to_slot_.find(id);
  if (it != var_to_slot_.end()) {
    return it->second;
  }
  return -1;
}

bool Environment::has_var(const std::string& id) const {
  return var_to_slot_.find(id) != var_to_slot_.end();
}

std::unordered_map<std::string, int>::iterator Environment::find_var(
    const std::string& id) {
  return var_to_slot_.find(id);
}

std::unordered_map<std::string, int>::iterator Environment::var_end() {
  return var_to_slot_.end();
}

int Environment::get_var(const std::string& id) const {
  int slot = find_slot(id);
  if (slot < 0) {
    throw std::runtime_error("Undefined variable: " + id);
  }
  return slots_[slot];
}

void Environment::set_var(const std::string& id, int val) {
  if (id == "OUTPUT_ARRAY_LENGTH" || id == "TMP_ARRAY_LENGTH") {
    throw std::runtime_error("Cannot modify reserved variable " + id);
  }
  int slot = get_or_create_slot(id);
  slots_[slot] = val;
}

void Environment::initialise_tmp_array(int len, int val) {
  tmp_array.assign(len, val);
  written_tmp_indices.clear();
  slots_[1] = len;
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
  slots_[0] = len;
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
void LiteralNode::resolve_slots(Environment& env) {}

VariableNode::VariableNode(std::string n) : name(std::move(n)), slot_(-1) {
  if (name == "OUTPUT_ARRAY_LENGTH") {
    slot_ = 0;
  } else if (name == "TMP_ARRAY_LENGTH") {
    slot_ = 1;
  }
}

int VariableNode::evaluate(Environment& env) {
  return env.get_slot_value(slot_);
}

void VariableNode::resolve_slots(Environment& env) {
  if (slot_ < 0) {
    slot_ = env.find_slot(name);
    if (slot_ < 0) {
      throw std::runtime_error("Undefined variable: " + name);
    }
  }
}

UnaryOp string_to_unary_op(std::string_view o) {
  if (o == "+") return UnaryOp::Plus;
  if (o == "-") return UnaryOp::Minus;
  if (o == "!") return UnaryOp::LogicalNot;
  throw std::runtime_error("Unknown unary operator: " + std::string(o));
}

BinaryOp string_to_binary_op(std::string_view o) {
  if (o == "+") return BinaryOp::Add;
  if (o == "-") return BinaryOp::Subtract;
  if (o == "*") return BinaryOp::Multiply;
  if (o == "/") return BinaryOp::Divide;
  if (o == "%") return BinaryOp::Modulo;
  if (o == "==") return BinaryOp::Equal;
  if (o == "!=") return BinaryOp::NotEqual;
  if (o == "<") return BinaryOp::Less;
  if (o == "<=") return BinaryOp::LessEqual;
  if (o == ">") return BinaryOp::Greater;
  if (o == ">=") return BinaryOp::GreaterEqual;
  if (o == "&&") return BinaryOp::LogicalAnd;
  if (o == "||") return BinaryOp::LogicalOr;
  throw std::runtime_error("Unknown binary operator: " + std::string(o));
}

UnaryOpNode::UnaryOpNode(UnaryOp o, std::unique_ptr<ExprNode> expr)
    : op(o), operand(std::move(expr)), operand_(operand.get()) {}

UnaryOpNode::UnaryOpNode(const std::string& o, std::unique_ptr<ExprNode> expr)
    : op(string_to_unary_op(o)), operand(std::move(expr)), operand_(operand.get()) {}

int UnaryOpNode::evaluate(Environment& env) {
  int val = operand_->evaluate(env);
  switch (op) {
    case UnaryOp::Plus:
      return +val;
    case UnaryOp::Minus:
      return -val;
    case UnaryOp::LogicalNot:
      return val == 0 ? 1 : 0;
    default:
      __builtin_unreachable();
  }
}

void UnaryOpNode::resolve_slots(Environment& env) {
  if (operand) operand->resolve_slots(env);
}

BinaryOpNode::BinaryOpNode(BinaryOp o, std::unique_ptr<ExprNode> expr1,
                           std::unique_ptr<ExprNode> expr2)
    : op(o),
      operand1(std::move(expr1)),
      operand2(std::move(expr2)),
      left_(operand1.get()),
      right_(operand2.get()) {}

BinaryOpNode::BinaryOpNode(const std::string& o,
                           std::unique_ptr<ExprNode> expr1,
                           std::unique_ptr<ExprNode> expr2)
    : op(string_to_binary_op(o)),
      operand1(std::move(expr1)),
      operand2(std::move(expr2)),
      left_(operand1.get()),
      right_(operand2.get()) {}

int BinaryOpNode::evaluate(Environment& env) {
  switch (op) {
    case BinaryOp::LogicalAnd: {
      int left = left_->evaluate(env);
      if (left == 0) return 0;
      return right_->evaluate(env) != 0 ? 1 : 0;
    }
    case BinaryOp::LogicalOr: {
      int left = left_->evaluate(env);
      if (left != 0) return 1;
      return right_->evaluate(env) != 0 ? 1 : 0;
    }
    case BinaryOp::Add:
      return left_->evaluate(env) + right_->evaluate(env);
    case BinaryOp::Subtract:
      return left_->evaluate(env) - right_->evaluate(env);
    case BinaryOp::Multiply:
      return left_->evaluate(env) * right_->evaluate(env);
    case BinaryOp::Divide: {
      int left = left_->evaluate(env);
      int right = right_->evaluate(env);
      return right != 0 ? left / right : 0;
    }
    case BinaryOp::Modulo: {
      int left = left_->evaluate(env);
      int right = right_->evaluate(env);
      return right != 0 ? left % right : 0;
    }
    case BinaryOp::Equal:
      return left_->evaluate(env) == right_->evaluate(env) ? 1 : 0;
    case BinaryOp::NotEqual:
      return left_->evaluate(env) != right_->evaluate(env) ? 1 : 0;
    case BinaryOp::Less:
      return left_->evaluate(env) < right_->evaluate(env) ? 1 : 0;
    case BinaryOp::LessEqual:
      return left_->evaluate(env) <= right_->evaluate(env) ? 1 : 0;
    case BinaryOp::Greater:
      return left_->evaluate(env) > right_->evaluate(env) ? 1 : 0;
    case BinaryOp::GreaterEqual:
      return left_->evaluate(env) >= right_->evaluate(env) ? 1 : 0;
    default:
      __builtin_unreachable();
  }
}

void BinaryOpNode::resolve_slots(Environment& env) {
  if (operand1) operand1->resolve_slots(env);
  if (operand2) operand2->resolve_slots(env);
}

IndexReadNode::IndexReadNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), expr(std::move(i)) {
  if (name != "tmp" && name != "output") {
    throw std::runtime_error("Read from invalid array: " + name);
  }
  is_tmp_ = (name == "tmp");
}

int IndexReadNode::evaluate(Environment& env) {
  int idx = expr->evaluate(env);
  return is_tmp_ ? env.get_tmp_array(idx) : env.get_output_array(idx);
}

void IndexReadNode::resolve_slots(Environment& env) {
  if (expr) expr->resolve_slots(env);
}

FunctionCallNode::FunctionCallNode(std::string n,
                                   std::vector<std::unique_ptr<ExprNode>> a)
    : name(std::move(n)), args(std::move(a)) {
  arity_ = args.size();
  if (arity_ > 4) {
    throw std::runtime_error("Function '" + name + "' called with " +
                             std::to_string(arity_) +
                             " arguments, but at most 4 are supported");
  }
  for (size_t i = 0; i < arity_; ++i) {
    args_[i] = args[i].get();
  }

  if (name == "dsum") {
    if (arity_ != 1) {
      throw std::runtime_error("dsum() requires exactly 1 argument");
    }
    builtin_id_ = BuiltinFn::Dsum;
  } else if (name == "get_nth_digit") {
    if (arity_ != 2 && arity_ != 3) {
      throw std::runtime_error("get_nth_digit() requires 2 or 3 arguments");
    }
    builtin_id_ = BuiltinFn::GetNthDigit;
  } else if (name == "is_prime" || name == "prime") {
    if (arity_ != 1) {
      throw std::runtime_error("is_prime() requires exactly 1 argument");
    }
    builtin_id_ = BuiltinFn::IsPrime;
  } else if (name == "is_palindrome" || name == "palindrome") {
    if (arity_ != 1) {
      throw std::runtime_error("is_palindrome() requires exactly 1 argument");
    }
    builtin_id_ = BuiltinFn::IsPalindrome;
  } else if (name == "pow") {
    if (arity_ != 2) {
      throw std::runtime_error("pow() requires exactly 2 arguments");
    }
    builtin_id_ = BuiltinFn::Pow;
  } else if (name == "isqrt") {
    if (arity_ != 1) {
      throw std::runtime_error("isqrt() requires exactly 1 argument");
    }
    builtin_id_ = BuiltinFn::Isqrt;
  } else if (name == "reverse") {
    if (arity_ != 1) {
      throw std::runtime_error("reverse() requires exactly 1 argument");
    }
    builtin_id_ = BuiltinFn::Reverse;
  } else {
    throw std::runtime_error("Unknown function: " + name);
  }
}

int FunctionCallNode::evaluate(Environment& env) {
  switch (builtin_id_) {
    case BuiltinFn::Dsum:
      return dsum(args_[0]->evaluate(env));
    case BuiltinFn::GetNthDigit:
      if (arity_ == 3) {
        return get_nth_digit(args_[0]->evaluate(env),
                              args_[1]->evaluate(env) - 1,
                              args_[2]->evaluate(env));
      }
      return get_nth_digit(args_[0]->evaluate(env),
                            args_[1]->evaluate(env) - 1);
    case BuiltinFn::IsPrime:
      return is_prime(args_[0]->evaluate(env)) ? 1 : 0;
    case BuiltinFn::IsPalindrome: {
      int val = args_[0]->evaluate(env);
      return (val == reverse_num(val)) ? 1 : 0;
    }
    case BuiltinFn::Pow:
      return ipow(args_[0]->evaluate(env), args_[1]->evaluate(env));
    case BuiltinFn::Isqrt:
      return isqrt(args_[0]->evaluate(env));
    case BuiltinFn::Reverse:
      return reverse_num(args_[0]->evaluate(env));
    default:
      __builtin_unreachable();
  }
}

void FunctionCallNode::resolve_slots(Environment& env) {
  for (auto& arg : args) {
    if (arg) arg->resolve_slots(env);
  }
}

void BlockStmtNode::add_statement(std::unique_ptr<StmtNode> stmt) {
  if (stmt) {
    statements_.push_back(stmt.get());
    owned_statements_.push_back(std::move(stmt));
  }
}
void BlockStmtNode::execute(Environment& env) {
  const auto* raw = statements_.data();
  const size_t sz = statements_.size();
  for (size_t i = 0; i < sz; ++i) {
    raw[i]->execute(env);
  }
}
void BlockStmtNode::resolve_slots(Environment& env) {
  for (const auto& stmt : statements_) {
    stmt->resolve_slots(env);
  }
}

VarDeclNode::VarDeclNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), init(std::move(i)), slot_(-1) {
  if (name == "OUTPUT_ARRAY_LENGTH" || name == "TMP_ARRAY_LENGTH" ||
      is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot declare to reserved variable: " + name);
  }
}
void VarDeclNode::resolve_slots(Environment& env) {
  if (init) init->resolve_slots(env);
  if (slot_ < 0) {
    slot_ = env.get_or_create_slot(name);
  }
}
void VarDeclNode::execute(Environment& env) {
  env.set_slot_value(slot_, init->evaluate(env));
}

AssignStmtNode::AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e)
    : name(std::move(n)), expr(std::move(e)), slot_(-1) {
  if (name == "OUTPUT_ARRAY_LENGTH" || name == "TMP_ARRAY_LENGTH" ||
      is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot assign to reserved variable: " + name);
  }
}
void AssignStmtNode::resolve_slots(Environment& env) {
  if (expr) expr->resolve_slots(env);
  if (slot_ < 0) {
    slot_ = env.find_slot(name);
    if (slot_ < 0) {
      throw std::runtime_error("Assignment to undeclared: " + name);
    }
  }
}
void AssignStmtNode::execute(Environment& env) {
  env.set_slot_value(slot_, expr->evaluate(env));
}

IndexAssignStmtNode::IndexAssignStmtNode(std::string n,
                                         std::unique_ptr<ExprNode> i,
                                         std::unique_ptr<ExprNode> v)
    : name(std::move(n)), idx_expr(std::move(i)), val_expr(std::move(v)) {
  if (name != "tmp" && name != "output") {
    throw std::runtime_error("Assignment to invalid array: " + name);
  }
  is_tmp_ = (name == "tmp");
}
void IndexAssignStmtNode::resolve_slots(Environment& env) {
  if (idx_expr) idx_expr->resolve_slots(env);
  if (val_expr) val_expr->resolve_slots(env);
}
void IndexAssignStmtNode::execute(Environment& env) {
  int idx = idx_expr->evaluate(env);
  int val = val_expr->evaluate(env);
  if (is_tmp_) {
    env.set_tmp_array(idx, val);
  } else {
    env.set_output_array(idx, val);
  }
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> e)
    : expr(std::move(e)) {}
void PrintStmtNode::resolve_slots(Environment& env) {
  if (expr) expr->resolve_slots(env);
}
void PrintStmtNode::execute(Environment& env) {
  std::cout << "printed: " << expr->evaluate(env) << "\n";
};

IfStmtNode::IfStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t,
                       std::unique_ptr<StmtNode> e)
    : cond(std::move(c)),
      then_branch(std::move(t)),
      else_branch(std::move(e)) {}
void IfStmtNode::resolve_slots(Environment& env) {
  if (cond) cond->resolve_slots(env);
  if (then_branch) then_branch->resolve_slots(env);
  if (else_branch) else_branch->resolve_slots(env);
}
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
void ForStmtNode::resolve_slots(Environment& env) {
  if (init) init->resolve_slots(env);
  if (condition) condition->resolve_slots(env);
  if (body) body->resolve_slots(env);
  if (update) update->resolve_slots(env);
}
void ForStmtNode::execute(Environment& env) {
  if (init) init->execute(env);
  if (update) {
    while (condition->evaluate(env) != 0) {
      body->execute(env);
      update->execute(env);
    }
  } else {
    while (condition->evaluate(env) != 0) {
      body->execute(env);
    }
  }
}

WhileStmtNode::WhileStmtNode(std::unique_ptr<ExprNode> cond,
                             std::unique_ptr<StmtNode> b)
    : condition(std::move(cond)), body(std::move(b)) {}
void WhileStmtNode::resolve_slots(Environment& env) {
  if (condition) condition->resolve_slots(env);
  if (body) body->resolve_slots(env);
}
void WhileStmtNode::execute(Environment& env) {
  while (condition->evaluate(env) != 0) {
    body->execute(env);
  }
};