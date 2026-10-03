#include "ast.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "builtin.h"

Environment::Environment() {
  slots_.reserve(64);
  get_or_create_slot("OUTPUT_ARRAY_LENGTH");
  get_or_create_slot("TMP_ARRAY_LENGTH");
  output_ = output_array.data();
  tmp_ = tmp_array.data();
  raw_slots_ = slots_.data();
}

Environment::Environment(Environment&& o) noexcept
    : var_to_slot_(std::move(o.var_to_slot_)),
      slots_(std::move(o.slots_)),
      output_array(std::move(o.output_array)),
      tmp_array(std::move(o.tmp_array)),
      written_output_indices(std::move(o.written_output_indices)),
      written_tmp_indices(std::move(o.written_tmp_indices)),
      output_(output_array.data()),
      tmp_(tmp_array.data()),
      raw_slots_(slots_.data()) {}

Environment& Environment::operator=(Environment&& o) noexcept {
  if (this != &o) {
    var_to_slot_ = std::move(o.var_to_slot_);
    slots_ = std::move(o.slots_);
    output_array = std::move(o.output_array);
    tmp_array = std::move(o.tmp_array);
    written_output_indices = std::move(o.written_output_indices);
    written_tmp_indices = std::move(o.written_tmp_indices);
    output_ = output_array.data();
    tmp_ = tmp_array.data();
    raw_slots_ = slots_.data();
  }
  return *this;
}

int Environment::get_or_create_slot(const std::string& id) {
  auto it = var_to_slot_.find(id);
  if (it != var_to_slot_.end()) {
    return it->second;
  }
  int slot = static_cast<int>(slots_.size());
  var_to_slot_.emplace(id, slot);
  slots_.push_back(0);
  raw_slots_ = slots_.data();
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
  tmp_ = tmp_array.data();
  written_tmp_indices.clear();
  slots_[1] = len;
}

void Environment::initialise_output_array(int len, int val) {
  output_array.assign(len, val);
  output_ = output_array.data();
  written_output_indices.clear();
  slots_[0] = len;
}

namespace {
bool is_reserved_variable_name(std::string_view name) {
  if ((name[0] == 'a' || name[0] == 'd') && name.size() > 1) {
    return std::all_of(name.begin() + 1, name.end(),
                       [](unsigned char c) { return std::isdigit(c); });
  }
  return name[0] == 'c' && name.size() == 2 &&
         std::isdigit(static_cast<unsigned char>(name[1]));
}
}  // namespace

LiteralNode::LiteralNode(int val) : value(val) {}
int LiteralNode::evaluate(Environment&) { return value; }
std::unique_ptr<ExprNode> LiteralNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment&) {
  return self;
}
std::unique_ptr<ExprNode> LiteralNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  return self;
}

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

std::unique_ptr<ExprNode> VariableNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment& env) {
  if (slot_ < 0) {
    slot_ = env.find_slot(name);
    if (slot_ < 0) {
      slot_ = env.get_or_create_slot(name);
    }
  }
  return self;
}

std::unique_ptr<ExprNode> VariableNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  return self;
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

int UnaryVarOpNode::evaluate(Environment& env) {
  int val = env.get_slot_value(slot_);
  switch (op) {
    case UnaryOp::Plus:
      return +val;
    case UnaryOp::Minus:
      return -val;
    case UnaryOp::LogicalNot:
      return val == 0 ? 1 : 0;
  }
  __builtin_unreachable();
}

std::unique_ptr<ExprNode> UnaryVarOpNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment&) {
  return self;
}

std::unique_ptr<ExprNode> UnaryVarOpNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  return self;
}

UnaryOpNode::UnaryOpNode(UnaryOp o, std::unique_ptr<ExprNode> expr)
    : op(o), operand(std::move(expr)), operand_(operand.get()) {}

UnaryOpNode::UnaryOpNode(const std::string& o, std::unique_ptr<ExprNode> expr)
    : op(string_to_unary_op(o)),
      operand(std::move(expr)),
      operand_(operand.get()) {}

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

std::unique_ptr<ExprNode> UnaryOpNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment& env) {
  operand = resolve_node(std::move(operand), env);
  operand_ = operand.get();
  if (operand_->is_literal()) {
    int val = operand_->get_literal_value();
    int res = 0;
    switch (op) {
      case UnaryOp::Plus:
        res = +val;
        break;
      case UnaryOp::Minus:
        res = -val;
        break;
      case UnaryOp::LogicalNot:
        res = (val == 0 ? 1 : 0);
        break;
    }
    return std::make_unique<LiteralNode>(res);
  }
  if (operand_->get_slot() >= 0) {
    return std::make_unique<UnaryVarOpNode>(op, operand_->get_slot());
  }
  return self;
}

std::unique_ptr<ExprNode> UnaryOpNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  operand = fold_node(std::move(operand));
  operand_ = operand.get();

  if (auto* lit = dynamic_cast<LiteralNode*>(operand_)) {
    int val = lit->get_value();
    int res = 0;
    switch (op) {
      case UnaryOp::Plus:
        res = +val;
        break;
      case UnaryOp::Minus:
        res = -val;
        break;
      case UnaryOp::LogicalNot:
        res = (val == 0 ? 1 : 0);
        break;
    }
    return std::make_unique<LiteralNode>(res);
  }
  return self;
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
  if (op == BinaryOp::LogicalAnd) {
    if (left_->evaluate(env) == 0) return 0;
    return right_->evaluate(env) != 0 ? 1 : 0;
  }
  if (op == BinaryOp::LogicalOr) {
    if (left_->evaluate(env) != 0) return 1;
    return right_->evaluate(env) != 0 ? 1 : 0;
  }
  return eval_binary_op(op, left_->evaluate(env), right_->evaluate(env));
}

