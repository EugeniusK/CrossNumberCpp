#ifndef AST_H
#define AST_H

#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "builtin.h"

class Environment {
 public:
  Environment();
  Environment(Environment&& o) noexcept;
  Environment& operator=(Environment&& o) noexcept;
  Environment(const Environment&) = delete;
  Environment& operator=(const Environment&) = delete;

  // Slot-based indexing
  int get_or_create_slot(const std::string& id);
  int find_slot(const std::string& id) const;

  inline int get_slot_value(int slot) const noexcept {
    return raw_slots_[slot];
  }

  inline void set_slot_value(int slot, int val) noexcept {
    raw_slots_[slot] = val;
  }

  // Name-based API
  bool has_var(const std::string& id) const;
  int get_var(const std::string& id) const;
  void set_var(const std::string& id, int val);

  std::unordered_map<std::string, int>::iterator find_var(
      const std::string& id);
  std::unordered_map<std::string, int>::iterator var_end();

  void initialise_output_array(int len, int val = 0);
  inline void reset_output_array() noexcept {
    for (int idx : written_output_indices) {
      output_[idx] = 0;
    }
    written_output_indices.clear();
  }
  inline int get_output_array(int idx) const noexcept { return output_[idx]; }
  inline void set_output_array(int idx, int val) noexcept {
    if (output_[idx] == 0 && val != 0) {
      written_output_indices.push_back(idx);
    }
    output_[idx] = val;
  }

  void initialise_tmp_array(int len, int val = 0);
  inline void reset_tmp_array() noexcept {
    for (int idx : written_tmp_indices) {
      tmp_[idx] = 0;
    }
    written_tmp_indices.clear();
  }
  inline int get_tmp_array(int idx) const noexcept { return tmp_[idx]; }
  inline void set_tmp_array(int idx, int val) noexcept {
    if (tmp_[idx] == 0 && val != 0) {
      written_tmp_indices.push_back(idx);
    }
    tmp_[idx] = val;
  }
  const std::vector<int>& get_written_output_indices() const noexcept {
    return written_output_indices;
  }

 private:
  std::unordered_map<std::string, int> var_to_slot_;
  std::vector<int> slots_;
  std::vector<int> output_array;
  std::vector<int> tmp_array;
  std::vector<int> written_output_indices;
  std::vector<int> written_tmp_indices;
  int* output_ = nullptr;
  int* tmp_ = nullptr;
  int* raw_slots_ = nullptr;
};

enum class BinaryOp : uint8_t;

class ExprNode {
 public:
  virtual ~ExprNode() = default;
  virtual int evaluate(Environment& env) = 0;
  virtual std::unique_ptr<ExprNode> resolve_slots(
      std::unique_ptr<ExprNode> self, Environment& env) = 0;
  virtual std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) = 0;
  virtual int get_slot() const noexcept { return -1; }
  virtual bool is_literal() const noexcept { return false; }
  virtual int get_literal_value() const noexcept { return 0; }
  virtual bool is_var_var_op(BinaryOp&, int&, int&) const noexcept {
    return false;
  }
  virtual bool is_var_lit_op(BinaryOp&, int&, int&) const noexcept {
    return false;
  }
  virtual bool is_lit_var_op(BinaryOp&, int&, int&) const noexcept {
    return false;
  }
  virtual bool is_var_expr_op(BinaryOp&, int&, ExprNode*&) const noexcept {
    return false;
  }
  virtual bool is_expr_var_op(BinaryOp&, ExprNode*&, int&) const noexcept {
    return false;
  }
  virtual bool is_expr_lit_op(BinaryOp&, ExprNode*&, int&) const noexcept {
    return false;
  }
  virtual bool is_fn_call_var1(BuiltinFn&, int&) const noexcept {
    return false;
  }
};

inline std::unique_ptr<ExprNode> fold_node(std::unique_ptr<ExprNode> node) {
  if (!node) return nullptr;
  auto* raw = node.get();
  return raw->fold_constants(std::move(node));
}

inline std::unique_ptr<ExprNode> resolve_node(std::unique_ptr<ExprNode> node,
                                              Environment& env) {
  if (!node) return nullptr;
  auto* raw = node.get();
  return raw->resolve_slots(std::move(node), env);
}

inline int eval_operand(int slot, ExprNode* expr, Environment& env) noexcept {
  return (slot >= 0) ? env.get_slot_value(slot) : expr->evaluate(env);
}

class LiteralNode : public ExprNode {
 public:
  LiteralNode(int val);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

  int get_value() const noexcept { return value; }
  bool is_literal() const noexcept override { return true; }
  int get_literal_value() const noexcept override { return value; }

 private:
  int value;
};

// "x"
class VariableNode : public ExprNode {
 public:
  VariableNode(std::string n);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

  int get_slot() const noexcept override { return slot_; }
  const std::string& get_name() const noexcept { return name; }

 private:
  std::string name;
  int slot_ = -1;
};

enum class UnaryOp { Plus, Minus, LogicalNot };

enum class BinaryOp : uint8_t {
  Add = 0,
  Subtract = 1,
  Multiply = 2,
  Divide = 3,
  Modulo = 4,
  Equal = 5,
  NotEqual = 6,
  Less = 7,
  LessEqual = 8,
  Greater = 9,
  GreaterEqual = 10,
  LogicalAnd = 11,
  LogicalOr = 12
};

BinaryOp string_to_binary_op(std::string_view o);
UnaryOp string_to_unary_op(std::string_view o);

