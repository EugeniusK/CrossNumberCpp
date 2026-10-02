#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "crossnumber.h"

ArrayCrossNumber::ArrayCrossNumber(int width, int height)
    : CrossNumber(width, height), width(width), height(height) {
  std::fill(std::begin(layout), std::end(layout), -2);
  std::fill(std::begin(value), std::end(value), -1);
  if (width * height > MAX_SQUARE_COUNT) {
    throw std::invalid_argument("Size of board cannot exceed " +
                                std::to_string(MAX_SQUARE_COUNT));
  }
}

void ArrayCrossNumber::define_squares(std::vector<int> squares) {
  for (size_t i = 0; i < squares.size(); ++i) {
    layout[i] = squares[i];
    if (squares[i] != -1) {
      value[i] = -2;
      // digits[i] = Digit();
    } else {
      value[i] = -1;
    }
  }
}

std::string ArrayCrossNumber::display_layout() {
  std::string output = "";

  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      output += "----";
    }
    output += "-\n";

    for (int w = 0; w < width; w++) {
      output += "|";
      if (layout[h * width + w] == -1) {
        output += "\u2588\u2588\u2588";
      } else if (layout[h * width + w] != 0) {
        int l = std::to_string(layout[h * width + w]).length();
        for (int j = 0; j < 3 - l; j++) {
          output += " ";
        }
        output += std::to_string(layout[h * width + w]);
      } else {
        output += "   ";
      }
    }
    output += "|\n";
  }
  for (int w = 0; w < width; w++) {
    output += "----";
  }
  output += "-";
  return output;
}

std::string ArrayCrossNumber::display_value() {
  std::string output = "";

  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      output += "----";
    }
    output += "-\n";

    for (int w = 0; w < width; w++) {
      output += "|";
      if (value[h * width + w] == -1)  // layout block
      {
        output += "\u2588\u2588\u2588";
      } else if (value[h * width + w] >= 0) {
        output += " ";

        output += std::to_string(value[h * width + w]);
        output += " ";
      } else  // is -2, nothing set yet
      {
        output += "   ";
      }
    }
    output += "|\n";
  }
  for (int w = 0; w < width; w++) {
    output += "----";
  }
  output += "-";
  return output;
}

std::string ArrayCrossNumber::display_digit_count() {
  auto format_width_3 = [](int val) -> std::string {
    std::string s = std::to_string(val);
    if (s.length() < 3) {
      s = std::string(3 - s.length(), ' ') + s;
    }
    return s;
  };

  std::string output = "Digit |";
  for (int i = 0; i < 10; ++i) {
    output += format_width_3(i);
    output += " |";
  }
  output += "\n";

  output += "-------";
  for (int i = 0; i < 10; ++i) {
    output += "-----";
  }
  output += "\n";

  output += "Count |";
  for (int i = 0; i < 10; ++i) {
    output += format_width_3(count_digits(i));
    output += " |";
  }

  return output;
}

void ArrayCrossNumber::set_value(int val, int x_pos, int y_pos, int length,
                                 bool is_horizontal) {
  auto cell_idx = [&](int i) {
    return is_horizontal ? y_pos * width + (x_pos + i)
                         : (y_pos + i) * width + x_pos;
  };
  for (int i = 0; i < length; ++i) {
    value[cell_idx(i)] = (val / ipow(10, length - i - 1)) % 10;
  }
}

int ArrayCrossNumber::get_value(int x_pos, int y_pos, int length,
                                bool is_horizontal) {
  auto cell_idx = [&](int i) {
    return is_horizontal ? y_pos * width + (x_pos + i)
                         : (y_pos + i) * width + x_pos;
  };
  int sum = 0;
  for (int i = 0; i < length; ++i) {
    sum += value[cell_idx(i)] * ipow(10, length - i - 1);
  }
  return sum;
}

void ArrayCrossNumber::clear_value(int x_pos, int y_pos, int length,
                                   bool is_horizontal) {
  auto cell_idx = [&](int i) {
    return is_horizontal ? y_pos * width + (x_pos + i)
                         : (y_pos + i) * width + x_pos;
  };
  for (int i = 0; i < length; ++i) {
    value[cell_idx(i)] = -2;
  }
}

bool ArrayCrossNumber::is_possible_value(int val, int x_pos, int y_pos,
                                         int length, bool is_horizontal) {
  auto cell_idx = [&](int i) {
    return is_horizontal ? y_pos * width + (x_pos + i)
                         : (y_pos + i) * width + x_pos;
  };
  for (int i = 0; i < length; ++i) {
    int idx = cell_idx(i);
    int digit = (val / ipow(10, length - i - 1)) % 10;
    if (value[idx] != digit && value[idx] != -2) {
      return false;
    }
  }
  return true;
}

void ArrayCrossNumber::set_value(int val, Hint& hint) {
  set_value(val, hint.get_x_pos(), hint.get_y_pos(), hint.get_length(),
            hint.get_is_horizontal());
}
int ArrayCrossNumber::get_value(Hint& hint) {
  return get_value(hint.get_x_pos(), hint.get_y_pos(), hint.get_length(),
                   hint.get_is_horizontal());
}
void ArrayCrossNumber::clear_value(Hint& hint) {
  clear_value(hint.get_x_pos(), hint.get_y_pos(), hint.get_length(),
              hint.get_is_horizontal());
}
bool ArrayCrossNumber::is_possible_value(int val, Hint& hint) {
  return is_possible_value(val, hint.get_x_pos(), hint.get_y_pos(),
                           hint.get_length(), hint.get_is_horizontal());
}

