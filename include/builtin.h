#ifndef BUILTIN_H
#define BUILTIN_H

#include <string>
#include <unordered_map>
#include <vector>

inline constexpr int ipow(int base, int exp) {
  // Fast path for powers of 10 (single check validates 0 <= exp < 10)
  if (base == 10 && static_cast<unsigned>(exp) < 10) {
    constexpr int kPow10[10] = {1,         10,        100,     1000,
                                10000,     100000,    1000000, 10000000,
                                100000000, 1000000000};
    return kPow10[exp];
  }

  if (exp <= 0) return 1;

  // O(log exp) exponentiation by squaring for other bases
  int result = 1;
  while (exp > 0) {
    if (exp & 1) result *= base;
    base *= base;
    exp >>= 1;
  }
  return result;
}
inline constexpr int digit_count(int n) {
  if (n < 0) {
    if (n == -2147483647 - 1) return 10;
    n = -n;
  }
  if (n < 10) return 1;
  if (n < 100) return 2;
  if (n < 1000) return 3;
  if (n < 10000) return 4;
  if (n < 100000) return 5;
  if (n < 1000000) return 6;
  if (n < 10000000) return 7;
  if (n < 100000000) return 8;
  if (n < 1000000000) return 9;
  return 10;
}

// 0-indexed from the left: digit=0 is the leftmost digit. Returns 0 if out of
// bounds.
inline constexpr int get_nth_digit(int n, int digit, int total_digit) {
  if (static_cast<unsigned>(digit) >= static_cast<unsigned>(total_digit)) {
    return 0;
  }
  if (n < 0) {
    if (n == -2147483647 - 1)
      n = 2147483647;
    else
      n = -n;
  }
  return (n / ipow(10, total_digit - digit - 1)) % 10;
}

inline constexpr int get_nth_digit(int n, int digit) {
  return get_nth_digit(n, digit, digit_count(n));
}
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

inline int reverse_num(int n) {
  int num = std::abs(n);
  int rev_num = 0;
  while (num > 0) {
    rev_num = rev_num * 10 + num % 10;
    num /= 10;
  }
  return rev_num;
}

constexpr int DSUM_LUT_SIZE = 1000000;
struct DsumLut {
  uint8_t table[DSUM_LUT_SIZE];
  DsumLut();
};
extern const DsumLut g_dsum_lut;

inline int dsum(int n) {
  if (static_cast<unsigned>(n) < static_cast<unsigned>(DSUM_LUT_SIZE)) {
    return g_dsum_lut.table[n];
  }
  int num = std::abs(n);
  if (num < DSUM_LUT_SIZE) {
    return g_dsum_lut.table[num];
  }
  int sum = 0;
  while (num != 0) {
    sum += num % 10;
    num /= 10;
  }
  return sum;
}

inline bool is_prime(int n) {
  if (n <= 3) return n > 1;
  if ((n % 2) == 0 || (n % 3) == 0) return false;
  for (int i = 5; 1LL * i * i <= n; i += 6) {
    if (n % i == 0 || n % (i + 2) == 0) return false;
  }
  return true;
}

inline int isqrt(int n) {
  if (n < 0) {
    throw std::runtime_error("isqrt() requires non-negative argument");
  }
  return static_cast<int>(std::sqrt(n));
}

enum class BuiltinFn : uint8_t {
  Dsum = 0,
  GetNthDigit = 1,
  IsPrime = 2,
  IsPalindrome = 3,
  Pow = 4,
  Isqrt = 5,
  Reverse = 6
};

// Built-in functions registry for DSL
using BuiltinFnPtr = int (*)(const int* args, size_t count);
using BuiltinTable = std::unordered_map<std::string, BuiltinFnPtr>;

const BuiltinTable& get_builtin_functions();

#endif