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

int ipow(int base, int exp) {
  int result = 1;
  for (int i = 0; i < exp; ++i) {
    result *= base;
  }
  return result;
}

int digit_count(int n) {
  int num = n;
  if (num == 0) return 1;

  int count = 0;
  while (num != 0) {
    num /= 10;
    count++;
  }
  return count;
}

int reverse_num(int n) {
  int rev_num = 0;
  while (n > 0) {
    rev_num = rev_num * 10 + n % 10;
    n = n / 10;
  }
  return rev_num;
}

// Built-in: prime(x)
int fn_prime(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("prime() requires exactly 1 argument");
  }
  return is_prime(args[0]) ? 1 : 0;
}

int fn_palindrome(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("palindrome() requires exactly 1 argument");
  }
  return (args[0] == reverse_num(args[0])) ? 1 : 0;
}

// // Built-in: gcd(a, b)
// int fn_gcd(const std::vector<int>& args) {
//   if (args.size() != 2) {
//     throw std::runtime_error("gcd() requires exactly 2 arguments");
//   }
//   return std::gcd(args[0], args[1]);
// }

// Built-in: pow(base, exp)
int fn_pow(const std::vector<int>& args) {
  if (args.size() != 2) {
    throw std::runtime_error("pow() requires exactly 2 arguments");
  }
  return ipow(args[0], args[1]);
}

int fn_isqrt(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("isqrt() requires exactly 1 argument");
  }

  return std::sqrt(args[0]);
}

int fn_dsum(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("dsum() requires exactly 1 argument");
  }

  int sum = 0;
  int n = args[0];
  while (n != 0) {
    int last = n % 10;
    sum += last;
    n /= 10;
  }
  return sum;
}

int fn_get_nth_digit(const std::vector<int>& args) {
  if (args.size() != 2) {
    throw std::runtime_error("get_nth_digit() requires exactly 2 argument");
  }
  return (args[0] / ipow(10, digit_count(args[0]) - args[1])) % 10;
}

// Built-in: pow(base, exp)
int fn_reverse(const std::vector<int>& args) {
  if (args.size() != 1) {
    throw std::runtime_error("reverse() requires exactly 1 argument");
  }
  return reverse_num(args[0]);
}

}  // anonymous namespace

const BuiltinTable& get_builtin_functions() {
  static const BuiltinTable registry = {{"is_prime", fn_prime},
                                        {"is_palindrome", fn_palindrome},
                                        // {"gcd", fn_gcd},
                                        {"pow", fn_pow},
                                        {"isqrt", fn_isqrt},
                                        {"dsum", fn_dsum},
                                        {"get_nth_digit", fn_get_nth_digit},
                                        {"reverse", fn_reverse}};

  return registry;
}