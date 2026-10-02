#ifndef BACKTRACKSTACK_H
#define BACKTRACKSTACK_H

#include <vector>

class BacktrackStack {
 public:
  BacktrackStack() = default;
  void push(int val);
  int pop();
  int get_top() const;
  int get_size() const;

 private:
  std::vector<int> stack;
};

#endif