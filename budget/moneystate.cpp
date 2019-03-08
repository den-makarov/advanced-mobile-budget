#include "moneystate.h"

using namespace std;

ostream& operator<<(ostream& stream, const MoneyState& state) {
  return stream << "{+" << state.earned << ", -" << state.spent << "}";
}