inline int eval_binary_op(BinaryOp op, int left, int right) noexcept {
  switch (op) {
    case BinaryOp::Add:
      return left + right;
    case BinaryOp::Subtract:
      return left - right;
    case BinaryOp::Multiply:
      return left * right;
    case BinaryOp::Divide:
      return right != 0 ? left / right : 0;
    case BinaryOp::Modulo:
      return right != 0 ? left % right : 0;
    case BinaryOp::Equal:
      return left == right ? 1 : 0;
    case BinaryOp::NotEqual:
      return left != right ? 1 : 0;
    case BinaryOp::Less:
      return left < right ? 1 : 0;
    case BinaryOp::LessEqual:
      return left <= right ? 1 : 0;
    case BinaryOp::Greater:
      return left > right ? 1 : 0;
    case BinaryOp::GreaterEqual:
      return left >= right ? 1 : 0;
    case BinaryOp::LogicalAnd:
      return (left != 0 && right != 0) ? 1 : 0;
    case BinaryOp::LogicalOr:
      return (left != 0 || right != 0) ? 1 : 0;
  }
  __builtin_unreachable();
}

template <BinaryOp Op>
inline constexpr int eval_binary_op_static(int left, int right) noexcept {
  if constexpr (Op == BinaryOp::Add)
    return left + right;
  else if constexpr (Op == BinaryOp::Subtract)
    return left - right;
  else if constexpr (Op == BinaryOp::Multiply)
    return left * right;
  else if constexpr (Op == BinaryOp::Divide)
    return right != 0 ? left / right : 0;
  else if constexpr (Op == BinaryOp::Modulo)
    return right != 0 ? left % right : 0;
  else if constexpr (Op == BinaryOp::Equal)
    return left == right ? 1 : 0;
  else if constexpr (Op == BinaryOp::NotEqual)
    return left != right ? 1 : 0;
  else if constexpr (Op == BinaryOp::Less)
    return left < right ? 1 : 0;
  else if constexpr (Op == BinaryOp::LessEqual)
    return left <= right ? 1 : 0;
  else if constexpr (Op == BinaryOp::Greater)
    return left > right ? 1 : 0;
  else if constexpr (Op == BinaryOp::GreaterEqual)
    return left >= right ? 1 : 0;
  else if constexpr (Op == BinaryOp::LogicalAnd)
    return (left != 0 && right != 0) ? 1 : 0;
  else if constexpr (Op == BinaryOp::LogicalOr)
    return (left != 0 || right != 0) ? 1 : 0;
}

template <template <BinaryOp> class NodeTemplate, typename... Args>
inline std::unique_ptr<ExprNode> make_binary_node(BinaryOp op, Args&&... args) {
  switch (op) {
    case BinaryOp::Add:
      return std::make_unique<NodeTemplate<BinaryOp::Add>>(
          std::forward<Args>(args)...);
    case BinaryOp::Subtract:
      return std::make_unique<NodeTemplate<BinaryOp::Subtract>>(
          std::forward<Args>(args)...);
    case BinaryOp::Multiply:
      return std::make_unique<NodeTemplate<BinaryOp::Multiply>>(
          std::forward<Args>(args)...);
    case BinaryOp::Divide:
      return std::make_unique<NodeTemplate<BinaryOp::Divide>>(
          std::forward<Args>(args)...);
    case BinaryOp::Modulo:
      return std::make_unique<NodeTemplate<BinaryOp::Modulo>>(
          std::forward<Args>(args)...);
    case BinaryOp::Equal:
      return std::make_unique<NodeTemplate<BinaryOp::Equal>>(
          std::forward<Args>(args)...);
    case BinaryOp::NotEqual:
      return std::make_unique<NodeTemplate<BinaryOp::NotEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::Less:
      return std::make_unique<NodeTemplate<BinaryOp::Less>>(
          std::forward<Args>(args)...);
    case BinaryOp::LessEqual:
      return std::make_unique<NodeTemplate<BinaryOp::LessEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::Greater:
      return std::make_unique<NodeTemplate<BinaryOp::Greater>>(
          std::forward<Args>(args)...);
    case BinaryOp::GreaterEqual:
      return std::make_unique<NodeTemplate<BinaryOp::GreaterEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::LogicalAnd:
      return std::make_unique<NodeTemplate<BinaryOp::LogicalAnd>>(
          std::forward<Args>(args)...);
    case BinaryOp::LogicalOr:
      return std::make_unique<NodeTemplate<BinaryOp::LogicalOr>>(
          std::forward<Args>(args)...);
  }
  __builtin_unreachable();
}

class UnaryVarOpNode : public ExprNode {
 public:
  UnaryVarOpNode(UnaryOp o, int slot) : op(o), slot_(slot) {}
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

 private:
  UnaryOp op;
  int slot_;
};

template <BinaryOp Op>
class BinaryVarVarOpNode : public ExprNode {
 public:
  BinaryVarVarOpNode(int left_slot, int right_slot)
      : left_slot_(left_slot), right_slot_(right_slot) {}

  int evaluate(Environment& env) override {
    return eval_binary_op_static<Op>(env.get_slot_value(left_slot_),
                                     env.get_slot_value(right_slot_));
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

  bool is_var_var_op(BinaryOp& op, int& left_slot,
                     int& right_slot) const noexcept override {
    op = Op;
    left_slot = left_slot_;
    right_slot = right_slot_;
    return true;
  }

 private:
  int left_slot_;
  int right_slot_;
};

template <BinaryOp Op>
class BinaryVarLitOpNode : public ExprNode {
 public:
  BinaryVarLitOpNode(int left_slot, int right_val)
      : left_slot_(left_slot), right_val_(right_val) {}

  int evaluate(Environment& env) override {
    return eval_binary_op_static<Op>(env.get_slot_value(left_slot_),
                                     right_val_);
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

  bool is_var_lit_op(BinaryOp& op, int& left_slot,
                     int& right_val) const noexcept override {
    op = Op;
    left_slot = left_slot_;
    right_val = right_val_;
    return true;
  }

 private:
  int left_slot_;
  int right_val_;
};

template <BinaryOp Op>
class BinaryLitVarOpNode : public ExprNode {
 public:
  BinaryLitVarOpNode(int left_val, int right_slot)
      : left_val_(left_val), right_slot_(right_slot) {}

