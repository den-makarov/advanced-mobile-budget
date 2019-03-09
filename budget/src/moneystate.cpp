#include "moneystate.h"

using namespace std;

ostream& operator<<(ostream& stream, const MoneyState& state) {
  return stream << "{+" << std::setw(7) << state.earned
                << ", -" << std::setw(7) << state.spent << "} ";
}

