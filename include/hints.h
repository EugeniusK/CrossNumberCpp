#ifndef HINTS_H
#define HINTS_H

#include <vector>

class Hint {
 public:
  Hint(int identifier, int is_horizontal);
  void load(std::vector<int> (*func)(int));
  int length;
  int x_pos;
  int y_pos;
  int priority;
  bool is_horizontal;

  std::vector<int> possible_values;
  int identifier;
  int number_possible_values;

 private:
};
#endif