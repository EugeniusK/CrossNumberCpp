#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "crossnumber.h"

ArrayCrossNumber::ArrayCrossNumber(int width, int height)
    : CrossNumber(width, height),
      width(width),
      height(height),
      layout(width * height, -2),
      value(width * height, -1) {}

void ArrayCrossNumber::define_squares(std::vector<int> squares) {
  for (size_t i = 0; i < squares.size(); ++i) {
    layout[i] = squares[i];
    if (squares[i] != -1) {
      value[i] = -2;
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

bool ArrayCrossNumber::try_values(const std::vector<int>& arr, int count) {
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

void ArrayCrossNumber::apply_values(const std::vector<int>& arr, int count) {
  for (int i = 0; i < count; i++) {
    set_value(hints[i].get().possible_values[arr[i]], hints[i]);
  }
}

void ArrayCrossNumber::clear_values(const std::vector<int>& arr, int count) {
  for (int i = 0; i < count; i++) {
    clear_value(hints[i]);
  }
}

// TODO [Review Later]: Review digit_shake() for:
// 1. Floating-point precision loss and overflow in init_product ==
// final_product.
// 2. Replacing double product comparison with exact bool changed tracking.
// 3. In-place filtering to avoid repeated std::erase_if per digit.
bool ArrayCrossNumber::digit_shake() {
  // global tmp_digits that tracks digits 0~9 for all squares in puzzle
  // initialise as all digits being used
  std::vector<std::array<bool, 10>> tmp_digits(this->width * this->height,
                                               {1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

  for (Hint& h : this->hints) {
    // for all hints, initialise local tmp_array to keep track of whether digits
    // are used
    std::vector<std::array<bool, 10>> tmp_array;
    for (int l = 0; l < h.get_length(); l++) {
      tmp_array.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }

    // populate tmp_array with digits used by the hint
    for (int n : h.possible_values) {
      for (int l = 0; l < h.get_length(); l++) {
        tmp_array[l][get_nth_digit(n, l, h.get_length())] = true;
      }
    }

    // update global tmp_digits
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

bool ArrayCrossNumber::digit_shake_with_dependencies() {
  std::vector<size_t> tmp_arr;
  bool any_overall_change = false;
  int pass_number = 1;

  // Helper lambda: computes the size of the Cartesian product of all dependency
  // candidate lists for hint 'h'. For example, if clue h depends on clue A (5
  // values) and clue B (10 values), total combinations = 5 * 10 = 50.
  auto compute_combinations = [this](Hint& h) -> long long {
    long long total = 1;
    for (const std::string& s : h.get_dependencies()) {
      if (s.empty()) continue;
      if (s[0] == 'c') {
        // 'c0'..'c9' are digit counts (can range from 0 up to total grid
        // squares)
        total *= (this->width * this->height + 1);
      } else {
        total *= this->get_hint(s).number_possible_values;
      }
    }
    return total;
  };

  // =========================================================================
  // MULTI-PASS CONVERGENCE LOOP
  // When a clue's candidate set is reduced, downstream clues (or cyclically
  // dependent clues) can now be evaluated with fewer dependency combinations or
  // might have some of their own candidates eliminated. We repeat full passes
  // until a pass makes no changes.
  // =========================================================================
  while (true) {
    bool pass_changed = false;
    std::cout << "\n========================================" << std::endl;
    std::cout << "--- Dependency Shake Pass " << pass_number << " ---"
              << std::endl;
    std::cout << "========================================" << std::endl;

    // STEP 1: Collect all hints that declare dependencies on other
    // clues/variables
    std::vector<std::reference_wrapper<Hint>> remaining_hints;
    for (Hint& h : this->hints) {
      if (h.get_if_has_dependencies()) {
        remaining_hints.push_back(h);
      }
    }

    // Process all dependent hints in this pass
    while (!remaining_hints.empty()) {
      // STEP 2: Greedy Ordering
      // Pick the hint with the smallest combination count across its
      // dependencies. Evaluating smaller search spaces first shrinks candidate
      // sets early, which in turn drastically reduces combination counts for
      // subsequent dependent clues.
      auto best_it = std::min_element(
          remaining_hints.begin(), remaining_hints.end(),
          [&](Hint& a, Hint& b) {
            return compute_combinations(a) < compute_combinations(b);
          });

      Hint& h = best_it->get();
      remaining_hints.erase(best_it);

      std::string clue_name = (h.get_is_horizontal() ? "a" : "d") +
                              std::to_string(h.get_identifier());
      long long combinations_count = compute_combinations(h);
      int old_possible_count = h.number_possible_values;

      std::cout << "\nEvaluating " << clue_name << ":"
                << " current candidates = " << old_possible_count
                << ", dependency combinations = " << combinations_count
                << std::endl;

      // STEP 3: Gather dependency lists and candidate values
      std::vector<std::string> dependencies = h.get_dependencies();
      std::vector<int> dependencies_number_possible_values;
      std::vector<std::vector<int>> dependencies_possible_values;
      long long total_combinations = 1LL;
      bool has_empty_dependency = false;

      std::cout << "  Dependencies: ";
      for (const std::string& s : dependencies) {
        if (s.empty()) continue;
        if (s[0] == 'c') {
          // Digit count dependency: candidate values 0, 1, ..., width * height
          std::vector<int> c_vals(this->width * this->height + 1);
          std::iota(c_vals.begin(), c_vals.end(), 0);
          dependencies_number_possible_values.push_back(
              static_cast<int>(c_vals.size()));
          dependencies_possible_values.push_back(std::move(c_vals));
          std::cout << s << " (count var, " << (this->width * this->height + 1)
                    << " values) ";
          total_combinations *= (this->width * this->height + 1);
        } else {
          Hint& dep = get_hint(s);
          if (dep.number_possible_values == 0) {
            has_empty_dependency = true;
          }
          dependencies_number_possible_values.push_back(
              dep.number_possible_values);
          dependencies_possible_values.push_back(dep.possible_values);
          std::cout << s << " (" << dep.number_possible_values << " values) ";
          total_combinations *= dep.number_possible_values;
        }
      }
      std::cout << "=> total combinations: " << total_combinations << std::endl;

      // STEP 4: Check for dead-ends / empty domains
      // If any dependency has 0 candidates, no valid assignments exist for this
      // clue.
      if (has_empty_dependency || total_combinations == 0) {
        if (!h.possible_values.empty()) {
          pass_changed = true;
          any_overall_change = true;
        }
        h.possible_values.clear();
        h.number_possible_values = 0;
        std::cout << "  " << clue_name
                  << " reduced to 0 (dependency domain is empty)" << std::endl;
        continue;
      }

      // STEP 5: Prepare candidate tracking and valid digit-length bounds
      // A number of length L must fall within [10^(L-1), 10^L) with no leading
      // zero.
      const size_t dependencies_size = dependencies.size();
      const int min_val = ipow(10, h.get_length() - 1);
      const int max_val = ipow(10, h.get_length());

      // 'seen[x]' tracks values already confirmed in updated_possible_values to
      // prevent duplicates
      std::vector<bool> seen(max_val, false);

      // 'is_currently_possible[x]' stores the clue's existing valid candidate
      // set. Dependency evaluation can only KEEP existing valid values, never
      // re-add previously pruned ones.
      std::vector<bool> is_currently_possible(max_val, false);
      for (int v : h.possible_values) {
        if (v >= 0 && v < max_val) {
          is_currently_possible[v] = true;
        }
      }
      std::vector<int> updated_possible_values;

      // STEP 6: Iterate through every Cartesian combination of dependency
      // values
      for (size_t i = 0; i < static_cast<size_t>(total_combinations); i++) {
        // Mixed-radix index decomposition: maps linear index 'i' into
        // coordinate indices (tmp_arr[0], tmp_arr[1], ...) across each
        // dependency's value list.
        size_t tmp = i;
        tmp_arr.clear();
        for (size_t j = 0; j < dependencies_size; j++) {
          size_t element_index = tmp % dependencies_number_possible_values[j];
          tmp /= dependencies_number_possible_values[j];
          tmp_arr.push_back(element_index);
        }

        // Set the chosen dependency values in the Hint environment
        for (size_t j = 0; j < tmp_arr.size(); j++) {
          h.set_env(dependencies[j],
                    dependencies_possible_values[j][tmp_arr[j]]);
        }

        // Execute the clue's AST script with the current dependency assignment
        h.run_program_on_dependency();

        // STEP 7: Harvest output values generated by the clue script
        for (int idx : h.get_written_output_indices()) {
          // Verify valid length range, non-zero output, membership in existing
          // domain, and de-duplicate
          if (idx >= min_val && idx < max_val) {
            if (h.get_output_array(idx) != 0 && is_currently_possible[idx] &&
                !seen[idx]) {
              seen[idx] = true;
              updated_possible_values.push_back(idx);
            }
          }
        }

        // EARLY EXIT OPTIMIZATION:
        // If all existing possible values have already been confirmed by at
        // least one dependency combination, no further values can possibly be
        // added or eliminated.
        if (updated_possible_values.size() == h.possible_values.size()) {
          break;
        }
      }

      // STEP 8: Finalize the pruned possible values list
      std::sort(updated_possible_values.begin(), updated_possible_values.end());
      if (updated_possible_values.size() != h.possible_values.size()) {
        pass_changed = true;
        any_overall_change = true;
      }
      h.possible_values = std::move(updated_possible_values);
      h.number_possible_values = h.possible_values.size();

      std::cout << "  " << clue_name << " reduced from " << old_possible_count
                << " to " << h.number_possible_values << " candidates";
      if (h.number_possible_values <= 10) {
        std::cout << " [";
        for (size_t k = 0; k < h.possible_values.size(); k++) {
          std::cout << h.possible_values[k]
                    << (k + 1 < h.possible_values.size() ? ", " : "");
        }
        std::cout << "]";
      }
      std::cout << std::endl;
      // digit_shake();
    }

    // If no clues were reduced during this entire pass, we have reached
    // convergence
    if (!pass_changed) {
      std::cout
          << "\nConvergence reached: no further reductions achieved in pass "
          << pass_number << "." << std::endl;
      break;
    }

    pass_number++;
  }
  std::cout << "digit_shake_with_dependencies completed successfully."
            << std::endl;

  return !any_overall_change;
}

int ArrayCrossNumber::count_digits(int n) {
  return std::count(value.begin(), value.end(), n);
}

std::string ArrayCrossNumber::debug() {
  std::string output = "";
  for (size_t i = 0; i < value.size(); i++) {
    output = output + " " + std::to_string(value[i]);
  }
  return output;
}

Hint& ArrayCrossNumber::get_hint(const std::string& identifier) const {
  if (identifier.size() < 2) {
    throw std::invalid_argument("Invalid hint identifier: " + identifier);
  }

  char type = static_cast<char>(
      std::tolower(static_cast<unsigned char>(identifier[0])));
  if (type != 'a' && type != 'd') {
    throw std::invalid_argument(
        "Invalid hint identifier prefix (must start with 'a' or 'd'): " +
        identifier);
  }

  int id = 0;
  try {
    id = std::stoi(identifier.substr(1));
  } catch (const std::exception&) {
    throw std::invalid_argument("Invalid hint identifier number: " +
                                identifier);
  }

  bool is_hor = (type == 'a');
  for (Hint& h : this->hints) {
    if (h.get_identifier() == id && h.get_is_horizontal() == is_hor) {
      return h;
    }
  }

  throw std::out_of_range("Hint not found: " + identifier);
}
