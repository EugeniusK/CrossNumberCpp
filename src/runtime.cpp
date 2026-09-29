#include "runtime.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// #include "lexer.h"
// #include "parser.h"

Workspace::Workspace(int length) {
  s.reserve(length);
  s.resize(length);
  std::fill(s.begin(), s.end(), true);
};

void Workspace::clear() { std::fill(s.begin(), s.end(), false); };

bool Workspace::get(int idx) {
  if (idx >= 1 && idx <= s.size()) {
    return s[idx - 1];
  } else {
    return false;
  }
};

void Workspace::set(int idx, bool val) {
  if (idx >= 1 && idx <= s.size()) {
    s[idx - 1] = val;
  }
};
