#ifndef HINTS_H
#define HINTS_H

#include <string>
#include <vector>

#include "parser_new.h"
class Hint : Parser {
 public:
  Hint(int identifier, int is_horizontal, std::string program = "");
  void load(std::vector<int> (*func)(int));
  void set_env(std::string variable_name, int val);
  int get_output_array(int idx);
  bool get_if_has_dependencies();
  std::vector<std::string> get_dependencies();
  //   int priority;

  std::vector<int> possible_values;
  int number_possible_values;

  int get_identifier();
  bool get_is_horizontal();

  int get_length();
  void set_length(int len);

  void run_program_on_load();
  void run_program_on_dependency();

  int get_x_pos();
  void set_x_pos(int pos);
  int get_y_pos();
  void set_y_pos(int pos);

 private:
  int identifier;
  bool is_horizontal;
  int length;
  int x_pos;
  int y_pos;
  Environment env;
};
#endif