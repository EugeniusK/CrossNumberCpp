#ifndef BUILTIN_H
#define BUILTIN_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>


// Core mathematical and digit manipulation utilities
int ipow(int base, int exp);
int get_nth_digit(int n, int digit, int total_digit);
int digit_count(int n);
int reverse_num(int n);
int dsum(int n);
bool is_prime(int n);
int isqrt(int n);

// Built-in functions registry for DSL
using BuiltinFn = std::function<int(const std::vector<int>&)>;
using BuiltinTable = std::unordered_map<std::string, BuiltinFn>;

const BuiltinTable& get_builtin_functions();

#endif