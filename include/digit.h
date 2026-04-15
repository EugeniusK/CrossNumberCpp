#ifndef DIGIT_H
#define DIGIT_H
class Digit {
 public:
  Digit();
  bool getDigitValid(int n);
  void setDigitValid(int n);
  void setDigitInvalid(int n);
  void clearAll();
  void print();

 private:
  bool digit_store[10];
};
#endif