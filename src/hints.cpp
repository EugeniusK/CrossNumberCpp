#include "hints.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

#include "utils.h"

Hint::Hint(int identifier, int is_horizontal) {
  this->identifier = identifier;
  this->is_horizontal = is_horizontal;
}

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

class PublicHint : public Hint {
 public:
  PublicHint(int identifier, int x_pos, int y_pos, int length,
             bool is_horizontal, std::string full_description,
             std::string short_description)
      : Hint(identifier, is_horizontal),
        full_description(full_description),
        short_description(short_description) {}

 private:
  std::string full_description;
  std::string short_description;
};
