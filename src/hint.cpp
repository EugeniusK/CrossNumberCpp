#include "hint.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "builtin.h"

Hint::Hint(int id, int is_hor, std::string program)
    : Parser(Lexer(std::move(program))),
      number_possible_values(0),
      partial_identifier(id),
      is_horizontal(is_hor),
      length(0),
      x_pos(0),
      y_pos(0) {
  identifier = (is_hor ? "a" : "d") + std::to_string(id);
}

bool Hint::get_if_has_dependencies() const { return has_dependencies; }

std::vector<std::string> Hint::get_dependencies() const {
  return {list_dependencies.begin(), list_dependencies.end()};
}

void Hint::set_env(const std::string& variable_name, int val) {
  int slot = env.find_slot(variable_name);
  if (slot >= 0) {
    env.set_slot_value(slot, val);
  } else {
    env.set_var(variable_name, val);
  }
}

void Hint::finalize_candidates() {
  static constexpr int pow10[] = {
      1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
  };
  candidates.resize(possible_values.size());
  for (size_t i = 0; i < possible_values.size(); ++i) {
    int v = possible_values[i];
    candidates[i].value = v;
    for (int d = 0; d < length && d < 8; ++d) {
      candidates[i].digits[d] = static_cast<uint8_t>((v / pow10[length - 1 - d]) % 10);
    }
  }
}

int Hint::get_partial_identifier() const { return partial_identifier; }
// std::string Hint::get_identifier() const { return identifier; }

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
  cached_program = parse_program();
  for (const auto& dep : list_dependencies) {
    env.get_or_create_slot(dep);
  }
  cached_program = resolve_stmt(std::move(cached_program), env);
  if (!get_if_has_dependencies()) {
    cached_program->execute(env);

    for (int i = 0; i < ipow(10, length); i++) {
      if (env.get_output_array(i) == 1 && i >= ipow(10, length - 1) &&
          i < ipow(10, length)) {
        possible_values.push_back(i);
      }
    }
    number_possible_values = possible_values.size();
  }

  if (number_possible_values == 0) {
    int start = (length == 1) ? 0 : ipow(10, length - 1);
    possible_values.reserve(ipow(10, length) - start);
    for (int i = start; i < ipow(10, length); i++) {
      possible_values.push_back(i);
    }
    number_possible_values = possible_values.size();
  }
  finalize_candidates();
}

void Hint::run_program_on_dependency() {
  env.reset_output_array();
  env.reset_tmp_array();
  if (!cached_program) {
    reset();
    cached_program = parse_program();
    for (const auto& dep : list_dependencies) {
      env.get_or_create_slot(dep);
    }
    cached_program = resolve_stmt(std::move(cached_program), env);
  }
  cached_program->execute(env);
}

const std::vector<int>& Hint::get_written_output_indices() const {
  return env.get_written_output_indices();
}

void Hint::set_x_pos(int pos) { x_pos = pos; }
void Hint::set_y_pos(int pos) { y_pos = pos; }