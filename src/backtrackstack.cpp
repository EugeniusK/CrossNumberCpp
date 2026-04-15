#include "backtrackstack.h"

BacktrackStack::BacktrackStack() {
  std::fill(stack, stack + MAX_HINT_COUNT, 0);
}
void BacktrackStack::push(int val) {
  if (top == MAX_HINT_COUNT) {
    throw std::logic_error("Stack is full");
  }
  stack[top] = val;
  top++;
}
int BacktrackStack::pop() {
  if (top == 0) {
    throw std::logic_error("Stack is empty");
  }
  top--;
  return stack[top];
}
int BacktrackStack::get_top() {
  if (top == 0) {
    throw std::logic_error("Stack is empty");
  }
  return stack[top - 1];
}
int BacktrackStack::get_head() { return stack[0]; }
int BacktrackStack::get_size() { return top; }
std::string BacktrackStack::display() {
  std::string out = "";
  for (int i = 0; i < top; i++) {
    out += std::to_string(stack[i]);
    out += ", ";
  }
  return out;
}