std::unique_ptr<ExprNode> BinaryOpNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment& env) {
  operand1 = resolve_node(std::move(operand1), env);
  operand2 = resolve_node(std::move(operand2), env);
  left_ = operand1.get();
  right_ = operand2.get();

  bool left_is_slot = (left_->get_slot() >= 0);
  bool right_is_slot = (right_->get_slot() >= 0);
  bool left_is_lit = left_->is_literal();
  bool right_is_lit = right_->is_literal();

  if (left_is_slot && right_is_slot) {
    return make_binary_node<BinaryVarVarOpNode>(op, left_->get_slot(),
                                                right_->get_slot());
  }
  if (left_is_slot && right_is_lit) {
    return make_binary_node<BinaryVarLitOpNode>(
        op, left_->get_slot(), right_->get_literal_value());
  }
  if (left_is_lit && right_is_slot) {
    return make_binary_node<BinaryLitVarOpNode>(
        op, left_->get_literal_value(), right_->get_slot());
  }
  if (left_is_lit && right_is_lit) {
    return std::make_unique<LiteralNode>(
        eval_binary_op(op, left_->get_literal_value(),
                       right_->get_literal_value()));
  }
  BuiltinFn fn;
  int fn_slot = -1;
  if (left_->is_fn_call_var1(fn, fn_slot)) {
    if (right_is_slot) {
      auto node = make_fn_call1_node<FnCall1VarVarOpNode>(op, fn, fn_slot, right_->get_slot());
      if (node) return node;
    }
    if (right_is_lit) {
      auto node = make_fn_call1_node<FnCall1VarLitOpNode>(op, fn, fn_slot, right_->get_literal_value());
      if (node) return node;
    }
  }
  if (left_is_slot) {
    return make_binary_node<BinaryVarExprOpNode>(op, left_->get_slot(),
                                                 std::move(operand2));
  }
  if (right_is_slot) {
    return make_binary_node<BinaryExprVarOpNode>(op, std::move(operand1),
                                                 right_->get_slot());
  }
  if (right_is_lit) {
    return make_binary_node<BinaryExprLitOpNode>(
        op, std::move(operand1), right_->get_literal_value());
  }
  if (left_is_lit) {
    return make_binary_node<BinaryLitExprOpNode>(
        op, left_->get_literal_value(), std::move(operand2));
  }
  return make_binary_node<BinaryExprExprOpNode>(op, std::move(operand1),
                                               std::move(operand2));
}

std::unique_ptr<ExprNode> BinaryOpNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  operand1 = fold_node(std::move(operand1));
  operand2 = fold_node(std::move(operand2));
  left_ = operand1.get();
  right_ = operand2.get();

  auto* left_lit = dynamic_cast<LiteralNode*>(left_);
  auto* right_lit = dynamic_cast<LiteralNode*>(right_);
  if (left_lit && right_lit) {
    return std::make_unique<LiteralNode>(
        eval_binary_op(op, left_lit->get_value(), right_lit->get_value()));
  }
  return self;
}

IndexReadNode::IndexReadNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), expr(std::move(i)), raw_expr_(expr.get()) {
  if (name != "tmp" && name != "output") {
    throw std::runtime_error("Read from invalid array: " + name);
  }
  is_tmp_ = (name == "tmp");
}

int IndexReadNode::evaluate(Environment& env) {
  int idx = expr_is_lit_ ? expr_lit_val_
                         : (expr_slot_ >= 0 ? env.get_slot_value(expr_slot_)
                                            : raw_expr_->evaluate(env));
  return is_tmp_ ? env.get_tmp_array(idx) : env.get_output_array(idx);
}

std::unique_ptr<ExprNode> IndexReadNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment& env) {
  expr = resolve_node(std::move(expr), env);
  raw_expr_ = expr.get();
  expr_slot_ = raw_expr_ ? raw_expr_->get_slot() : -1;
  expr_is_lit_ = raw_expr_ ? raw_expr_->is_literal() : false;
  expr_lit_val_ = expr_is_lit_ ? raw_expr_->get_literal_value() : 0;
  return self;
}

std::unique_ptr<ExprNode> IndexReadNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  expr = fold_node(std::move(expr));
  raw_expr_ = expr.get();
  expr_slot_ = raw_expr_ ? raw_expr_->get_slot() : -1;
  expr_is_lit_ = raw_expr_ ? raw_expr_->is_literal() : false;
  expr_lit_val_ = expr_is_lit_ ? raw_expr_->get_literal_value() : 0;
  return self;
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
      return dsum(get_arg(0, env));
    case BuiltinFn::GetNthDigit:
      return get_nth_digit(get_arg(0, env),
                           get_arg(1, env) - 1);
    case BuiltinFn::IsPrime:
      return is_prime(get_arg(0, env)) ? 1 : 0;
    case BuiltinFn::IsPalindrome: {
      int val = get_arg(0, env);
      return (val == reverse_num(val)) ? 1 : 0;
    }
    case BuiltinFn::Pow:
      return ipow(get_arg(0, env), get_arg(1, env));
    case BuiltinFn::Isqrt:
      return isqrt(get_arg(0, env));
    case BuiltinFn::Reverse:
      return reverse_num(get_arg(0, env));
    default:
      __builtin_unreachable();
  }
}

