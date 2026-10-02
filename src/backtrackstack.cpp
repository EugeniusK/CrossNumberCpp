#include "backtrackstack.h"

#include <stdexcept>

void BacktrackStack::push(int val) {
  stack.push_back(val);
}

int BacktrackStack::pop() {
  if (stack.empty()) {
    throw std::logic_error("Stack is empty");
  }
  int val = stack.back();
  stack.pop_back();
  return val;
}

int BacktrackStack::get_top() const {
  if (stack.empty()) {
    throw std::logic_error("Stack is empty");
  }
  return stack.back();
}

int BacktrackStack::get_size() const {
  return static_cast<int>(stack.size());
}