  int evaluate(Environment& env) override {
    return eval_binary_op_static<Op>(left_val_,
                                     env.get_slot_value(right_slot_));
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

  bool is_lit_var_op(BinaryOp& op, int& left_val,
                     int& right_slot) const noexcept override {
    op = Op;
    left_val = left_val_;
    right_slot = right_slot_;
    return true;
  }

 private:
  int left_val_;
  int right_slot_;
};

template <BinaryOp Op>
class BinaryVarExprOpNode : public ExprNode {
 public:
  BinaryVarExprOpNode(int left_slot, std::unique_ptr<ExprNode> right)
      : left_slot_(left_slot),
        right_expr_(std::move(right)),
        raw_right_(right_expr_.get()) {}

  int evaluate(Environment& env) override {
    if constexpr (Op == BinaryOp::LogicalAnd) {
      if (env.get_slot_value(left_slot_) == 0) return 0;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else if constexpr (Op == BinaryOp::LogicalOr) {
      if (env.get_slot_value(left_slot_) != 0) return 1;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else {
      return eval_binary_op_static<Op>(env.get_slot_value(left_slot_),
                                       raw_right_->evaluate(env));
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override {
    if (right_expr_) {
      right_expr_ = resolve_node(std::move(right_expr_), env);
      raw_right_ = right_expr_.get();
    }
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    if (right_expr_) {
      right_expr_ = fold_node(std::move(right_expr_));
      raw_right_ = right_expr_.get();
    }
    return self;
  }

  bool is_var_expr_op(BinaryOp& op, int& left_slot,
                      ExprNode*& right_expr) const noexcept override {
    op = Op;
    left_slot = left_slot_;
    right_expr = raw_right_;
    return true;
  }

 private:
  int left_slot_;
  std::unique_ptr<ExprNode> right_expr_;
  ExprNode* raw_right_ = nullptr;
};

template <BinaryOp Op>
class BinaryExprVarOpNode : public ExprNode {
 public:
  BinaryExprVarOpNode(std::unique_ptr<ExprNode> left, int right_slot)
      : left_expr_(std::move(left)),
        raw_left_(left_expr_.get()),
        right_slot_(right_slot) {}

  int evaluate(Environment& env) override {
    if constexpr (Op == BinaryOp::LogicalAnd) {
      if (raw_left_->evaluate(env) == 0) return 0;
      return env.get_slot_value(right_slot_) != 0 ? 1 : 0;
    } else if constexpr (Op == BinaryOp::LogicalOr) {
      if (raw_left_->evaluate(env) != 0) return 1;
      return env.get_slot_value(right_slot_) != 0 ? 1 : 0;
    } else {
      return eval_binary_op_static<Op>(raw_left_->evaluate(env),
                                       env.get_slot_value(right_slot_));
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override {
    if (left_expr_) {
      left_expr_ = resolve_node(std::move(left_expr_), env);
      raw_left_ = left_expr_.get();
    }
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    if (left_expr_) {
      left_expr_ = fold_node(std::move(left_expr_));
      raw_left_ = left_expr_.get();
    }
    return self;
  }

  bool is_expr_var_op(BinaryOp& op, ExprNode*& left_expr,
                      int& right_slot) const noexcept override {
    op = Op;
    left_expr = raw_left_;
    right_slot = right_slot_;
    return true;
  }

 private:
  std::unique_ptr<ExprNode> left_expr_;
  ExprNode* raw_left_ = nullptr;
  int right_slot_;
};

template <BinaryOp Op>
class BinaryExprLitOpNode : public ExprNode {
 public:
  BinaryExprLitOpNode(std::unique_ptr<ExprNode> left, int right_val)
      : left_expr_(std::move(left)),
        raw_left_(left_expr_.get()),
        right_val_(right_val) {}

  int evaluate(Environment& env) override {
    if constexpr (Op == BinaryOp::LogicalAnd) {
      if (raw_left_->evaluate(env) == 0) return 0;
      return right_val_ != 0 ? 1 : 0;
    } else if constexpr (Op == BinaryOp::LogicalOr) {
      if (raw_left_->evaluate(env) != 0) return 1;
      return right_val_ != 0 ? 1 : 0;
    } else {
      return eval_binary_op_static<Op>(raw_left_->evaluate(env), right_val_);
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override {
    if (left_expr_) {
      left_expr_ = resolve_node(std::move(left_expr_), env);
      raw_left_ = left_expr_.get();
    }
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    if (left_expr_) {
      left_expr_ = fold_node(std::move(left_expr_));
      raw_left_ = left_expr_.get();
    }
    return self;
  }

  bool is_expr_lit_op(BinaryOp& op, ExprNode*& left_expr,
                      int& right_val) const noexcept override {
    op = Op;
    left_expr = raw_left_;
    right_val = right_val_;
    return true;
  }

 private:
  std::unique_ptr<ExprNode> left_expr_;
  ExprNode* raw_left_ = nullptr;
  int right_val_;
};

template <BinaryOp Op>
class BinaryLitExprOpNode : public ExprNode {
 public:
  BinaryLitExprOpNode(int left_val, std::unique_ptr<ExprNode> right)
      : left_val_(left_val),
        right_expr_(std::move(right)),
        raw_right_(right_expr_.get()) {}

  int evaluate(Environment& env) override {
    if constexpr (Op == BinaryOp::LogicalAnd) {
      if (left_val_ == 0) return 0;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else if constexpr (Op == BinaryOp::LogicalOr) {
      if (left_val_ != 0) return 1;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else {
      return eval_binary_op_static<Op>(left_val_, raw_right_->evaluate(env));
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override {
    if (right_expr_) {
      right_expr_ = resolve_node(std::move(right_expr_), env);
      raw_right_ = right_expr_.get();
    }
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    if (right_expr_) {
      right_expr_ = fold_node(std::move(right_expr_));
      raw_right_ = right_expr_.get();
    }
    return self;
  }

 private:
  int left_val_;
  std::unique_ptr<ExprNode> right_expr_;
  ExprNode* raw_right_ = nullptr;
};

template <BinaryOp Op>
class BinaryExprExprOpNode : public ExprNode {
 public:
  BinaryExprExprOpNode(std::unique_ptr<ExprNode> left,
                       std::unique_ptr<ExprNode> right)
      : left_expr_(std::move(left)),
        right_expr_(std::move(right)),
        raw_left_(left_expr_.get()),
        raw_right_(right_expr_.get()) {}

  int evaluate(Environment& env) override {
    if constexpr (Op == BinaryOp::LogicalAnd) {
      if (raw_left_->evaluate(env) == 0) return 0;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else if constexpr (Op == BinaryOp::LogicalOr) {
      if (raw_left_->evaluate(env) != 0) return 1;
      return raw_right_->evaluate(env) != 0 ? 1 : 0;
    } else {
      return eval_binary_op_static<Op>(raw_left_->evaluate(env),
                                       raw_right_->evaluate(env));
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override {
    if (left_expr_) {
      left_expr_ = resolve_node(std::move(left_expr_), env);
      raw_left_ = left_expr_.get();
    }
    if (right_expr_) {
      right_expr_ = resolve_node(std::move(right_expr_), env);
      raw_right_ = right_expr_.get();
    }
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    if (left_expr_) {
      left_expr_ = fold_node(std::move(left_expr_));
      raw_left_ = left_expr_.get();
    }
    if (right_expr_) {
      right_expr_ = fold_node(std::move(right_expr_));
      raw_right_ = right_expr_.get();
    }
    return self;
  }

 private:
  std::unique_ptr<ExprNode> left_expr_;
  std::unique_ptr<ExprNode> right_expr_;
  ExprNode* raw_left_ = nullptr;
  ExprNode* raw_right_ = nullptr;
};

class UnaryOpNode : public ExprNode {
 public:
  UnaryOpNode(UnaryOp o, std::unique_ptr<ExprNode> expr);
  UnaryOpNode(const std::string& o, std::unique_ptr<ExprNode> expr);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

 private:
  UnaryOp op;
  std::unique_ptr<ExprNode> operand;
  ExprNode* operand_ = nullptr;
};

class BinaryOpNode : public ExprNode {
 public:
  BinaryOpNode(BinaryOp o, std::unique_ptr<ExprNode> expr1,
               std::unique_ptr<ExprNode> expr2);
  BinaryOpNode(const std::string& o, std::unique_ptr<ExprNode> expr1,
               std::unique_ptr<ExprNode> expr2);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

 private:
  BinaryOp op;
  std::unique_ptr<ExprNode> operand1;
  std::unique_ptr<ExprNode> operand2;
  ExprNode* left_ = nullptr;
  ExprNode* right_ = nullptr;
};

// tmp[x] or output[x]
class IndexReadNode : public ExprNode {
 public:
  IndexReadNode(std::string n, std::unique_ptr<ExprNode> i);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

 private:
  std::string name;
  std::unique_ptr<ExprNode> expr;
  ExprNode* raw_expr_ = nullptr;
  int expr_slot_ = -1;
  bool expr_is_lit_ = false;
  int expr_lit_val_ = 0;
  bool is_tmp_ = false;
};

// func(x,y)
class FunctionCallNode : public ExprNode {
 public:
  FunctionCallNode(std::string n, std::vector<std::unique_ptr<ExprNode>> a);
  int evaluate(Environment& env) override;
  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment& env) override;
  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override;

  inline int get_arg(size_t i, Environment& env) const noexcept {
    if (arg_slots_[i] >= 0) return env.get_slot_value(arg_slots_[i]);
    if (arg_is_lit_[i]) return arg_lit_vals_[i];
    return args_[i]->evaluate(env);
  }

 private:
  std::string name;
  std::vector<std::unique_ptr<ExprNode>> args;
  BuiltinFn builtin_id_;
  size_t arity_ = 0;
  ExprNode* args_[4] = {nullptr, nullptr, nullptr, nullptr};
  int arg_slots_[4] = {-1, -1, -1, -1};
  bool arg_is_lit_[4] = {false, false, false, false};
  int arg_lit_vals_[4] = {0, 0, 0, 0};
};

template <BuiltinFn Fn>
class FunctionCallVar1Node : public ExprNode {
 public:
  explicit FunctionCallVar1Node(int slot) : slot_(slot) {}

  int evaluate(Environment& env) override {
    int val = env.get_slot_value(slot_);
    if constexpr (Fn == BuiltinFn::IsPrime) {
      return is_prime(val) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Dsum) {
      return dsum(val);
    } else if constexpr (Fn == BuiltinFn::IsPalindrome) {
      return (val == reverse_num(val)) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Isqrt) {
      return isqrt(val);
    } else if constexpr (Fn == BuiltinFn::Reverse) {
      return reverse_num(val);
    } else {
      return 0;
    }
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

  bool is_fn_call_var1(BuiltinFn& fn, int& slot) const noexcept override {
    fn = Fn;
    slot = slot_;
    return true;
  }

 private:
  int slot_;
};

template <BuiltinFn Fn, BinaryOp Op>
class FnCall1VarVarOpNode : public ExprNode {
 public:
  FnCall1VarVarOpNode(int fn_slot, int right_slot)
      : fn_slot_(fn_slot), right_slot_(right_slot) {}

  int evaluate(Environment& env) override {
    int val = env.get_slot_value(fn_slot_);
    int fn_res = 0;
    if constexpr (Fn == BuiltinFn::IsPrime) {
      fn_res = is_prime(val) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Dsum) {
      fn_res = dsum(val);
    } else if constexpr (Fn == BuiltinFn::IsPalindrome) {
      fn_res = (val == reverse_num(val)) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Isqrt) {
      fn_res = isqrt(val);
    } else if constexpr (Fn == BuiltinFn::Reverse) {
      fn_res = reverse_num(val);
    }
    return eval_binary_op_static<Op>(fn_res, env.get_slot_value(right_slot_));
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

 private:
  int fn_slot_;
  int right_slot_;
};

template <BuiltinFn Fn, BinaryOp Op>
class FnCall1VarLitOpNode : public ExprNode {
 public:
  FnCall1VarLitOpNode(int fn_slot, int lit_val)
      : fn_slot_(fn_slot), lit_val_(lit_val) {}

  int evaluate(Environment& env) override {
    int val = env.get_slot_value(fn_slot_);
    int fn_res = 0;
    if constexpr (Fn == BuiltinFn::IsPrime) {
      fn_res = is_prime(val) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Dsum) {
      fn_res = dsum(val);
    } else if constexpr (Fn == BuiltinFn::IsPalindrome) {
      fn_res = (val == reverse_num(val)) ? 1 : 0;
    } else if constexpr (Fn == BuiltinFn::Isqrt) {
      fn_res = isqrt(val);
    } else if constexpr (Fn == BuiltinFn::Reverse) {
      fn_res = reverse_num(val);
    }
    return eval_binary_op_static<Op>(fn_res, lit_val_);
  }

  std::unique_ptr<ExprNode> resolve_slots(std::unique_ptr<ExprNode> self,
                                          Environment&) override {
    return self;
  }

  std::unique_ptr<ExprNode> fold_constants(
      std::unique_ptr<ExprNode> self) override {
    return self;
  }

 private:
  int fn_slot_;
  int lit_val_;
};

template <template <BuiltinFn, BinaryOp> class NodeTemplate, BinaryOp Op>
inline std::unique_ptr<ExprNode> make_fn_call1_node_op(BuiltinFn fn, int slot,
                                                       int second) {
  switch (fn) {
    case BuiltinFn::IsPrime:
      return std::make_unique<NodeTemplate<BuiltinFn::IsPrime, Op>>(slot,
                                                                    second);
    case BuiltinFn::Dsum:
      return std::make_unique<NodeTemplate<BuiltinFn::Dsum, Op>>(slot, second);
    case BuiltinFn::IsPalindrome:
      return std::make_unique<NodeTemplate<BuiltinFn::IsPalindrome, Op>>(
          slot, second);
    case BuiltinFn::Isqrt:
      return std::make_unique<NodeTemplate<BuiltinFn::Isqrt, Op>>(slot, second);
    case BuiltinFn::Reverse:
      return std::make_unique<NodeTemplate<BuiltinFn::Reverse, Op>>(slot,
                                                                    second);
    default:
      return nullptr;
  }
}

template <template <BuiltinFn, BinaryOp> class NodeTemplate>
inline std::unique_ptr<ExprNode> make_fn_call1_node(BinaryOp op, BuiltinFn fn,
                                                    int slot, int second) {
  switch (op) {
    case BinaryOp::Equal:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Equal>(fn, slot,
                                                                  second);
    case BinaryOp::NotEqual:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::NotEqual>(fn, slot,
                                                                     second);
    case BinaryOp::Less:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Less>(fn, slot,
                                                                 second);
    case BinaryOp::LessEqual:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::LessEqual>(fn, slot,
                                                                      second);
    case BinaryOp::Greater:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Greater>(fn, slot,
                                                                    second);
    case BinaryOp::GreaterEqual:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::GreaterEqual>(
          fn, slot, second);
    case BinaryOp::Add:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Add>(fn, slot,
                                                                second);
    case BinaryOp::Subtract:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Subtract>(fn, slot,
                                                                     second);
    case BinaryOp::Multiply:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Multiply>(fn, slot,
                                                                     second);
    case BinaryOp::Divide:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Divide>(fn, slot,
                                                                   second);
    case BinaryOp::Modulo:
      return make_fn_call1_node_op<NodeTemplate, BinaryOp::Modulo>(fn, slot,
                                                                   second);
    default:
      return nullptr;
  }
}

class StmtNode {
 public:
  virtual ~StmtNode() = default;
  virtual void execute(Environment& env) = 0;
  virtual std::unique_ptr<StmtNode> resolve_slots(
      std::unique_ptr<StmtNode> self, Environment& env) = 0;
  virtual void fold_constants() = 0;
};

inline std::unique_ptr<StmtNode> resolve_stmt(std::unique_ptr<StmtNode> stmt,
                                              Environment& env) {
  if (!stmt) return nullptr;
  auto* raw = stmt.get();
  return raw->resolve_slots(std::move(stmt), env);
}

class BlockStmtNode : public StmtNode {
 public:
  void add_statement(std::unique_ptr<StmtNode> stmt);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;
  size_t size() const noexcept { return owned_statements_.size(); }
  std::vector<std::unique_ptr<StmtNode>>& get_owned_statements() noexcept {
    return owned_statements_;
  }
  StmtNode* const* get_raw_statements() const noexcept {
    return raw_statements_;
  }
  size_t get_statements_size() const noexcept { return statements_size_; }

 private:
  std::vector<std::unique_ptr<StmtNode>> owned_statements_;
  std::vector<StmtNode*> statements_;
  StmtNode* const* raw_statements_ = nullptr;
  size_t statements_size_ = 0;
};

// let x = 5;
class VarDeclNode : public StmtNode {
 public:
  VarDeclNode(std::string n, std::unique_ptr<ExprNode> i);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

  int get_slot() const noexcept { return slot_; }
  bool is_init_lit() const noexcept { return init_is_lit_; }
  int get_init_lit_val() const noexcept { return init_lit_val_; }

 private:
  std::string name;
  std::unique_ptr<ExprNode> init;
  ExprNode* raw_init_ = nullptr;
  int slot_ = -1;
  int init_slot_ = -1;
  bool init_is_lit_ = false;
  int init_lit_val_ = 0;
};

// x = 5;
class AssignStmtNode : public StmtNode {
 public:
  AssignStmtNode(std::string n, std::unique_ptr<ExprNode> e);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

  int get_slot() const noexcept { return slot_; }
  bool is_expr_lit() const noexcept { return expr_is_lit_; }
  int get_expr_lit_val() const noexcept { return expr_lit_val_; }
  ExprNode* get_expr() const noexcept { return raw_expr_; }
  bool is_self_op_lit(BinaryOp& op, int& step) const noexcept {
    if (is_self_op_) {
      op = self_op_;
      step = expr_lit_val_;
      return true;
    }
    return false;
  }

 protected:
  AssignStmtNode(std::string n, int slot, int lit_val)
      : name(std::move(n)),
        slot_(slot),
        expr_is_lit_(true),
        expr_lit_val_(lit_val) {}
  AssignStmtNode(std::string n, int slot, int src_slot, bool)
      : name(std::move(n)), slot_(slot), expr_slot_(src_slot) {}
  AssignStmtNode(std::string n, int slot, int lit_val, BinaryOp op)
      : name(std::move(n)),
        slot_(slot),
        expr_lit_val_(lit_val),
        is_self_op_(true),
        self_op_(op) {}
  AssignStmtNode(std::string n, int slot, std::unique_ptr<ExprNode> e)
      : name(std::move(n)),
        slot_(slot),
        expr(std::move(e)),
        raw_expr_(expr.get()) {}

  std::string name;
  int slot_ = -1;
  int expr_slot_ = -1;
  bool expr_is_lit_ = false;
  int expr_lit_val_ = 0;
  bool is_self_op_ = false;
  BinaryOp self_op_ = BinaryOp::Add;
  std::unique_ptr<ExprNode> expr;
  ExprNode* raw_expr_ = nullptr;
};

class AssignLitStmtNode : public AssignStmtNode {
 public:
  AssignLitStmtNode(std::string n, int slot, int lit_val)
      : AssignStmtNode(std::move(n), slot, lit_val) {}

  void execute(Environment& env) override {
    env.set_slot_value(slot_, expr_lit_val_);
  }
};

class AssignVarStmtNode : public AssignStmtNode {
 public:
  AssignVarStmtNode(std::string n, int slot, int src_slot)
      : AssignStmtNode(std::move(n), slot, src_slot, true) {}

  void execute(Environment& env) override {
    env.set_slot_value(slot_, env.get_slot_value(expr_slot_));
  }
};

template <BinaryOp Op>
class AssignOpSelfLitStmtNode : public AssignStmtNode {
 public:
  AssignOpSelfLitStmtNode(std::string n, int slot, int lit_val)
      : AssignStmtNode(std::move(n), slot, lit_val, Op) {}

  void execute(Environment& env) override {
    env.set_slot_value(slot_, eval_binary_op_static<Op>(
                                  env.get_slot_value(slot_), expr_lit_val_));
  }
};

class AssignExprStmtNode : public AssignStmtNode {
 public:
  AssignExprStmtNode(std::string n, int slot, std::unique_ptr<ExprNode> e)
      : AssignStmtNode(std::move(n), slot, std::move(e)) {}

  void execute(Environment& env) override {
    env.set_slot_value(slot_, raw_expr_->evaluate(env));
  }

  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override {
    if (expr) {
      expr = resolve_node(std::move(expr), env);
      raw_expr_ = expr.get();
    }
    return self;
  }

  void fold_constants() override {
    if (expr) {
      expr = fold_node(std::move(expr));
      raw_expr_ = expr.get();
    }
  }
};

inline std::unique_ptr<StmtNode> make_assign_self_lit_node(BinaryOp op,
                                                           std::string name,
                                                           int slot,
                                                           int lit_val) {
  switch (op) {
    case BinaryOp::Add:
      return std::make_unique<AssignOpSelfLitStmtNode<BinaryOp::Add>>(
          std::move(name), slot, lit_val);
    case BinaryOp::Subtract:
      return std::make_unique<AssignOpSelfLitStmtNode<BinaryOp::Subtract>>(
          std::move(name), slot, lit_val);
    case BinaryOp::Multiply:
      return std::make_unique<AssignOpSelfLitStmtNode<BinaryOp::Multiply>>(
          std::move(name), slot, lit_val);
    case BinaryOp::Divide:
      return std::make_unique<AssignOpSelfLitStmtNode<BinaryOp::Divide>>(
          std::move(name), slot, lit_val);
    case BinaryOp::Modulo:
      return std::make_unique<AssignOpSelfLitStmtNode<BinaryOp::Modulo>>(
          std::move(name), slot, lit_val);
    default:
      return nullptr;
  }
}

// tmp[x] = 5;
class TmpIndexAssignStmtNode : public StmtNode {
 public:
  TmpIndexAssignStmtNode(std::unique_ptr<ExprNode> i,
                         std::unique_ptr<ExprNode> v);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  std::unique_ptr<ExprNode> idx_expr;
  std::unique_ptr<ExprNode> val_expr;
  ExprNode* raw_idx_expr_ = nullptr;
  ExprNode* raw_val_expr_ = nullptr;
  int idx_slot_ = -1;
  int val_slot_ = -1;
  bool idx_is_lit_ = false;
  int idx_lit_val_ = 0;
  bool val_is_lit_ = false;
  int val_lit_val_ = 0;
};

// output[x] = 5;
class OutputIndexAssignStmtNode : public StmtNode {
 public:
  OutputIndexAssignStmtNode(std::unique_ptr<ExprNode> i,
                            std::unique_ptr<ExprNode> v);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  std::unique_ptr<ExprNode> idx_expr;
  std::unique_ptr<ExprNode> val_expr;
  ExprNode* raw_idx_expr_ = nullptr;
  ExprNode* raw_val_expr_ = nullptr;
  int idx_slot_ = -1;
  int val_slot_ = -1;
  bool idx_is_lit_ = false;
  int idx_lit_val_ = 0;
  bool val_is_lit_ = false;
  int val_lit_val_ = 0;
};

// print(x);
class PrintStmtNode : public StmtNode {
 public:
  explicit PrintStmtNode(std::unique_ptr<ExprNode> e);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  std::unique_ptr<ExprNode> expr;
  ExprNode* raw_expr_ = nullptr;
};

class IfStmtNode : public StmtNode {
 public:
  IfStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t,
             std::unique_ptr<StmtNode> e);

  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  std::unique_ptr<ExprNode> cond;
  std::unique_ptr<StmtNode> then_branch;
  std::unique_ptr<StmtNode> else_branch;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_then_ = nullptr;
  StmtNode* raw_else_ = nullptr;
};

class IfThenStmtNode : public StmtNode {
 public:
  IfThenStmtNode(std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> t);

  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

  ExprNode* get_cond() const noexcept { return raw_cond_; }
  StmtNode* get_then() const noexcept { return raw_then_; }
  std::unique_ptr<ExprNode> extract_cond() { return std::move(cond); }
  std::unique_ptr<StmtNode> extract_then() { return std::move(then_branch); }

 private:
  std::unique_ptr<ExprNode> cond;
  std::unique_ptr<StmtNode> then_branch;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_then_ = nullptr;
};

class ForStmtNode : public StmtNode {
 public:
  ForStmtNode(std::unique_ptr<StmtNode> i, std::unique_ptr<ExprNode> c,
              std::unique_ptr<StmtNode> u, std::unique_ptr<StmtNode> b);

  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  std::unique_ptr<StmtNode> init;
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> update;
  std::unique_ptr<StmtNode> body;
  StmtNode* raw_init_ = nullptr;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_update_ = nullptr;
  StmtNode* raw_body_ = nullptr;
};

class ForCountVarLimitStmtNode : public StmtNode {
 public:
  ForCountVarLimitStmtNode(int var_slot, int start_val, int limit_slot,
                           std::unique_ptr<StmtNode> body);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  int var_slot_;
  int start_val_;
  int limit_slot_;
  std::unique_ptr<StmtNode> body_;
  StmtNode* raw_body_ = nullptr;
};

class ForCountLitLimitStmtNode : public StmtNode {
 public:
  ForCountLitLimitStmtNode(int var_slot, int start_val, int limit_val,
                           std::unique_ptr<StmtNode> body);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  int var_slot_;
  int start_val_;
  int limit_val_;
  std::unique_ptr<StmtNode> body_;
  StmtNode* raw_body_ = nullptr;
};

class ForCountIfVarLimitStmtNode : public StmtNode {
 public:
  ForCountIfVarLimitStmtNode(int var_slot, int start_val, int limit_slot,
                             std::unique_ptr<ExprNode> cond,
                             std::unique_ptr<StmtNode> then_branch);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  int var_slot_;
  int start_val_;
  int limit_slot_;
  std::unique_ptr<ExprNode> cond_;
  std::unique_ptr<StmtNode> then_branch_;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_then_ = nullptr;
};

class ForCountIfLitLimitStmtNode : public StmtNode {
 public:
  ForCountIfLitLimitStmtNode(int var_slot, int start_val, int limit_val,
                             std::unique_ptr<ExprNode> cond,
                             std::unique_ptr<StmtNode> then_branch);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  int var_slot_;
  int start_val_;
  int limit_val_;
  std::unique_ptr<ExprNode> cond_;
  std::unique_ptr<StmtNode> then_branch_;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_then_ = nullptr;
};

template <template <BinaryOp> class NodeTemplate, typename... Args>
inline std::unique_ptr<StmtNode> make_while_node(BinaryOp op, Args&&... args) {
  switch (op) {
    case BinaryOp::Add:
      return std::make_unique<NodeTemplate<BinaryOp::Add>>(
          std::forward<Args>(args)...);
    case BinaryOp::Subtract:
      return std::make_unique<NodeTemplate<BinaryOp::Subtract>>(
          std::forward<Args>(args)...);
    case BinaryOp::Multiply:
      return std::make_unique<NodeTemplate<BinaryOp::Multiply>>(
          std::forward<Args>(args)...);
    case BinaryOp::Divide:
      return std::make_unique<NodeTemplate<BinaryOp::Divide>>(
          std::forward<Args>(args)...);
    case BinaryOp::Modulo:
      return std::make_unique<NodeTemplate<BinaryOp::Modulo>>(
          std::forward<Args>(args)...);
    case BinaryOp::Equal:
      return std::make_unique<NodeTemplate<BinaryOp::Equal>>(
          std::forward<Args>(args)...);
    case BinaryOp::NotEqual:
      return std::make_unique<NodeTemplate<BinaryOp::NotEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::Less:
      return std::make_unique<NodeTemplate<BinaryOp::Less>>(
          std::forward<Args>(args)...);
    case BinaryOp::LessEqual:
      return std::make_unique<NodeTemplate<BinaryOp::LessEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::Greater:
      return std::make_unique<NodeTemplate<BinaryOp::Greater>>(
          std::forward<Args>(args)...);
    case BinaryOp::GreaterEqual:
      return std::make_unique<NodeTemplate<BinaryOp::GreaterEqual>>(
          std::forward<Args>(args)...);
    case BinaryOp::LogicalAnd:
      return std::make_unique<NodeTemplate<BinaryOp::LogicalAnd>>(
          std::forward<Args>(args)...);
    case BinaryOp::LogicalOr:
      return std::make_unique<NodeTemplate<BinaryOp::LogicalOr>>(
          std::forward<Args>(args)...);
  }
  __builtin_unreachable();
}

class WhileBaseStmtNode : public StmtNode {
 public:
  explicit WhileBaseStmtNode(std::unique_ptr<StmtNode> body)
      : body_(std::move(body)), raw_body_(body_.get()) {
    init_body_cache();
  }

  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override {
    if (body_) body_ = resolve_stmt(std::move(body_), env);
    raw_body_ = body_.get();
    init_body_cache();
    return self;
  }

  void fold_constants() override {
    if (body_) {
      body_->fold_constants();
      raw_body_ = body_.get();
      init_body_cache();
    }
  }

 protected:
  void init_body_cache() {
    raw_stmt0_ = nullptr;
    raw_stmt1_ = nullptr;
    if (auto* blk = dynamic_cast<BlockStmtNode*>(raw_body_)) {
      if (blk->get_statements_size() == 2) {
        raw_stmt0_ = blk->get_raw_statements()[0];
        raw_stmt1_ = blk->get_raw_statements()[1];
      }
    }
  }

  inline void execute_body(Environment& env) noexcept {
    if (raw_stmt1_) {
      raw_stmt0_->execute(env);
      raw_stmt1_->execute(env);
    } else {
      raw_body_->execute(env);
    }
  }

  std::unique_ptr<StmtNode> body_;
  StmtNode* raw_body_ = nullptr;
  StmtNode* raw_stmt0_ = nullptr;
  StmtNode* raw_stmt1_ = nullptr;
};

template <BinaryOp Op>
class WhileVarLitCondStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarLitCondStmtNode(int slot, int val, std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)), slot_(slot), lit_val_(val) {}

  void execute(Environment& env) override {
    while (eval_binary_op_static<Op>(env.get_slot_value(slot_), lit_val_) !=
           0) {
      execute_body(env);
    }
  }

 private:
  int slot_;
  int lit_val_;
};

template <BinaryOp Op>
class WhileVarVarCondStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarVarCondStmtNode(int left_slot, int right_slot,
                          std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)),
        left_slot_(left_slot),
        right_slot_(right_slot) {}

  void execute(Environment& env) override {
    while (eval_binary_op_static<Op>(env.get_slot_value(left_slot_),
                                     env.get_slot_value(right_slot_)) != 0) {
      execute_body(env);
    }
  }

 private:
  int left_slot_;
  int right_slot_;
};

template <BinaryOp Op>
class WhileLitVarCondStmtNode : public WhileBaseStmtNode {
 public:
  WhileLitVarCondStmtNode(int lit_val, int slot, std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)), lit_val_(lit_val), slot_(slot) {}

  void execute(Environment& env) override {
    while (eval_binary_op_static<Op>(lit_val_, env.get_slot_value(slot_)) !=
           0) {
      execute_body(env);
    }
  }

 private:
  int lit_val_;
  int slot_;
};

template <BinaryOp Op>
class WhileVarSquareVarLimitStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarSquareVarLimitStmtNode(int var_slot, int limit_slot,
                                 std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)),
        var_slot_(var_slot),
        limit_slot_(limit_slot) {}

  void execute(Environment& env) override {
    while (eval_binary_op_static<Op>(
               env.get_slot_value(var_slot_) * env.get_slot_value(var_slot_),
               env.get_slot_value(limit_slot_)) != 0) {
      execute_body(env);
    }
  }

 private:
  int var_slot_;
  int limit_slot_;
};

template <BinaryOp Op>
class WhileVarSquareLitLimitStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarSquareLitLimitStmtNode(int var_slot, int lit_val,
                                 std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)),
        var_slot_(var_slot),
        lit_val_(lit_val) {}

  void execute(Environment& env) override {
    while (eval_binary_op_static<Op>(
               env.get_slot_value(var_slot_) * env.get_slot_value(var_slot_),
               lit_val_) != 0) {
      execute_body(env);
    }
  }

 private:
  int var_slot_;
  int lit_val_;
};

class WhileVarCondStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarCondStmtNode(int slot, std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)), slot_(slot) {}

  void execute(Environment& env) override {
    while (env.get_slot_value(slot_) != 0) {
      execute_body(env);
    }
  }

 private:
  int slot_;
};

class WhileVarModVarZeroStmtNode : public WhileBaseStmtNode {
 public:
  WhileVarModVarZeroStmtNode(int num_slot, int div_slot,
                             std::unique_ptr<StmtNode> body)
      : WhileBaseStmtNode(std::move(body)),
        num_slot_(num_slot),
        div_slot_(div_slot) {}

  void execute(Environment& env) override {
    while (true) {
      int d = env.get_slot_value(div_slot_);
      if (d == 0 || (env.get_slot_value(num_slot_) % d) != 0) break;
      execute_body(env);
    }
  }

 private:
  int num_slot_;
  int div_slot_;
};

class WhileStmtNode : public StmtNode {
 public:
  WhileStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b);
  void execute(Environment& env) override;
  std::unique_ptr<StmtNode> resolve_slots(std::unique_ptr<StmtNode> self,
                                          Environment& env) override;
  void fold_constants() override;

 private:
  void init_body_cache();

  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> body;
  ExprNode* raw_cond_ = nullptr;
  StmtNode* raw_body_ = nullptr;
  StmtNode* raw_stmt0_ = nullptr;
  StmtNode* raw_stmt1_ = nullptr;
};

#endif