std::unique_ptr<ExprNode> FunctionCallNode::resolve_slots(
    std::unique_ptr<ExprNode> self, Environment& env) {
  for (size_t i = 0; i < arity_; ++i) {
    if (args[i]) {
      args[i] = resolve_node(std::move(args[i]), env);
      args_[i] = args[i].get();
      if (args_[i]) {
        arg_slots_[i] = args_[i]->get_slot();
        arg_is_lit_[i] = args_[i]->is_literal();
        arg_lit_vals_[i] = arg_is_lit_[i] ? args_[i]->get_literal_value() : 0;
      }
    }
  }

  if (arity_ == 1 && arg_slots_[0] >= 0) {
    switch (builtin_id_) {
      case BuiltinFn::IsPrime:
        return std::make_unique<FunctionCallVar1Node<BuiltinFn::IsPrime>>(arg_slots_[0]);
      case BuiltinFn::Dsum:
        return std::make_unique<FunctionCallVar1Node<BuiltinFn::Dsum>>(arg_slots_[0]);
      case BuiltinFn::IsPalindrome:
        return std::make_unique<FunctionCallVar1Node<BuiltinFn::IsPalindrome>>(arg_slots_[0]);
      case BuiltinFn::Isqrt:
        return std::make_unique<FunctionCallVar1Node<BuiltinFn::Isqrt>>(arg_slots_[0]);
      case BuiltinFn::Reverse:
        return std::make_unique<FunctionCallVar1Node<BuiltinFn::Reverse>>(arg_slots_[0]);
      default:
        break;
    }
  }

  return self;
}

std::unique_ptr<ExprNode> FunctionCallNode::fold_constants(
    std::unique_ptr<ExprNode> self) {
  bool all_literals = (arity_ > 0);
  for (size_t i = 0; i < arity_; ++i) {
    if (args[i]) {
      args[i] = fold_node(std::move(args[i]));
      args_[i] = args[i].get();
      if (args_[i]) {
        arg_slots_[i] = args_[i]->get_slot();
        arg_is_lit_[i] = args_[i]->is_literal();
        arg_lit_vals_[i] = arg_is_lit_[i] ? args_[i]->get_literal_value() : 0;
        if (!arg_is_lit_[i]) {
          all_literals = false;
        }
      } else {
        all_literals = false;
      }
    } else {
      all_literals = false;
    }
  }

  if (all_literals) {
    try {
      switch (builtin_id_) {
        case BuiltinFn::Dsum:
          return std::make_unique<LiteralNode>(dsum(arg_lit_vals_[0]));
        case BuiltinFn::GetNthDigit:
          if (arg_lit_vals_[1] >= 1) {
            return std::make_unique<LiteralNode>(
                get_nth_digit(arg_lit_vals_[0], arg_lit_vals_[1] - 1));
          }
          break;
        case BuiltinFn::IsPrime:
          return std::make_unique<LiteralNode>(
              is_prime(arg_lit_vals_[0]) ? 1 : 0);
        case BuiltinFn::IsPalindrome:
          return std::make_unique<LiteralNode>(
              (arg_lit_vals_[0] == reverse_num(arg_lit_vals_[0])) ? 1 : 0);
        case BuiltinFn::Pow:
          if (arg_lit_vals_[1] >= 0) {
            return std::make_unique<LiteralNode>(
                ipow(arg_lit_vals_[0], arg_lit_vals_[1]));
          }
          break;
        case BuiltinFn::Isqrt:
          if (arg_lit_vals_[0] >= 0) {
            return std::make_unique<LiteralNode>(isqrt(arg_lit_vals_[0]));
          }
          break;
        case BuiltinFn::Reverse:
          return std::make_unique<LiteralNode>(reverse_num(arg_lit_vals_[0]));
        default:
          break;
      }
    } catch (...) {
      // In case of any arithmetic error, keep the node unfolded
    }
  }

  return self;
}

void BlockStmtNode::add_statement(std::unique_ptr<StmtNode> stmt) {
  if (stmt) {
    statements_.push_back(stmt.get());
    owned_statements_.push_back(std::move(stmt));
    raw_statements_ = statements_.data();
    statements_size_ = statements_.size();
  }
}
void BlockStmtNode::execute(Environment& env) {
  if (statements_size_ == 2) {
    raw_statements_[0]->execute(env);
    raw_statements_[1]->execute(env);
    return;
  }
  if (statements_size_ == 3) {
    raw_statements_[0]->execute(env);
    raw_statements_[1]->execute(env);
    raw_statements_[2]->execute(env);
    return;
  }
  if (statements_size_ == 4) {
    raw_statements_[0]->execute(env);
    raw_statements_[1]->execute(env);
    raw_statements_[2]->execute(env);
    raw_statements_[3]->execute(env);
    return;
  }
  for (size_t i = 0; i < statements_size_; ++i) {
    raw_statements_[i]->execute(env);
  }
}
std::unique_ptr<StmtNode> BlockStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  std::vector<std::unique_ptr<StmtNode>> new_statements;
  new_statements.reserve(owned_statements_.size());
  for (auto& stmt : owned_statements_) {
    if (!stmt) continue;
    auto resolved = resolve_stmt(std::move(stmt), env);
    if (!resolved) continue;
    if (auto* child_block = dynamic_cast<BlockStmtNode*>(resolved.get())) {
      for (auto& child_stmt : child_block->get_owned_statements()) {
        if (child_stmt) {
          new_statements.push_back(std::move(child_stmt));
        }
      }
    } else {
      new_statements.push_back(std::move(resolved));
    }
  }
  owned_statements_ = std::move(new_statements);

  if (owned_statements_.size() == 1) {
    return std::move(owned_statements_[0]);
  }

  statements_.clear();
  for (const auto& s : owned_statements_) {
    statements_.push_back(s.get());
  }
  raw_statements_ = statements_.data();
  statements_size_ = statements_.size();
  return self;
}
void BlockStmtNode::fold_constants() {
  for (auto& stmt : owned_statements_) {
    if (stmt) stmt->fold_constants();
  }
  statements_.clear();
  for (const auto& stmt : owned_statements_) {
    statements_.push_back(stmt.get());
  }
  raw_statements_ = statements_.data();
  statements_size_ = statements_.size();
}

