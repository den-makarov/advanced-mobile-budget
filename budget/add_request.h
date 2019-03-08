#ifndef ADD_REQUEST_H
#define ADD_REQUEST_H

#include "request.h"

template <int SIGN>
struct AddMoneyRequest : ModifyRequest {
  static_assert(SIGN == -1 || SIGN == 1);
  //struct EarnRequest : ModifyRequest {
  //  EarnRequest() : ModifyRequest(Type::EARN) {}
  AddMoneyRequest(const Date& date)
    : ModifyRequest(SIGN == 1 ? Type::EARN : Type::SPEND)
    , start_date(date)
    , date_from(date)
    , date_to(date)
  {}

  void ParseFrom(std::string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    income = static_cast<size_t>(ConvertToInt(input));
    //  value = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to, start_date);
    const double daily_income = income * 1.0 / date_segment.length();
    manager.AddBulkOperation(date_segment, BulkMoneyAdder{daily_income});
    //    const auto date_range = manager.MakeDateRange(date_from, date_to);
    //    const double daily_value = value * 1.0 / size(date_range);
    //    const MoneyState daily_change =
    //        SIGN == 1
    //            ? MoneyState{.earned = daily_value}
    //            : MoneyState{.spent = daily_value};
    //    for (auto& money : date_range) {
    //      money += daily_change;
    //    }
  }

  const Date start_date;
  Date date_from;
  Date date_to;
  size_t income = 0;
  //  size_t value = 0;
};

#endif // ADD_REQUEST_H
