#ifndef BACKTRACKSTACK_H
#define BACKTRACKSTACK_H
#include <string>

#include "utils.h"

class BacktrackStack {
 public:
  BacktrackStack();
  void push(int val);
  int pop();
  int get_top();
  int get_head();
  int get_size();
  std::string display();

 private:
  int stack[MAX_HINT_COUNT];
  int top = 0;
};
#endif