VarDeclNode::VarDeclNode(std::string n, std::unique_ptr<ExprNode> i)
    : name(std::move(n)), init(std::move(i)), raw_init_(init.get()), slot_(-1) {
  if (name == "OUTPUT_ARRAY_LENGTH" || name == "TMP_ARRAY_LENGTH" ||
      is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot declare to reserved variable: " + name);
  }
}
std::unique_ptr<StmtNode> VarDeclNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (slot_ < 0) {
    slot_ = env.get_or_create_slot(name);
  }
  if (init) {
    init = resolve_node(std::move(init), env);
    raw_init_ = init.get();
    init_slot_ = raw_init_ ? raw_init_->get_slot() : -1;
    init_is_lit_ = raw_init_ ? raw_init_->is_literal() : false;
    init_lit_val_ = init_is_lit_ ? raw_init_->get_literal_value() : 0;
  }
  return self;
}
void VarDeclNode::execute(Environment& env) {
  if (init_is_lit_) {
    env.set_slot_value(slot_, init_lit_val_);
  } else if (init_slot_ >= 0) {
    env.set_slot_value(slot_, env.get_slot_value(init_slot_));
  } else if (raw_init_) {
    env.set_slot_value(slot_, raw_init_->evaluate(env));
  }
}
void VarDeclNode::fold_constants() {
  if (init) {
    init = fold_node(std::move(init));
    raw_init_ = init.get();
    init_slot_ = raw_init_ ? raw_init_->get_slot() : -1;
    init_is_lit_ = raw_init_ ? raw_init_->is_literal() : false;
    init_lit_val_ = init_is_lit_ ? raw_init_->get_literal_value() : 0;
  }
}

AssignStmtNode::AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e)
    : name(std::move(n)), slot_(-1), expr(std::move(e)), raw_expr_(expr.get()) {
  if (name == "OUTPUT_ARRAY_LENGTH" || name == "TMP_ARRAY_LENGTH" ||
      is_reserved_variable_name(name)) {
    throw std::runtime_error("Cannot assign to reserved variable: " + name);
  }
}
std::unique_ptr<StmtNode> AssignStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (expr) {
    expr = resolve_node(std::move(expr), env);
    raw_expr_ = expr.get();
    expr_slot_ = raw_expr_ ? raw_expr_->get_slot() : -1;
    expr_is_lit_ = raw_expr_ ? raw_expr_->is_literal() : false;
    expr_lit_val_ = expr_is_lit_ ? raw_expr_->get_literal_value() : 0;
  }
  if (slot_ < 0) {
    slot_ = env.find_slot(name);
    if (slot_ < 0) {
      slot_ = env.get_or_create_slot(name);
    }
  }

  if (expr_is_lit_) {
    return std::make_unique<AssignLitStmtNode>(std::move(name), slot_, expr_lit_val_);
  }
  if (expr_slot_ >= 0) {
    return std::make_unique<AssignVarStmtNode>(std::move(name), slot_, expr_slot_);
  }
  if (raw_expr_) {
    BinaryOp op;
    int left_slot = -1, lit_val = 0;
    if (raw_expr_->is_var_lit_op(op, left_slot, lit_val) && left_slot == slot_) {
      auto node = make_assign_self_lit_node(op, std::move(name), slot_, lit_val);
      if (node) return node;
    }
  }
  return std::make_unique<AssignExprStmtNode>(std::move(name), slot_, std::move(expr));
}
void AssignStmtNode::execute(Environment& env) {
  if (slot_ < 0) [[unlikely]] {
    slot_ = env.get_or_create_slot(name);
  }
  if (expr_is_lit_) {
    env.set_slot_value(slot_, expr_lit_val_);
  } else if (expr_slot_ >= 0) {
    env.set_slot_value(slot_, env.get_slot_value(expr_slot_));
  } else if (raw_expr_) {
    env.set_slot_value(slot_, raw_expr_->evaluate(env));
  }
}
void AssignStmtNode::fold_constants() {
  if (expr) {
    expr = fold_node(std::move(expr));
    raw_expr_ = expr.get();
    expr_slot_ = raw_expr_ ? raw_expr_->get_slot() : -1;
    expr_is_lit_ = raw_expr_ ? raw_expr_->is_literal() : false;
    expr_lit_val_ = expr_is_lit_ ? raw_expr_->get_literal_value() : 0;
  }
}

TmpIndexAssignStmtNode::TmpIndexAssignStmtNode(std::unique_ptr<ExprNode> i,
                                               std::unique_ptr<ExprNode> v)
    : idx_expr(std::move(i)),
      val_expr(std::move(v)),
      raw_idx_expr_(idx_expr.get()),
      raw_val_expr_(val_expr.get()) {}

std::unique_ptr<StmtNode> TmpIndexAssignStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  idx_expr = resolve_node(std::move(idx_expr), env);
  val_expr = resolve_node(std::move(val_expr), env);
  raw_idx_expr_ = idx_expr.get();
  raw_val_expr_ = val_expr.get();
  idx_slot_ = raw_idx_expr_ ? raw_idx_expr_->get_slot() : -1;
  val_slot_ = raw_val_expr_ ? raw_val_expr_->get_slot() : -1;
  idx_is_lit_ = raw_idx_expr_ ? raw_idx_expr_->is_literal() : false;
  idx_lit_val_ = idx_is_lit_ ? raw_idx_expr_->get_literal_value() : 0;
  val_is_lit_ = raw_val_expr_ ? raw_val_expr_->is_literal() : false;
  val_lit_val_ = val_is_lit_ ? raw_val_expr_->get_literal_value() : 0;
  return self;
}

