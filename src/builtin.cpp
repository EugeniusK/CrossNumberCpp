#include "builtin.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

DsumLut::DsumLut() {
  table[0] = 0;
  for (int i = 1; i < DSUM_LUT_SIZE; ++i) {
    table[i] = static_cast<uint8_t>(table[i / 10] + (i % 10));
  }
}

const DsumLut g_dsum_lut;

namespace {

int fn_prime(const int* args, size_t count) {
  if (count != 1) {
    throw std::runtime_error("is_prime() requires exactly 1 argument");
  }
  return is_prime(args[0]) ? 1 : 0;
}

int fn_palindrome(const int* args, size_t count) {
  if (count != 1) {
    throw std::runtime_error("is_palindrome() requires exactly 1 argument");
  }
  return (args[0] == reverse_num(args[0])) ? 1 : 0;
}

int fn_pow(const int* args, size_t count) {
  if (count != 2) {
    throw std::runtime_error("pow() requires exactly 2 arguments");
  }
  return ipow(args[0], args[1]);
}

int fn_isqrt(const int* args, size_t count) {
  if (count != 1) {
    throw std::runtime_error("isqrt() requires exactly 1 argument");
  }
  return isqrt(args[0]);
}

int fn_dsum(const int* args, size_t count) {
  if (count != 1) {
    throw std::runtime_error("dsum() requires exactly 1 argument");
  }
  return dsum(args[0]);
}

int fn_get_nth_digit(const int* args, size_t count) {
  if (count != 2) {
    throw std::runtime_error("get_nth_digit() requires exactly 2 arguments");
  }
  return get_nth_digit(args[0], args[1] - 1);
}

int fn_reverse(const int* args, size_t count) {
  if (count != 1) {
    throw std::runtime_error("reverse() requires exactly 1 argument");
  }
  return reverse_num(args[0]);
}

}  // anonymous namespace

const BuiltinTable& get_builtin_functions() {
  static const BuiltinTable registry = {
      {"is_prime", fn_prime}, {"is_palindrome", fn_palindrome},
      {"pow", fn_pow},        {"isqrt", fn_isqrt},
      {"dsum", fn_dsum},      {"get_nth_digit", fn_get_nth_digit},
      {"reverse", fn_reverse}};

  return registry;
}