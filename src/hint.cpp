#include "hint.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

#include "utils.h"

Hint::Hint(int id, int is_hor, std::string program)
    : Parser(Lexer(std::move(program))),
      number_possible_values(0),
      identifier(id),
      is_horizontal(is_hor) {}

bool Hint::get_if_has_dependencies() const { return has_dependencies; }
std::vector<std::string> Hint::get_dependencies() const {
  return {list_dependencies.begin(), list_dependencies.end()};
}

void Hint::set_env(std::string variable_name, int val) {
  env.set_var(variable_name, val);
}

int Hint::get_output_array(int idx) { return env.get_output_array(idx); }

int Hint::get_identifier() const { return identifier; }

bool Hint::get_is_horizontal() const { return is_horizontal; }

int Hint::get_length() const { return length; }
void Hint::set_length(int len) {
  if (len < 2) {
    throw std::invalid_argument(
        "Hint length must be greater than or equal to 2 (got " +
        std::to_string(len) + ")");
  }
  length = len;
}

void Hint::run_program_on_load() {
  env.initialise_output_array(static_cast<int>(ipow(10, length)), 0);
  env.initialise_tmp_array(static_cast<int>(ipow(10, length)), 0);
  auto program = parse_program();
  if (get_if_has_dependencies()) {
  } else {
    program->execute(env);

    for (int i = 0; i < ipow(10, length); i++) {
      if (env.get_output_array(i) == 1 && i >= ipow(10, length - 1) &&
          i < ipow(10, length)) {
        possible_values.push_back(i);
      }
    }
    this->number_possible_values = this->possible_values.size();
  }

  if (number_possible_values == 0) {
    int start = (length == 1) ? 0 : ipow(10, length - 1);
    possible_values.reserve(ipow(10, length) - start);
    for (int i = start; i < ipow(10, length); i++) {
      possible_values.push_back(i);
    }
    number_possible_values = possible_values.size();
  }
}

void Hint::run_program_on_dependency() {
  reset();
  auto program = parse_program();
  env.reset_output_array();
  env.reset_tmp_array();
  program->execute(env);
}

int Hint::get_x_pos() const { return x_pos; }
void Hint::set_x_pos(int pos) { x_pos = pos; }
int Hint::get_y_pos() const { return y_pos; }
void Hint::set_y_pos(int pos) { y_pos = pos; }

void Hint::load(std::vector<int> (*func)(int)) {
  possible_values.clear();
  number_possible_values = 0;

  std::vector<int> arr = func(ipow(10, length));

  for (int i : arr) {
    if (i >= ipow(10, length - 1) && i < ipow(10, length)) {
      possible_values.push_back(i);
    }
  }

  std::ranges::sort(possible_values);
  auto [first, last] = std::ranges::unique(possible_values);
  possible_values.erase(first, last);

  number_possible_values = possible_values.size();
}