void TmpIndexAssignStmtNode::execute(Environment& env) {
  int idx = idx_is_lit_ ? idx_lit_val_
                        : (idx_slot_ >= 0 ? env.get_slot_value(idx_slot_)
                                          : raw_idx_expr_->evaluate(env));
  int val = val_is_lit_ ? val_lit_val_
                        : (val_slot_ >= 0 ? env.get_slot_value(val_slot_)
                                          : raw_val_expr_->evaluate(env));
  env.set_tmp_array(idx, val);
}
void TmpIndexAssignStmtNode::fold_constants() {
  if (idx_expr) {
    idx_expr = fold_node(std::move(idx_expr));
    raw_idx_expr_ = idx_expr.get();
  }
  if (val_expr) {
    val_expr = fold_node(std::move(val_expr));
    raw_val_expr_ = val_expr.get();
  }
  idx_slot_ = raw_idx_expr_ ? raw_idx_expr_->get_slot() : -1;
  val_slot_ = raw_val_expr_ ? raw_val_expr_->get_slot() : -1;
  idx_is_lit_ = raw_idx_expr_ ? raw_idx_expr_->is_literal() : false;
  idx_lit_val_ = idx_is_lit_ ? raw_idx_expr_->get_literal_value() : 0;
  val_is_lit_ = raw_val_expr_ ? raw_val_expr_->is_literal() : false;
  val_lit_val_ = val_is_lit_ ? raw_val_expr_->get_literal_value() : 0;
}

OutputIndexAssignStmtNode::OutputIndexAssignStmtNode(
    std::unique_ptr<ExprNode> i, std::unique_ptr<ExprNode> v)
    : idx_expr(std::move(i)),
      val_expr(std::move(v)),
      raw_idx_expr_(idx_expr.get()),
      raw_val_expr_(val_expr.get()) {}

std::unique_ptr<StmtNode> OutputIndexAssignStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  idx_expr = resolve_node(std::move(idx_expr), env);
  val_expr = resolve_node(std::move(val_expr), env);
  raw_idx_expr_ = idx_expr.get();
  raw_val_expr_ = val_expr.get();
  idx_slot_ = raw_idx_expr_ ? raw_idx_expr_->get_slot() : -1;
  val_slot_ = raw_val_expr_ ? raw_val_expr_->get_slot() : -1;
  idx_is_lit_ = raw_idx_expr_ ? raw_idx_expr_->is_literal() : false;
  idx_lit_val_ = idx_is_lit_ ? raw_idx_expr_->get_literal_value() : 0;
  val_is_lit_ = raw_val_expr_ ? raw_val_expr_->is_literal() : false;
  val_lit_val_ = val_is_lit_ ? raw_val_expr_->get_literal_value() : 0;
  return self;
}

void OutputIndexAssignStmtNode::execute(Environment& env) {
  int idx = idx_is_lit_ ? idx_lit_val_
                        : (idx_slot_ >= 0 ? env.get_slot_value(idx_slot_)
                                          : raw_idx_expr_->evaluate(env));
  int val = val_is_lit_ ? val_lit_val_
                        : (val_slot_ >= 0 ? env.get_slot_value(val_slot_)
                                          : raw_val_expr_->evaluate(env));
  env.set_output_array(idx, val);
}
void OutputIndexAssignStmtNode::fold_constants() {
  if (idx_expr) {
    idx_expr = fold_node(std::move(idx_expr));
    raw_idx_expr_ = idx_expr.get();
  }
  if (val_expr) {
    val_expr = fold_node(std::move(val_expr));
    raw_val_expr_ = val_expr.get();
  }
  idx_slot_ = raw_idx_expr_ ? raw_idx_expr_->get_slot() : -1;
  val_slot_ = raw_val_expr_ ? raw_val_expr_->get_slot() : -1;
  idx_is_lit_ = raw_idx_expr_ ? raw_idx_expr_->is_literal() : false;
  idx_lit_val_ = idx_is_lit_ ? raw_idx_expr_->get_literal_value() : 0;
  val_is_lit_ = raw_val_expr_ ? raw_val_expr_->is_literal() : false;
  val_lit_val_ = val_is_lit_ ? raw_val_expr_->get_literal_value() : 0;
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> e)
    : expr(std::move(e)), raw_expr_(expr.get()) {}
std::unique_ptr<StmtNode> PrintStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (expr) {
    expr = resolve_node(std::move(expr), env);
    raw_expr_ = expr.get();
  }
  return self;
}
void PrintStmtNode::execute(Environment& env) {
  if (raw_expr_) {
    std::cout << "printed: " << raw_expr_->evaluate(env) << "\n";
  }
}
void PrintStmtNode::fold_constants() {
  if (expr) {
    expr = fold_node(std::move(expr));
    raw_expr_ = expr.get();
  }
}

IfStmtNode::IfStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t,
                       std::unique_ptr<StmtNode> e)
    : cond(std::move(c)),
      then_branch(std::move(t)),
      else_branch(std::move(e)),
      raw_cond_(cond.get()),
      raw_then_(then_branch.get()),
      raw_else_(else_branch.get()) {}
