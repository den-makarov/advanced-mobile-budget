#ifndef COMPUTE_REQUEST_H
#define COMPUTE_REQUEST_H

#include "request.h"

struct ComputeIncomeRequest : ReadRequest<double> {
  ComputeIncomeRequest(const Date& date) :
    ReadRequest(Type::COMPUTE_INCOME),
    start_date(date),
    date_from(date),
    date_to(date)
  {}

  void ParseFrom(std::string_view input) override;

  double Process(const BudgetManager& manager) const override {
    //    const auto range = manager.MakeDateRange(date_from, date_to);
    //    return accumulate(begin(range), end(range), MoneyState{}).ComputeIncome();
    return manager.ComputeSum(MakeDateSegment(date_from, date_to, start_date));
  }
  const Date start_date;
  Date date_from;
  Date date_to;
};
#endif // COMPUTE_REQUEST_H