void ArrayCrossNumber::load_hint(Hint& hint) {
  // try and find the x_pos and y_pos of the hint based on the identifier
  // if not found, throw error
  bool found = false;
  for (int i = 0; i < this->width * this->height; i++) {
    if (layout[i] == hint.get_identifier()) {
      found = true;
      hint.set_x_pos(i % width);
      hint.set_y_pos(i / width);
      break;
    }
  }

  if (found == false) {
    throw std::logic_error("Hint not in layout initially defined");
  }

  // find the length of the hint
  int length = 0;

  if (hint.get_is_horizontal()) {
    for (int i = 0; hint.get_x_pos() + i < this->width; i++) {
      if (this->value[hint.get_x_pos() + i + hint.get_y_pos() * this->width] !=
          -1) {
        length += 1;
      } else {
        break;
      }
    }
  } else {
    for (int i = 0; hint.get_y_pos() + i < this->height; i++) {
      if (this->value[hint.get_x_pos() +
                      (hint.get_y_pos() + i) * this->width] != -1) {
        length += 1;
      } else {
        break;
      }
    }
  }

  if (length < 2) {
    throw std::logic_error(
        "Hint " + std::string(hint.get_is_horizontal() ? "a" : "d") +
        std::to_string(hint.get_identifier()) + " refers to a number with " +
        std::to_string(length) +
        " digit(s): all hints must refer to numbers with two or more digits");
  }

  hint.set_length(length);
  hint.run_program_on_load();
  hints.push_back(std::ref(hint));
}

bool ArrayCrossNumber::try_values(int arr[], int count) {
  for (int i = 0; i < count; i++) {
    if (is_possible_value(hints[i].get().possible_values[arr[i]], hints[i])) {
      set_value(hints[i].get().possible_values[arr[i]], hints[i]);
    } else {
      for (int j = 0; j <= i; j++) {
        clear_value(hints[j]);
      }

      return false;
    }
  }

  for (int i = 0; i < count; i++) {
    clear_value(hints[i]);
  }

  return true;
}

void ArrayCrossNumber::apply_values(int arr[], int count) {
  for (int i = 0; i < count; i++) {
    set_value(hints[i].get().possible_values[arr[i]], hints[i]);
  }
}

void ArrayCrossNumber::clear_values(int arr[], int count) {
  for (int i = 0; i < count; i++) {
    clear_value(hints[i]);
  }
}

bool ArrayCrossNumber::digit_shake() {
  std::vector<std::array<bool, 10>> tmp_digits(this->width * this->height,
                                               {1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

  for (Hint& h : this->hints) {
    std::vector<std::array<bool, 10>> tmp_array;
    for (int l = 0; l < h.get_length(); l++) {
      tmp_array.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    for (int n : h.possible_values) {
      for (int l = 0; l < h.get_length(); l++) {
        tmp_array[l][get_nth_digit(n, l, h.get_length())] = true;
      }
    }

    for (int l = 0; l < h.get_length(); l++) {
      if (h.get_is_horizontal()) {
        for (int i = 0; i < 10; i++) {
          tmp_digits[h.get_x_pos() + l + this->width * h.get_y_pos()][i] =
              tmp_digits[h.get_x_pos() + l + this->width * h.get_y_pos()][i] &&
              tmp_array[l][i];
        }
      } else {
        for (int i = 0; i < 10; i++) {
          tmp_digits[h.get_x_pos() + this->width * (h.get_y_pos() + l)][i] =
              tmp_digits[h.get_x_pos() + this->width * (h.get_y_pos() + l)]
                        [i] &&
              tmp_array[l][i];
        }
      }
    }
  }

  double init_product = 1.0;
  for (Hint& h : this->hints) {
    init_product *= h.number_possible_values;
  }
  std::cout << "possible combinations: " << init_product << std::endl;

  for (Hint& h : hints) {
    int length = h.get_length();
    for (int l = 0; l < h.get_length(); l++) {
      if (h.get_is_horizontal()) {
        for (int i = 0; i < 10; i++) {
          if (!tmp_digits[h.get_x_pos() + l + width * h.get_y_pos()][i]) {
            std::erase_if(h.possible_values, [=](int val) {
              return get_nth_digit(val, l, length) == i;
            });
            h.number_possible_values = h.possible_values.size();
          }
        }
      } else {
        for (int i = 0; i < 10; i++) {
          if (!tmp_digits[h.get_x_pos() + width * (h.get_y_pos() + l)][i]) {
            std::erase_if(h.possible_values, [=](int val) {
              return get_nth_digit(val, l, length) == i;
            });
            h.number_possible_values = h.possible_values.size();
          }
        }
      }
    }
  }

  double final_product = 1.0;
  for (Hint& h : hints) {
    final_product *= h.number_possible_values;
  }
  std::cout << "possible combinations: " << final_product << std::endl;

  return init_product == final_product;
}

int ArrayCrossNumber::count_digits(int n) {
  return std::count(value, value + width * height, n);
}

std::string ArrayCrossNumber::debug() {
  std::string output = "";
  for (int i = 0; i < MAX_SQUARE_COUNT; i++) {
    output = output + " " + std::to_string(value[i]);
  }
  return output;
}