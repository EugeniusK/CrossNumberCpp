#ifndef CROSSNUMBER_H
#define CROSSNUMBER_H
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "backtrackstack.h"
#include "digit.h"
#include "hints.h"
#include "utils.h"

class CrossNumber {
 public:
  CrossNumber(int width, int height);
  virtual ~CrossNumber();
  virtual void define_squares(std::vector<int> squares) = 0;
  virtual std::string display_layout() = 0;
  virtual std::string display_value() = 0;
  virtual std::string display_digit_count() = 0;
  virtual void set_value(int val, int x_pos, int y_pos, int length, bool is_horizontal) = 0;
  virtual int get_value(int x_pos, int y_pos, int length, bool is_horizontal) = 0;
  virtual void clear_value(int x_pos, int y_pos, int length, bool is_horizontal) = 0;
  virtual bool is_possible_value(int val, int x_pos, int y_pos, int length,
                         bool is_horizontal) = 0;
  virtual void set_value(int val, Hint& hint) = 0;
  virtual int get_value(Hint& hint) = 0;
  virtual void clear_value(Hint& hint) = 0;
  virtual bool is_possible_value(int val, Hint& hint) = 0;
  virtual void load_hint(Hint& hint) = 0;
  virtual bool try_values(int arr[], int count) = 0;
  virtual void apply_values(int arr[], int count) = 0;
  virtual void clear_values(int arr[], int count) = 0;

  virtual bool digit_shake() = 0;
  virtual int count_digits(int n) = 0;

  std::vector<std::reference_wrapper<Hint>> hints;

};

class ArrayCrossNumber : public CrossNumber {
 public:
  ArrayCrossNumber(int width, int height);
  void define_squares(std::vector<int> squares) override;
  std::string display_layout() override;
  std::string display_value() override;
  std::string display_digit_count() override;
  void set_value(int val, int x_pos, int y_pos, int length, bool is_horizontal) override;
  int get_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  void clear_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  bool is_possible_value(int val, int x_pos, int y_pos, int length,
                         bool is_horizontal) override;
  void set_value(int val, Hint& hint) override;
  int get_value(Hint& hint) override;
  void clear_value(Hint& hint) override;
  bool is_possible_value(int val, Hint& hint) override;
  void load_hint(Hint& hint) override;
  bool try_values(int arr[], int count) override;
  void apply_values(int arr[], int count) override;
  void clear_values(int arr[], int count) override;

  bool digit_shake() override;
  int count_digits(int n) override;

 private:
  int width;
  int height;
  int layout[MAX_SQUARE_COUNT];    // only describes layout, location of clues
  Digit digits[MAX_SQUARE_COUNT];  // possible digits for each square
  int value[MAX_SQUARE_COUNT];     // value that board takes at the moment
};


class BitwiseCrossNumber : public CrossNumber {
 public:
  BitwiseCrossNumber(int width, int height);
  void define_squares(std::vector<int> squares) override;
  std::string display_layout() override;
  std::string display_value() override;
  std::string display_digit_count() override;
  void set_value(int val, int x_pos, int y_pos, int length, bool is_horizontal) override;
  int get_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  void clear_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  bool is_possible_value(int val, int x_pos, int y_pos, int length,
                         bool is_horizontal) override;
  void set_value(int val, Hint& hint) override;
  int get_value(Hint& hint) override;
  void clear_value(Hint& hint) override;
  bool is_possible_value(int val, Hint& hint) override;
  void load_hint(Hint& hint) override;
  bool try_values(int arr[], int count) override;
  void apply_values(int arr[], int count) override;
  void clear_values(int arr[], int count) override;

  bool digit_shake() override;
  int count_digits(int n) override;

 private:
  int width;
  int height;
  int layout[MAX_SQUARE_COUNT];    // only describes layout, location of clues
  Digit digits[MAX_SQUARE_COUNT];  // possible digits for each square
  int value[MAX_SQUARE_COUNT];     // value that board takes at the moment
};




#endif