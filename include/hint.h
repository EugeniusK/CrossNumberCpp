#ifndef HINT_H
#define HINT_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "parser.h"

struct CandidateValue {
  int value;
  std::array<uint8_t, 8> digits;
};

class Hint : Parser {
 public:
  Hint(int identifier, int is_horizontal, std::string program = "");
  void set_env(const std::string& variable_name, int val);
  int get_slot_for_var(const std::string& variable_name) const {
    return env.find_slot(variable_name);
  }
  void set_slot_env(int slot, int val) { env.set_slot_value(slot, val); }
  inline int get_output_array(int idx) const noexcept {
    return env.get_output_array(idx);
  }
  const std::vector<int>& get_written_output_indices() const;
  bool get_if_has_dependencies() const;
  std::vector<std::string> get_dependencies() const;

  std::vector<int> possible_values;
  int number_possible_values;
  std::vector<CandidateValue> candidates;
  void finalize_candidates();

  int get_partial_identifier() const;
  [[nodiscard]] inline std::string get_identifier() const noexcept {
    return identifier;
  };
  [[nodiscard]] inline bool get_is_horizontal() const noexcept {
    return is_horizontal;
  };

  [[nodiscard]] inline int get_length() const noexcept { return length; }
  void set_length(int len);

  void run_program_on_load();
  void run_program_on_dependency();

  [[nodiscard]] inline int get_x_pos() const noexcept { return x_pos; };
  void set_x_pos(int pos);
  [[nodiscard]] inline int get_y_pos() const noexcept { return y_pos; };
  void set_y_pos(int pos);

 private:
  int partial_identifier;
  std::string identifier;
  bool is_horizontal;
  int length = 0;
  int x_pos = 0;
  int y_pos = 0;
  Environment env;
  std::unique_ptr<StmtNode> cached_program;
};

#endif