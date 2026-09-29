#ifndef RUNTIME_H
#define RUNTIME_H
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using Context = std::unordered_map<std::string, int>;

class Workspace {
 public:
  Workspace(int length);
  void clear();
  bool get(int idx);
  void set(int idx, bool val);

 private:
  std::vector<bool> s;
};

#endif