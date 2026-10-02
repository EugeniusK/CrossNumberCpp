#ifndef BACKTRACKSTACK_H
#define BACKTRACKSTACK_H
#include <array>
#include <string>

#include "utils.h"

class BacktrackStack {
 public:
  BacktrackStack() = default;
  void push(int val);
  int pop();
  int get_top() const;
  int get_head() const;
  int get_size() const;
  std::string display() const;

 private:
  std::array<int, MAX_HINT_COUNT> stack{};
  int top = 0;
};
#endif