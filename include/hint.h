#ifndef HINT_H
#define HINT_H

#include <string>
#include <vector>

#include "parser.h"

class Hint : Parser {
 public:
  Hint(int identifier, int is_horizontal, std::string program = "");
  void set_env(std::string variable_name, int val);
  int get_output_array(int idx);
  const std::vector<int>& get_written_output_indices() const;
  bool get_if_has_dependencies() const;
  std::vector<std::string> get_dependencies() const;

  std::vector<int> possible_values;
  int number_possible_values;

  int get_identifier() const;
  bool get_is_horizontal() const;

  int get_length() const;
  void set_length(int len);

  void run_program_on_load();
  void run_program_on_dependency();

  int get_x_pos() const;
  void set_x_pos(int pos);
  int get_y_pos() const;
  void set_y_pos(int pos);

 private:
  int identifier;
  bool is_horizontal;
  int length = 0;
  int x_pos = 0;
  int y_pos = 0;
  Environment env;
  std::unique_ptr<BlockStmtNode> cached_program;
};

#endif