#ifndef CROSSNUMBER_H
#define CROSSNUMBER_H
#include <functional>
#include <string>
#include <vector>

// #include "digit.h"
#include "hint.h"
#include "utils.h"

class CrossNumber {
 public:
  // initialise crossnumber with specified width and height
  CrossNumber(int width, int height);

  virtual ~CrossNumber();

  // crossnumber structure is defined
  // 1~N for hint 1~N
  // 0 for empty square
  // -1 for invalid square where number cannot be placed
  virtual void define_squares(std::vector<int> squares) = 0;

  // returns text based interface that indicate
  // invalid squares as ███
  // hint locations as 1~N
  // empty squares as |   |
  virtual std::string display_layout() = 0;

  // returns text based interface that indicate
  // invalid squares as ███
  // values currently stored in squares as 0~9
  // empty squares as |   |
  virtual std::string display_value() = 0;

  // returns table with digits 0~9 and their counts, separated by |
  virtual std::string display_digit_count() = 0;

  // set of methods for adding guesses to the crossnumber
  // requires x_pos, y_pos, length, is_horizontal for specific location
  virtual void set_value(int val, int x_pos, int y_pos, int length,
                         bool is_horizontal) = 0;
  virtual int get_value(int x_pos, int y_pos, int length,
                        bool is_horizontal) = 0;
  virtual void clear_value(int x_pos, int y_pos, int length,
                           bool is_horizontal) = 0;
  virtual bool is_possible_value(int val, int x_pos, int y_pos, int length,
                                 bool is_horizontal) = 0;

  // set of methods for adding guesses to the crossnumber
  // uses the Hint to specify the location
  virtual void set_value(int val, Hint& hint) = 0;
  virtual int get_value(Hint& hint) = 0;
  virtual void clear_value(Hint& hint) = 0;
  virtual bool is_possible_value(int val, Hint& hint) = 0;

  // add new hint
  virtual void load_hint(Hint& hint) = 0;

  // based on guesses and the possible values stored inside of them, attempt
  // them
  virtual bool try_values(int arr[], int count) = 0;
  virtual void apply_values(int arr[], int count) = 0;
  virtual void clear_values(int arr[], int count) = 0;

  // based on the possible values stored inside the hints,
  // remove the values that aren't possible
  // returns true if no reduction made
  virtual bool digit_shake() = 0;
  // get how many times digit n has been used
  virtual int count_digits(int n) = 0;

  // hints that can be modified
  std::vector<std::reference_wrapper<Hint>> hints;
};

class ArrayCrossNumber : public CrossNumber {
 public:
  ArrayCrossNumber(int width, int height);
  void define_squares(std::vector<int> squares) override;
  std::string display_layout() override;
  std::string display_value() override;
  std::string display_digit_count() override;
  std::string debug();
  void set_value(int val, int x_pos, int y_pos, int length,
                 bool is_horizontal) override;
  int get_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  void clear_value(int x_pos, int y_pos, int length,
                   bool is_horizontal) override;
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
  int layout[MAX_SQUARE_COUNT];  // only describes layout, location of clues
  // Digit digits[MAX_SQUARE_COUNT];  // possible digits for each square
  int value[MAX_SQUARE_COUNT];  // value that board takes at the moment
};

#endif