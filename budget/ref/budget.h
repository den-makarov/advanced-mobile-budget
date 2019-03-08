#ifndef BUDGET_H
#define BUDGET_H

#include "date.h"

#include <vector>
#include <optional>
#include <numeric>
#include <cmath>

class Budget
{
public:
  using iter = std::vector<double>::iterator;
  using c_iter = std::vector<double>::const_iterator;

  Budget();
  Budget(const Date& first, const Date& last);

  double operator[](const Date& date) const {
    return incomes[static_cast<size_t>(ComputeDaysDiff(date, c_begin))];
  }

  double& operator[](const Date& date) {
    return incomes[static_cast<size_t>(ComputeDaysDiff(date, c_begin))];
  }

  double ComputeIncomes(const Date& from, const Date& to) const;
  double ComputeTotal(const Date& from, const Date& to) const;
  void PayTax(const Date& from, const Date& to);
  void PayTax(const Date& from, const Date& to, int tax);
  void Incomes(const Date& from, const Date& to, int amount);
  void Spend(const Date& from, const Date& to, int amount);

private:
  Range<c_iter> GetRange(const Date& from, const Date& to, Range<Budget::c_iter> ref) const;
  Range<iter> GetRange(const Date& from, const Date& to, Range<Budget::iter> ref);
  double ComputeSpends(const Date& from, const Date& to) const;

private:
  const Date c_begin;
  const Date c_end;
  std::vector<double> incomes;
  std::vector<double> spends;
  const double default_tax = 0.13;
};

#endif // BUDGET_H
