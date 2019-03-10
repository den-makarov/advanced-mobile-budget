#ifndef PAY_TAX_REQUEST_H
#define PAY_TAX_REQUEST_H

#include "request.h"

struct PayTaxRequest : ModifyRequest {
  PayTaxRequest(const Date& date)
    : ModifyRequest(Type::PAY_TAX)
    , start_date(date)
    , date_from(date)
    , date_to(date)
  {}
  void ParseFrom(std::string_view input) override;

  void Process(BudgetManager& manager) const override {
    manager.AddBulkOperation(MakeDateSegment(date_from, date_to, start_date),
                             BulkTaxApplier{(1.0 - static_cast<double>(percentage) / 100.0)});
    //    for (auto& money : manager.MakeDateRange(date_from, date_to)) {
    //      money.earned *= 1 - percentage / 100.0;
    //    }
  }

  const Date start_date;
  Date date_from;
  Date date_to;
  size_t percentage = 0;
};

#endif // PAY_TAX_REQUEST_H
