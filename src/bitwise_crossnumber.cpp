#include "crossnumber.h"

#include <algorithm>
#include <array>
#include <vector>


BitwiseCrossNumber::BitwiseCrossNumber(int width, int height) : CrossNumber(width, height) {
  this->width = width;
  this->height = height;
  std::fill(std::begin(layout), std::end(layout), -2);
  if (width * height > MAX_SQUARE_COUNT) {
    std::string error_message_size =
        "Size of board cannot exceed " + std::to_string(MAX_SQUARE_COUNT);
    throw std::invalid_argument(error_message_size);
  }
};

void BitwiseCrossNumber::define_squares(std::vector<int> squares) {
  // layout - 1~N for hint on board
  // layout - 0 for valid square without hint
  // layout - -1 for invalid square

  // value - unassigned for valid square
  // value - -1 for invalid square
  for (int i = 0; i < squares.size(); i++) {
    layout[i] = squares[i];
    value[i] = -2;
    if (squares[i] != -1) {
      digits[i] = Digit();
    } else {
      value[i] = -1;
    }
  }
}
std::string BitwiseCrossNumber::display_layout() {
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
std::string BitwiseCrossNumber::display_value() {
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

std::string BitwiseCrossNumber::display_digit_count() {
  std::string output = "";
  for (int i = 0; i<10; i++) {
    output += std::to_string(std::count(std::begin(this->value), std::end(this->value), i));
    output += " | ";
  }

  return output;
}

void BitwiseCrossNumber::set_value(int val, int x_pos, int y_pos, int length,
                            bool is_horizontal) {
  if (is_horizontal) {
    for (int i = 0; i < length; i++) {
      value[y_pos * width + (x_pos + i)] =
          (val / ipow(10, length - i - 1)) % 10;
    }
  } else {
    for (int i = 0; i < length; i++) {
      value[(y_pos + i) * width + x_pos] =
          (val / ipow(10, length - i - 1)) % 10;
    }
  }
}
int BitwiseCrossNumber::get_value(int x_pos, int y_pos, int length,
                           bool is_horizontal) {
  int sum = 0;
  if (is_horizontal) {
    for (int i = 0; i < length; i++) {
      sum += value[y_pos * width + (x_pos + i)] * ipow(10, length - i - 1);
    }
  } else {
    for (int i = 0; i < length; i++) {
      sum += value[(y_pos + i) * width + x_pos] * ipow(10, length - i - 1);
    }
  }
  return sum;
}
void BitwiseCrossNumber::clear_value(int x_pos, int y_pos, int length,
                              bool is_horizontal) {
  if (is_horizontal) {
    for (int i = 0; i < length; i++) {
      value[y_pos * width + (x_pos + i)] = -2;
    }
  } else {
    for (int i = 0; i < length; i++) {
      value[(y_pos + i) * width + x_pos] = -2;
    }
  }
}
bool BitwiseCrossNumber::is_possible_value(int val, int x_pos, int y_pos, int length,
                                    bool is_horizontal) {
  bool valid = true;
  if (is_horizontal) {
    for (int i = 0; i < length; i++) {
      int idx = y_pos * width + (x_pos + i);
      int digit = (val / ipow(10, length - i - 1)) % 10;
      valid = valid && (value[idx] == digit || value[idx] == -2);
    }
  } else {
    for (int i = 0; i < length; i++) {
      int idx = (y_pos + i) * width + x_pos;
      int digit = (val / ipow(10, length - i - 1)) % 10;
      valid = valid && (value[idx] == digit || value[idx] == -2);
    }
  }
  return valid;
}

void BitwiseCrossNumber::set_value(int val, Hint& hint) {
  set_value(val, hint.x_pos, hint.y_pos, hint.length, hint.is_horizontal);
}
int BitwiseCrossNumber::get_value(Hint& hint) {
  return get_value(hint.x_pos, hint.y_pos, hint.length, hint.is_horizontal);
}
void BitwiseCrossNumber::clear_value(Hint& hint) {
  clear_value(hint.x_pos, hint.y_pos, hint.length, hint.is_horizontal);
}
bool BitwiseCrossNumber::is_possible_value(int val, Hint& hint) {
  return is_possible_value(val, hint.x_pos, hint.y_pos, hint.length,
                           hint.is_horizontal);
}

void BitwiseCrossNumber::load_hint(Hint& hint) {
  bool found = false;
  for (int i = 0; i < this->width * this->height; i++) {
    if (layout[i] == hint.identifier) {
      found = true;
      hint.x_pos = i % width;
      hint.y_pos = i / width;
      break;
    }
  }

  if (found == false) {
    throw std::logic_error("Hint not in layout initially defined");
  }

  int length = 0;

  for (int i = 0; i < this->height; i++) {
    if (hint.is_horizontal) {
      if (this->value[hint.x_pos + i + hint.y_pos * this->width] != -1 &&
          hint.x_pos + i < this->width) {
        length += 1;
      } else {
        break;
      }
    } else {
      if (this->value[hint.x_pos + (hint.y_pos + i) * this->width] != -1 &&
          hint.y_pos + i < this->height) {
        length += 1;

      } else {
        break;
      }
    }
  }

  hint.length = length;
  hint.possible_values = {};
  for (int i = ipow(10, length - 1); i < ipow(10, length); i++) {
    hint.possible_values.push_back(i);
  }
  hint.number_possible_values = hint.possible_values.size();

  hints.push_back(std::ref(hint));
}

bool BitwiseCrossNumber::try_values(int arr[], int count) {
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

void BitwiseCrossNumber::apply_values(int arr[], int count) {
  for (int i = 0; i < count; i++) {
    set_value(hints[i].get().possible_values[arr[i]], hints[i]);
  }
}

void BitwiseCrossNumber::clear_values(int arr[], int count) {
  for (int i = 0; i < count; i++) {
    clear_value(hints[i]);
  }
}

bool BitwiseCrossNumber::digit_shake() {
  std::vector<std::array<bool, 10>> tmp_digits(this->width * this->height,
                                               {1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

  for (Hint& h : this->hints) {
    std::vector<std::array<bool, 10>> tmp_array;
    for (int l = 0; l < h.length; l++) {
      tmp_array.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    for (int n : h.possible_values) {
      for (int l = 0; l < h.length; l++) {
        tmp_array[l][get_nth_digit(n, l, h.length)] = true;
      }
    }

    for (int l = 0; l < h.length; l++) {
      if (h.is_horizontal) {
        for (int i = 0; i < 10; i++) {
          tmp_digits[h.x_pos + l + this->width * h.y_pos][i] =
              tmp_digits[h.x_pos + l + this->width * h.y_pos][i] &&
              tmp_array[l][i];
        }
      } else {
        for (int i = 0; i < 10; i++) {
          tmp_digits[h.x_pos + this->width * (h.y_pos + l)][i] =
              tmp_digits[h.x_pos + this->width * (h.y_pos + l)][i] &&
              tmp_array[l][i];
        }
      }
    }
  }

  double init_product = 1.0;
  for (Hint& h: this ->hints) {
    init_product *= h.number_possible_values;
  }
    // std::cout << "possible combinations: " << init_product << std::endl;

  for (Hint& h : this->hints) {
    int length = h.length;
    for (int l = 0; l < h.length; l++) {
      if (h.is_horizontal) {
        for (int i = 0; i < 10; i++) {
          if (!tmp_digits[h.x_pos + l + this->width * h.y_pos][i]) {
            h.possible_values.erase(
                std::remove_if(h.possible_values.begin(),
                               h.possible_values.end(),
                               [=](int val) {
                                 return get_nth_digit(val, l, length) == i;
                               }),
                h.possible_values.end());
            h.number_possible_values = h.possible_values.size();
          }
        }
      } else {
        for (int i = 0; i < 10; i++) {
          if (!tmp_digits[h.x_pos + this->width * (h.y_pos + l)][i]) {
            h.possible_values.erase(
                std::remove_if(h.possible_values.begin(),
                               h.possible_values.end(),
                               [=](int val) {
                                 return get_nth_digit(val, l, length) == i;
                               }),
                h.possible_values.end());
            h.number_possible_values = h.possible_values.size();
          }
        }
      }
    }
  }

  double final_product = 1.0;
  for (Hint& h: this ->hints) {
    final_product *= h.number_possible_values;
  }
  // std::cout << "reduced from " << log10(init_product) << " to " << log10(final_product) << std::endl;
  return init_product == final_product;

}

int BitwiseCrossNumber::count_digits(int n) {
  int c = 0;
  for (int i = 0; i < MAX_SQUARE_COUNT; i++) {
    if (value[i] == n) {
      c++;
    }
  }
  return c;
}