std::unique_ptr<StmtNode> IfStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  cond = resolve_node(std::move(cond), env);
  raw_cond_ = cond.get();
  if (then_branch) then_branch = resolve_stmt(std::move(then_branch), env);
  if (else_branch) else_branch = resolve_stmt(std::move(else_branch), env);
  raw_then_ = then_branch.get();
  raw_else_ = else_branch.get();
  if (!else_branch) {
    return std::make_unique<IfThenStmtNode>(std::move(cond), std::move(then_branch));
  }
  return self;
}
void IfStmtNode::execute(Environment& env) {
  if (raw_cond_->evaluate(env) != 0) {
    if (raw_then_) [[likely]] {
      raw_then_->execute(env);
    }
  } else if (raw_else_) {
    raw_else_->execute(env);
  }
}
void IfStmtNode::fold_constants() {
  if (cond) {
    cond = fold_node(std::move(cond));
    raw_cond_ = cond.get();
  }
  if (then_branch) {
    then_branch->fold_constants();
    raw_then_ = then_branch.get();
  }
  if (else_branch) {
    else_branch->fold_constants();
    raw_else_ = else_branch.get();
  }
}

IfThenStmtNode::IfThenStmtNode(std::unique_ptr<ExprNode> c,
                               std::unique_ptr<StmtNode> t)
    : cond(std::move(c)),
      then_branch(std::move(t)),
      raw_cond_(cond.get()),
      raw_then_(then_branch.get()) {}

void IfThenStmtNode::execute(Environment& env) {
  if (raw_cond_->evaluate(env) != 0) {
    if (raw_then_) [[likely]] {
      raw_then_->execute(env);
    }
  }
}

std::unique_ptr<StmtNode> IfThenStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  cond = resolve_node(std::move(cond), env);
  raw_cond_ = cond.get();
  if (then_branch) then_branch = resolve_stmt(std::move(then_branch), env);
  raw_then_ = then_branch.get();
  return self;
}

void IfThenStmtNode::fold_constants() {
  if (cond) {
    cond = fold_node(std::move(cond));
    raw_cond_ = cond.get();
  }
  if (then_branch) {
    then_branch->fold_constants();
    raw_then_ = then_branch.get();
  }
}

ForStmtNode::ForStmtNode(std::unique_ptr<StmtNode> i,
                         std::unique_ptr<ExprNode> c,
                         std::unique_ptr<StmtNode> u,
                         std::unique_ptr<StmtNode> b)
    : init(std::move(i)),
      condition(std::move(c)),
      update(std::move(u)),
      body(std::move(b)),
      raw_init_(init.get()),
      raw_cond_(condition.get()),
      raw_update_(update.get()),
      raw_body_(body.get()) {}

std::unique_ptr<StmtNode> ForStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (init) init = resolve_stmt(std::move(init), env);
  condition = resolve_node(std::move(condition), env);
  if (update) update = resolve_stmt(std::move(update), env);
  if (body) body = resolve_stmt(std::move(body), env);
  raw_init_ = init.get();
  raw_cond_ = condition.get();
  raw_update_ = update.get();
  raw_body_ = body.get();

  // Pattern detection for canonical counting loop:
  int var_slot = -1;
  int start_val = 0;
  bool init_matched = false;
  if (auto* decl = dynamic_cast<VarDeclNode*>(raw_init_)) {
    if (decl->is_init_lit()) {
      var_slot = decl->get_slot();
      start_val = decl->get_init_lit_val();
      init_matched = true;
    }
  } else if (auto* assign = dynamic_cast<AssignStmtNode*>(raw_init_)) {
    if (assign->is_expr_lit()) {
      var_slot = assign->get_slot();
      start_val = assign->get_expr_lit_val();
      init_matched = true;
    }
  }

  bool update_matched = false;
  if (init_matched && var_slot >= 0) {
    if (auto* assign = dynamic_cast<AssignStmtNode*>(raw_update_)) {
      if (assign->get_slot() == var_slot) {
        BinaryOp u_op;
        int u_step = -1;
        if (assign->is_self_op_lit(u_op, u_step)) {
          if (u_op == BinaryOp::Add && u_step == 1) {
            update_matched = true;
          }
        } else if (assign->get_expr()) {
          int u_left = -1;
          if (assign->get_expr()->is_var_lit_op(u_op, u_left, u_step)) {
            if (u_op == BinaryOp::Add && u_left == var_slot && u_step == 1) {
              update_matched = true;
            }
          }
        }
      }
    }
  }

  if (update_matched && raw_cond_) {
    BinaryOp c_op;
    int c_left = -1, c_right = -1;
    if (raw_cond_->is_var_var_op(c_op, c_left, c_right)) {
      if (c_left == var_slot && c_op == BinaryOp::Less) {
        if (auto* if_then = dynamic_cast<IfThenStmtNode*>(raw_body_)) {
          auto if_cond = if_then->extract_cond();
          auto if_then_branch = if_then->extract_then();
          return std::make_unique<ForCountIfVarLimitStmtNode>(
              var_slot, start_val, c_right, std::move(if_cond), std::move(if_then_branch));
        }
        return std::make_unique<ForCountVarLimitStmtNode>(
            var_slot, start_val, c_right, std::move(body));
      }
    } else if (raw_cond_->is_var_lit_op(c_op, c_left, c_right)) {
      if (c_left == var_slot && (c_op == BinaryOp::Less || c_op == BinaryOp::LessEqual)) {
        int limit = (c_op == BinaryOp::Less) ? c_right : (c_right + 1);
        if (auto* if_then = dynamic_cast<IfThenStmtNode*>(raw_body_)) {
          auto if_cond = if_then->extract_cond();
          auto if_then_branch = if_then->extract_then();
          return std::make_unique<ForCountIfLitLimitStmtNode>(
              var_slot, start_val, limit, std::move(if_cond), std::move(if_then_branch));
        }
        return std::make_unique<ForCountLitLimitStmtNode>(
            var_slot, start_val, limit, std::move(body));
      }
    }
  }

  return self;
}

