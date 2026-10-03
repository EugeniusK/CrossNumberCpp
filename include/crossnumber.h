#ifndef CROSSNUMBER_H
#define CROSSNUMBER_H
#include <functional>
#include <span>
#include <string>
#include <vector>

#include "builtin.h"
#include "hint.h"

struct LocalIntersection {
  int other_hint_idx;  // Index into this->hints for the earlier hint (j < i)
  int pos_in_self;     // 0-based digit index along hint i (from MSB/left)
  int pos_in_other;    // 0-based digit index along hint j (from MSB/left)
};

struct HintIntersections {
  LocalIntersection items[12];
  uint8_t count = 0;

  inline const LocalIntersection* data() const noexcept { return items; }
};

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
                         bool is_horizontal) noexcept = 0;
  virtual int get_value(int x_pos, int y_pos, int length,
                        bool is_horizontal) = 0;
  virtual void clear_value(int x_pos, int y_pos, int length,
                           bool is_horizontal) noexcept = 0;
  virtual bool is_possible_value(int val, int x_pos, int y_pos, int length,
                                 bool is_horizontal) const noexcept = 0;

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
  virtual bool try_values(const std::vector<int>& arr, int count) const noexcept = 0;
  virtual void apply_values(const std::vector<int>& arr, int count) = 0;
  virtual void clear_values(const std::vector<int>& arr, int count) = 0;

  virtual void init_intersections() {}

  // based on the possible values stored inside the hints,
  // remove the values that aren't possible
  // returns true if no reduction made
  virtual bool digit_shake() = 0;
  virtual bool digit_shake_with_dependencies() = 0;

  // get how many times digit n has been used
  virtual int count_digits(int n) = 0;

  // get hint by identifier (e.g. "a1", "d9")
  // virtual Hint& get_hint(const std::string& identifier) = 0;
  virtual Hint& get_hint(const std::string& identifier) const = 0;

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
                 bool is_horizontal) noexcept override;
  int get_value(int x_pos, int y_pos, int length, bool is_horizontal) override;
  void clear_value(int x_pos, int y_pos, int length,
                   bool is_horizontal) noexcept override;
  bool is_possible_value(int val, int x_pos, int y_pos, int length,
                         bool is_horizontal) const noexcept override;
  void set_value(int val, Hint& hint) override;
  int get_value(Hint& hint) override;
  void clear_value(Hint& hint) override;
  bool is_possible_value(int val, Hint& hint) override;
  void load_hint(Hint& hint) override;
  void init_intersections() override;
  bool try_values(std::span<const int> arr, int count) const noexcept;
  bool try_values(const std::vector<int>& arr, int count) const noexcept override;
  void apply_values(const std::vector<int>& arr, int count) override;
  void clear_values(const std::vector<int>& arr, int count) override;

  bool digit_shake() override;
  bool digit_shake_with_dependencies() override;

  int count_digits(int n) override;

  // Hint& get_hint(const std::string& identifier) override;
  Hint& get_hint(const std::string& identifier) const override;

 private:
  int width;
  int height;
  std::vector<int> layout;  // only describes layout, location of clues
  std::vector<int> value;   // value that board takes at the moment
  std::vector<HintIntersections> hint_intersections_;
};

#endif