#ifndef BUILTIN_H
#define BUILTIN_H
#include <functional>
#include <unordered_map>

using BuiltinFn = std::function<int(const std::vector<int>&)>;
using BuiltinTable = std::unordered_map<std::string, BuiltinFn>;

const BuiltinTable& get_builtin_functions();

#endif