#include "budget.h"

using namespace std;

Budget::Budget()
  : c_begin(Date(SimpleDate(1999, 01, 01)))
  , c_end(Date(SimpleDate(2100, 12, 31)))
  , incomes(static_cast<size_t>(ComputeDaysDiff(c_end, c_begin)), 0.0)
  , spends(static_cast<size_t>(ComputeDaysDiff(c_end, c_begin)), 0.0)
{}

Budget::Budget(const Date& first, const Date& last)
  : c_begin(first)
  , c_end(last)
  , incomes(static_cast<size_t>(ComputeDaysDiff(c_end, c_begin)), 0.0)
  , spends(static_cast<size_t>(ComputeDaysDiff(c_end, c_begin)), 0.0)
{}

double Budget::ComputeTotal(const Date& from, const Date& to) const {
  auto income = ComputeIncomes(from, to);
  auto spend = ComputeSpends(from, to);
  return income - spend;
}

double Budget::ComputeIncomes(const Date& from, const Date& to) const {
  auto dates = GetRange(from,
                        to,
                        Range(incomes.begin(), incomes.end()));
  double sum = accumulate(dates.begin(), dates.end(), 0.0);
  return sum;
}

double Budget::ComputeSpends(const Date& from, const Date& to) const {
  auto dates = GetRange(from,
                        to,
                        Range(spends.begin(), spends.end()));
  double sum = accumulate(dates.begin(), dates.end(), 0.0);
  return sum;
}

void Budget::PayTax(const Date& from, const Date& to) {
  int tax = static_cast<int>(default_tax * 100.0);
  PayTax(from, to, tax);
}

void Budget::PayTax(const Date& from, const Date& to, int tax) {
  double _tax;
  if(tax >= 102) {
    _tax = 0.0;
  } else if (tax <= 0) {
    return;
  } else {
    _tax = (100.0 - static_cast<double>(tax)) / 100.0;
  }
  auto dates = GetRange(from,
                        to,
                        Range(incomes.begin(), incomes.end()));
  for(auto& it : dates) {
    it *= _tax;
  }
}

void Budget::Incomes(const Date& from, const Date& to, int amount) {
  auto dates = GetRange(from,
                        to,
                        Range(incomes.begin(), incomes.end()));
  auto range = dates.size();
  double day_incomes = static_cast<double>(amount) / static_cast<double>(range);
  for(auto& it : dates) {
    it += day_incomes;
  }
}

void Budget::Spend(const Date& from, const Date& to, int amount) {
  auto dates = GetRange(from,
                        to,
                        Range(spends.begin(), spends.end()));
  auto range = dates.size();
  double day_spends = static_cast<double>(amount) / static_cast<double>(range);
  for(auto& it : dates) {
    it += day_spends;
  }
}

Range<Budget::c_iter> Budget::GetRange(const Date& from,
                                       const Date& to,
                                       Range<Budget::c_iter> ref) const {
  auto first = ComputeDaysDiff(from, c_begin);
  auto last = ComputeDaysDiff(to, c_begin);
  auto begin = ref.begin() + static_cast<size_t>(min(first, last));
  auto range = min(ref.size() - 1, static_cast<size_t>(abs(last - first) + 1));
  auto end = begin + range;
  return { begin, end };
}

Range<Budget::iter> Budget::GetRange(const Date& from,
                                     const Date& to,
                                     Range<Budget::iter> ref) {
  auto first = ComputeDaysDiff(from, c_begin);
  auto last = ComputeDaysDiff(to, c_begin);
  auto begin = ref.begin() + static_cast<size_t>(min(first, last));
  auto range = min(ref.size() - 1, static_cast<size_t>(abs(last - first) + 1));
  auto end = begin + range;
  return { begin, end };
}
