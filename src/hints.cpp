#include "hints.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

#include "parser_new.h"
#include "utils.h"

Hint::Hint(int id, int is_hor, std::string program)
    : Parser(Lexer(std::move(program))), identifier(id), is_horizontal(is_hor) {
  env = Environment();
  number_possible_values = 0;
}
bool Hint::get_if_has_dependencies() { return has_dependencies; }
std::vector<std::string> Hint::get_dependencies() {
  std::vector<std::string> output(list_dependencies.begin(),
                                  list_dependencies.end());
  return output;
}

void Hint::set_env(std::string variable_name, int val) {
  env.set_var(variable_name, val);
}

int Hint::get_output_array(int idx) { return env.get_output_array(idx); }

int Hint::get_identifier() { return identifier; }

bool Hint::get_is_horizontal() { return is_horizontal; }

int Hint::get_length() { return length; }
void Hint::set_length(int len) { length = len; }

void Hint::run_program_on_load() {
  env.initialise_output_array(static_cast<int>(ipow(10, length)), 0);
  env.initialise_tmp_array(static_cast<int>(ipow(10, length)), 0);
  auto program = parse_program();
  // std::cout << print_list_variables() << std::endl;
  if (get_if_has_dependencies()) {
    std::cout << std::to_string(identifier) << "has dependency!!" << std::endl;

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
    possible_values.reserve(ipow(10, length) - ipow(10, length - 1));
    for (int i = ipow(10, length - 1); i < ipow(10, length); i++) {
      possible_values.push_back(i);
    }
    number_possible_values = possible_values.size();
  }
}

void Hint::run_program_on_dependency() {
  reset();
  auto program = parse_program();
  env.reset_output_array();
  program->execute(env);
}

int Hint::get_x_pos() { return x_pos; };
void Hint::set_x_pos(int pos) { x_pos = pos; }
int Hint::get_y_pos() { return y_pos; };
void Hint::set_y_pos(int pos) { y_pos = pos; }

void Hint::load(std::vector<int> (*func)(int)) {
  this->possible_values.clear();
  this->number_possible_values = 0;

  std::vector<int> arr = func(ipow(10, this->length));

  for (int i : arr) {
    if (i >= ipow(10, this->length - 1) && i < ipow(10, this->length)) {
      this->possible_values.push_back(i);
    }
  }

  sort(this->possible_values.begin(), this->possible_values.end());
  auto it =
      std::unique(this->possible_values.begin(), this->possible_values.end());
  this->possible_values.erase(it, this->possible_values.end());

  this->number_possible_values = this->possible_values.size();
}