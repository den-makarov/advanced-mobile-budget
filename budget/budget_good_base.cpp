#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "range.h"
#include "stringhelper.h"
#include "dates.h"
#include "moneystate.h"
#include "indexsegment.h"
#include "bulkupdater.h"
#include "segmenttree.h"

using namespace std;

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = static_cast<size_t>(ComputeDaysDiff(END_DATE, START_DATE));

size_t ComputeDayIndex(const Date& date) {
  return static_cast<size_t>(ComputeDaysDiff(date, START_DATE));
}

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to) {
  return {ComputeDayIndex(date_from), ComputeDayIndex(date_to) + 1};
}

class BudgetManager : public SummingSegmentTree<MoneyState, BulkLinearUpdater<MoneyState>> {
public:
    BudgetManager() : SummingSegmentTree(DAY_COUNT) {}
//    auto MakeDateRange(const Date& date_from, const Date& date_to) const {
//      const auto segment = MakeDateSegment(date_from, date_to);
//      return Range(begin() + segment.left, begin() + segment.right);
//    }
};

struct Request;
using RequestHolder = unique_ptr<Request>;

struct Request {
  enum class Type {
    COMPUTE_INCOME,
    EARN,
    SPEND,
    PAY_TAX
  };

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(string_view input) = 0;
  virtual ~Request();
//  virtual ~Request() = default;

  const Type type;
};

Request::~Request() {}

const unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"ComputeIncome", Request::Type::COMPUTE_INCOME},
    {"Earn", Request::Type::EARN},
    {"Spend", Request::Type::SPEND},
    {"PayTax", Request::Type::PAY_TAX},
};

template <typename ResultType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const BudgetManager& manager) const = 0;
};

struct ModifyRequest : Request {
  using Request::Request;
  virtual void Process(BudgetManager& manager) const = 0;
};

struct ComputeIncomeRequest : ReadRequest<double> {
  ComputeIncomeRequest() : ReadRequest(Type::COMPUTE_INCOME) {}
  void ParseFrom(string_view input) override;

  double Process(const BudgetManager& manager) const override {
//    const auto range = manager.MakeDateRange(date_from, date_to);
//    return accumulate(begin(range), end(range), MoneyState{}).ComputeIncome();
    return manager.ComputeSum(MakeDateSegment(date_from, date_to));
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
};

void ComputeIncomeRequest::ParseFrom(string_view input) {
  date_from = Date::FromString(ReadToken(input));
  date_to = Date::FromString(input);
}

template <int SIGN>
struct AddMoneyRequest : ModifyRequest {
  static_assert(SIGN == -1 || SIGN == 1);
//struct EarnRequest : ModifyRequest {
//  EarnRequest() : ModifyRequest(Type::EARN) {}
  AddMoneyRequest() : ModifyRequest(SIGN == 1 ? Type::EARN : Type::SPEND) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    income = static_cast<size_t>(ConvertToInt(input));
  //  value = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
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

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t income = 0;
//  size_t value = 0;
};

struct PayTaxRequest : ModifyRequest {
  PayTaxRequest() : ModifyRequest(Type::PAY_TAX) {}
  void ParseFrom(string_view input) override;

  void Process(BudgetManager& manager) const override {
    manager.AddBulkOperation(MakeDateSegment(date_from, date_to), BulkTaxApplier{1});
//    for (auto& money : manager.MakeDateRange(date_from, date_to)) {
//      money.earned *= 1 - percentage / 100.0;
//    }
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  uint8_t percentage = 0;
};

void PayTaxRequest::ParseFrom(string_view input) {
  date_from = Date::FromString(ReadToken(input));
//  date_to = Date::FromString(input);
  date_to = Date::FromString(ReadToken(input));
  percentage = ConvertToInt(input);
}

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
    case Request::Type::COMPUTE_INCOME:
      return make_unique<ComputeIncomeRequest>();
    case Request::Type::EARN:
//      return make_unique<EarnRequest>();
      return make_unique<AddMoneyRequest<+1>>();
    case Request::Type::SPEND:
//      return nullptr;
      return make_unique<AddMoneyRequest<-1>>();
    case Request::Type::PAY_TAX:
      return make_unique<PayTaxRequest>();
  }
  return nullptr;
}

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
  Number number;
  stream >> number;
  string dummy;
  getline(stream, dummy);
  return number;
}

optional<Request::Type> ConvertRequestTypeFromString(string_view type_str) {
  if (const auto it = STR_TO_REQUEST_TYPE.find(type_str);
      it != STR_TO_REQUEST_TYPE.end()) {
    return it->second;
  } else {
    return nullopt;
  }
}

RequestHolder ParseRequest(string_view request_str) {
  const auto request_type = ConvertRequestTypeFromString(ReadToken(request_str));
  if (!request_type) {
    return nullptr;
  }
  RequestHolder request = Request::Create(*request_type);
  if (request) {
    request->ParseFrom(request_str);
  };
  return request;
}

vector<RequestHolder> ReadRequests(istream& in_stream = cin) {
  const size_t request_count = ReadNumberOnLine<size_t>(in_stream);

  vector<RequestHolder> requests;
  requests.reserve(request_count);

  for (size_t i = 0; i < request_count; ++i) {
    string request_str;
    getline(in_stream, request_str);
    if (auto request = ParseRequest(request_str)) {
      requests.push_back(move(request));
    }
  }
  return requests;
}

vector<double> ProcessRequests(const vector<RequestHolder>& requests) {
  vector<double> responses;
  BudgetManager manager;
  for (const auto& request_holder : requests) {
    if (request_holder->type == Request::Type::COMPUTE_INCOME) {
      const auto& request = static_cast<const ComputeIncomeRequest&>(*request_holder);
      responses.push_back(request.Process(manager));
    } else {
      const auto& request = static_cast<const ModifyRequest&>(*request_holder);
      request.Process(manager);
    }
  }
  return responses;
}

void PrintResponses(const vector<double>& responses, ostream& stream = cout) {
  for (const double response : responses) {
    stream << setprecision(10) << fixed << response << endl;
  }
}

int main() {
  const auto requests = ReadRequests();
  const auto responses = ProcessRequests(requests);
  PrintResponses(responses);
  return 0;
}