void ForStmtNode::execute(Environment& env) {
  if (raw_init_) raw_init_->execute(env);
  if (raw_update_) {
    while (raw_cond_->evaluate(env) != 0) {
      raw_body_->execute(env);
      raw_update_->execute(env);
    }
  } else {
    while (raw_cond_->evaluate(env) != 0) {
      raw_body_->execute(env);
    }
  }
}
void ForStmtNode::fold_constants() {
  if (init) {
    init->fold_constants();
    raw_init_ = init.get();
  }
  if (condition) {
    condition = fold_node(std::move(condition));
    raw_cond_ = condition.get();
  }
  if (update) {
    update->fold_constants();
    raw_update_ = update.get();
  }
  if (body) {
    body->fold_constants();
    raw_body_ = body.get();
  }
}

ForCountVarLimitStmtNode::ForCountVarLimitStmtNode(
    int var_slot, int start_val, int limit_slot, std::unique_ptr<StmtNode> body)
    : var_slot_(var_slot),
      start_val_(start_val),
      limit_slot_(limit_slot),
      body_(std::move(body)),
      raw_body_(body_.get()) {}

void ForCountVarLimitStmtNode::execute(Environment& env) {
  int limit = env.get_slot_value(limit_slot_);
  int i = start_val_;
  for (; i < limit; ++i) {
    env.set_slot_value(var_slot_, i);
    raw_body_->execute(env);
  }
  env.set_slot_value(var_slot_, i);
}

std::unique_ptr<StmtNode> ForCountVarLimitStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  body_ = resolve_stmt(std::move(body_), env);
  raw_body_ = body_.get();
  if (auto* if_then = dynamic_cast<IfThenStmtNode*>(raw_body_)) {
    auto if_cond = if_then->extract_cond();
    auto if_then_branch = if_then->extract_then();
    return std::make_unique<ForCountIfVarLimitStmtNode>(
        var_slot_, start_val_, limit_slot_, std::move(if_cond), std::move(if_then_branch));
  }
  return self;
}

void ForCountVarLimitStmtNode::fold_constants() {
  if (body_) body_->fold_constants();
  raw_body_ = body_.get();
}

ForCountLitLimitStmtNode::ForCountLitLimitStmtNode(
    int var_slot, int start_val, int limit_val, std::unique_ptr<StmtNode> body)
    : var_slot_(var_slot),
      start_val_(start_val),
      limit_val_(limit_val),
      body_(std::move(body)),
      raw_body_(body_.get()) {}

void ForCountLitLimitStmtNode::execute(Environment& env) {
  int i = start_val_;
  for (; i < limit_val_; ++i) {
    env.set_slot_value(var_slot_, i);
    raw_body_->execute(env);
  }
  env.set_slot_value(var_slot_, i);
}

std::unique_ptr<StmtNode> ForCountLitLimitStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  body_ = resolve_stmt(std::move(body_), env);
  raw_body_ = body_.get();
  if (auto* if_then = dynamic_cast<IfThenStmtNode*>(raw_body_)) {
    auto if_cond = if_then->extract_cond();
    auto if_then_branch = if_then->extract_then();
    return std::make_unique<ForCountIfLitLimitStmtNode>(
        var_slot_, start_val_, limit_val_, std::move(if_cond), std::move(if_then_branch));
  }
  return self;
}

void ForCountLitLimitStmtNode::fold_constants() {
  if (body_) body_->fold_constants();
  raw_body_ = body_.get();
}

ForCountIfVarLimitStmtNode::ForCountIfVarLimitStmtNode(
    int var_slot, int start_val, int limit_slot,
    std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> then_branch)
    : var_slot_(var_slot),
      start_val_(start_val),
      limit_slot_(limit_slot),
      cond_(std::move(cond)),
      then_branch_(std::move(then_branch)),
      raw_cond_(cond_.get()),
      raw_then_(then_branch_.get()) {}

void ForCountIfVarLimitStmtNode::execute(Environment& env) {
  int limit = env.get_slot_value(limit_slot_);
  int i = start_val_;
  if (raw_then_) [[likely]] {
    for (; i < limit; ++i) {
      env.set_slot_value(var_slot_, i);
      if (raw_cond_->evaluate(env) != 0) {
        raw_then_->execute(env);
      }
    }
  } else {
    for (; i < limit; ++i) {
      env.set_slot_value(var_slot_, i);
      (void)raw_cond_->evaluate(env);
    }
  }
  env.set_slot_value(var_slot_, i);
}

std::unique_ptr<StmtNode> ForCountIfVarLimitStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (cond_) {
    cond_ = resolve_node(std::move(cond_), env);
    raw_cond_ = cond_.get();
  }
  if (then_branch_) {
    then_branch_ = resolve_stmt(std::move(then_branch_), env);
    raw_then_ = then_branch_.get();
  }
  return self;
}

void ForCountIfVarLimitStmtNode::fold_constants() {
  if (cond_) {
    cond_ = fold_node(std::move(cond_));
    raw_cond_ = cond_.get();
  }
  if (then_branch_) {
    then_branch_->fold_constants();
    raw_then_ = then_branch_.get();
  }
}

