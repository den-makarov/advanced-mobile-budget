#ifndef MONEYSTATE_H
#define MONEYSTATE_H

#include <iostream>
#include <iomanip>

struct MoneyState {
  double earned = 0.0;
  double spent = 0.0;

  /*double ComputeIncome() const {
    return earned - spent;
  }*/

  MoneyState& operator+=(const MoneyState& other) {
    earned += other.earned;
    spent += other.spent;
    return *this;
  }

  MoneyState operator+(const MoneyState& other) const {
    return {earned + other.earned, spent + other.spent};
  }

  operator double() {
    return earned - spent;
  }

  MoneyState operator*(double factor) const {
    return {earned * factor, spent};
  }

  MoneyState operator+(double amount) const {
    return {earned + amount, spent};
  }

  MoneyState operator-(double amount) const {
    return {earned, spent + amount};
  }
};

std::ostream& operator<<(std::ostream& stream, const MoneyState& state);

#endif // MONEYSTATE_H
