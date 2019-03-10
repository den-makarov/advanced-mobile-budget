#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>

#include "range.h"
#include "stringhelper.h"
#include "dates.h"
#include "moneystate.h"
#include "indexsegment.h"
#include "bulkupdater.h"
#include "segmenttree.h"
#include "budgetmanager.h"
#include "request.h"
#include "compute_request.h"
#include "add_request.h"
#include "pay_tax_request.h"

using namespace std;

//static const Date START_DATE = Date::FromString("2001-02-01");
//static const Date END_DATE = Date::FromString("2001-02-09");
static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = static_cast<size_t>(ComputeDaysDiff(END_DATE, START_DATE));

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
  case Request::Type::COMPUTE_INCOME:
    return make_unique<ComputeIncomeRequest>(START_DATE);
  case Request::Type::EARN:
    //      return make_unique<EarnRequest>();
    return make_unique<AddMoneyRequest<+1>>(START_DATE);
  case Request::Type::SPEND:
    //      return nullptr;
    return make_unique<AddMoneyRequest<-1>>(START_DATE);
  case Request::Type::PAY_TAX:
    return make_unique<PayTaxRequest>(START_DATE);
  }
  return nullptr;
}

const unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
  {"ComputeIncome", Request::Type::COMPUTE_INCOME},
  {"Earn", Request::Type::EARN},
  {"Spend", Request::Type::SPEND},
  {"PayTax", Request::Type::PAY_TAX},
};

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
  BudgetManager manager(DAY_COUNT);
  for (const auto& request_holder : requests) {
    if (request_holder->type == Request::Type::COMPUTE_INCOME) {
      const auto& request = static_cast<const ComputeIncomeRequest&>(*request_holder);
      responses.push_back(request.Process(manager));
    } else {
      const auto& request = static_cast<const ModifyRequest&>(*request_holder);
      request.Process(manager);
//      cout << "Request:\n";
//      manager.operator<<(cout);
//      cout << "\n";
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