ForCountIfLitLimitStmtNode::ForCountIfLitLimitStmtNode(
    int var_slot, int start_val, int limit_val,
    std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> then_branch)
    : var_slot_(var_slot),
      start_val_(start_val),
      limit_val_(limit_val),
      cond_(std::move(cond)),
      then_branch_(std::move(then_branch)),
      raw_cond_(cond_.get()),
      raw_then_(then_branch_.get()) {}

void ForCountIfLitLimitStmtNode::execute(Environment& env) {
  int i = start_val_;
  if (raw_then_) [[likely]] {
    for (; i < limit_val_; ++i) {
      env.set_slot_value(var_slot_, i);
      if (raw_cond_->evaluate(env) != 0) {
        raw_then_->execute(env);
      }
    }
  } else {
    for (; i < limit_val_; ++i) {
      env.set_slot_value(var_slot_, i);
      (void)raw_cond_->evaluate(env);
    }
  }
  env.set_slot_value(var_slot_, i);
}

std::unique_ptr<StmtNode> ForCountIfLitLimitStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  if (cond_) {
    cond_ = resolve_node(std::move(cond_), env);
    raw_cond_ = cond_.get();
  }
  if (then_branch_) {
    then_branch_ = resolve_stmt(std::move(then_branch_), env);
    raw_then_ = then_branch_.get();
  }
  return self;
}

void ForCountIfLitLimitStmtNode::fold_constants() {
  if (cond_) {
    cond_ = fold_node(std::move(cond_));
    raw_cond_ = cond_.get();
  }
  if (then_branch_) {
    then_branch_->fold_constants();
    raw_then_ = then_branch_.get();
  }
}

WhileStmtNode::WhileStmtNode(std::unique_ptr<ExprNode> cond,
                             std::unique_ptr<StmtNode> b)
    : condition(std::move(cond)),
      body(std::move(b)),
      raw_cond_(condition.get()),
      raw_body_(body.get()) {
  init_body_cache();
}

void WhileStmtNode::init_body_cache() {
  raw_stmt0_ = nullptr;
  raw_stmt1_ = nullptr;
  if (auto* blk = dynamic_cast<BlockStmtNode*>(raw_body_)) {
    if (blk->get_statements_size() == 2) {
      raw_stmt0_ = blk->get_raw_statements()[0];
      raw_stmt1_ = blk->get_raw_statements()[1];
    }
  }
}

std::unique_ptr<StmtNode> WhileStmtNode::resolve_slots(
    std::unique_ptr<StmtNode> self, Environment& env) {
  condition = resolve_node(std::move(condition), env);
  if (body) body = resolve_stmt(std::move(body), env);
  raw_cond_ = condition.get();
  raw_body_ = body.get();
  init_body_cache();

  if (!raw_cond_ || !raw_body_) return self;

  if (raw_cond_->is_literal()) {
    if (raw_cond_->get_literal_value() == 0) {
      return std::make_unique<BlockStmtNode>();
    }
    return self;
  }

  if (raw_cond_->get_slot() >= 0) {
    return std::make_unique<WhileVarCondStmtNode>(raw_cond_->get_slot(),
                                                  std::move(body));
  }

  BinaryOp op;
  int left_slot = -1, right_slot = -1, lit_val = 0;
  ExprNode* left_expr = nullptr;

  if (raw_cond_->is_var_lit_op(op, left_slot, lit_val)) {
    return make_while_node<WhileVarLitCondStmtNode>(op, left_slot, lit_val,
                                                    std::move(body));
  }

  if (raw_cond_->is_lit_var_op(op, lit_val, right_slot)) {
    return make_while_node<WhileLitVarCondStmtNode>(op, lit_val, right_slot,
                                                    std::move(body));
  }

  if (raw_cond_->is_var_var_op(op, left_slot, right_slot)) {
    return make_while_node<WhileVarVarCondStmtNode>(op, left_slot, right_slot,
                                                    std::move(body));
  }

  if (raw_cond_->is_expr_var_op(op, left_expr, right_slot)) {
    BinaryOp inner_op;
    int inner_left = -1, inner_right = -1;
    if (left_expr && left_expr->is_var_var_op(inner_op, inner_left, inner_right) &&
        inner_op == BinaryOp::Multiply && inner_left == inner_right) {
      return make_while_node<WhileVarSquareVarLimitStmtNode>(
          op, inner_left, right_slot, std::move(body));
    }
  }

  if (raw_cond_->is_expr_lit_op(op, left_expr, lit_val)) {
    BinaryOp inner_op;
    int inner_left = -1, inner_right = -1;
    if (left_expr && left_expr->is_var_var_op(inner_op, inner_left, inner_right)) {
      if (inner_op == BinaryOp::Multiply && inner_left == inner_right) {
        return make_while_node<WhileVarSquareLitLimitStmtNode>(
            op, inner_left, lit_val, std::move(body));
      }
      if (inner_op == BinaryOp::Modulo && op == BinaryOp::Equal && lit_val == 0) {
        return std::make_unique<WhileVarModVarZeroStmtNode>(
            inner_left, inner_right, std::move(body));
      }
    }
  }

  return self;
}

void WhileStmtNode::execute(Environment& env) {
  if (raw_stmt1_) {
    while (raw_cond_->evaluate(env) != 0) {
      raw_stmt0_->execute(env);
      raw_stmt1_->execute(env);
    }
  } else {
    while (raw_cond_->evaluate(env) != 0) {
      raw_body_->execute(env);
    }
  }
}

void WhileStmtNode::fold_constants() {
  if (condition) {
    condition = fold_node(std::move(condition));
    raw_cond_ = condition.get();
  }
  if (body) {
    body->fold_constants();
    raw_body_ = body.get();
    init_body_cache();
  }
}