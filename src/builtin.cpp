#include "builtin.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace {

// Helper: Checks primality
bool is_prime(int n) {
  if (n < 2) return false;
  for (int i = 2; i * i <= n; ++i) {
    if (n % i == 0) return false;
  }
  return true;
}

// Built-in: prime(x)
int fn_prime(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("prime() requires exactly 1 argument");
  }
  return is_prime(args[0]) ? 1 : 0;
}

// Built-in: gcd(a, b)
int fn_gcd(const std::vector<int>& args) {
  if (args.size() != 2) {
    throw std::runtime_error("gcd() requires exactly 2 arguments");
  }
  return std::gcd(args[0], args[1]);
}

// Built-in: pow(base, exp)
int fn_pow(const std::vector<int>& args) {
  if (args.size() != 2) {
    throw std::runtime_error("pow() requires exactly 2 arguments");
  }
  return static_cast<int>(std::pow(args[0], args[1]));
}

// Built-in: clamp(val, low, high)
int fn_clamp(const std::vector<int>& args) {
  if (args.size() != 3) {
    throw std::runtime_error("clamp() requires exactly 3 arguments");
  }
  return std::clamp(args[0], args[1], args[2]);
}

}  // anonymous namespace

const BuiltinTable& get_builtin_functions() {
  static const BuiltinTable registry = {
      {"prime", fn_prime},
      {"gcd", fn_gcd},
      {"pow", fn_pow},
      {"clamp", fn_clamp},
  };
  return registry;
}