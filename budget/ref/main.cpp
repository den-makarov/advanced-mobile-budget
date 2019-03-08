#include "date.h"
#include "budget.h"

using namespace std;

/**
 * @brief This is solution with variable tax support
 * and Spend requests parsing
 */

int main()
{
  int command_count;
  cin >> command_count;
  Budget budget;
  while(command_count--) {
    istream& fake_cin = cin;

    SimpleDate from;
    SimpleDate to;
    int amount;

    string command;
    fake_cin >> command;

    if(command == "PayTax") {
/* Pay tax request with givven amount */
      fake_cin >> from >> to >> amount;
      budget.PayTax(Date(from), Date(to), amount);
    } else if(command == "Earn") {
/* Earn request */
      SimpleDate from;
      SimpleDate to;
      fake_cin >> from >> to >> amount;
      budget.Incomes(Date(from), Date(to), amount);
    } else if(command == "Spend") {
/* Spend request */
      SimpleDate from;
      SimpleDate to;
      fake_cin >> from >> to >> amount;
      budget.Spend(Date(from), Date(to), amount);
    } else {
/* Calculate total amount request */
      SimpleDate from;
      SimpleDate to;
      fake_cin >> from >> to;
      auto total = budget.ComputeTotal(Date(from), Date(to));
      cout << setprecision(3) << fixed << total << '\n';
    }
  }
  return 0;
}
