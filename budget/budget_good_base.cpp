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

using namespace std;

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = static_cast<size_t>(ComputeDaysDiff(END_DATE, START_DATE));

struct MoneyState {
  double earned = 0.0;
  double spent = 0.0;

  double ComputeIncome() const {
    return earned - spent;
  }

  MoneyState& operator+=(const MoneyState& other) {
    earned += other.earned;
    spent += other.spent;
    return *this;
  }

  MoneyState operator+(const MoneyState& other) const {
    return MoneyState(*this) += other;
  }

  operator double() {
    return earned + spent;
  }

  MoneyState operator*(double factor) const {
    return {earned * factor, spent * factor};
  }
};

ostream& operator<<(ostream& stream, const MoneyState& state) {
  return stream << "{+" << state.earned << ", -" << state.spent << "}";
}

struct IndexSegment {
  size_t left;
  size_t right;

  size_t length() const {
    return right - left;
  }
  bool empty() const {
    return length() == 0;
  }

  bool Contains(IndexSegment other) const {
    return left <= other.left && other.right <= right;
  }
};

IndexSegment IntersectSegments(IndexSegment lhs, IndexSegment rhs) {
  const size_t left = max(lhs.left, rhs.left);
  const size_t right = min(lhs.right, rhs.right);
  return {left, max(left, right)};
}

bool AreSegmentsIntersected(IndexSegment lhs, IndexSegment rhs) {
  return !(lhs.right <= rhs.left || rhs.right <= lhs.left);
}

struct BulkMoneyAdder {
  double delta = 0.0;
};

constexpr uint8_t TAX_PERCENTAGE = 13;

struct BulkTaxApplier {
  static constexpr double FACTOR = 1.0 - TAX_PERCENTAGE / 100.0;
  uint32_t count = 0;

  double ComputeFactor() const {
    return pow(FACTOR, count);
  }
};

template <typename Data>
class BulkLinearUpdater {
public:
  BulkLinearUpdater() = default;

  BulkLinearUpdater(const BulkMoneyAdder& add)
      : add_(add)
  {}

  BulkLinearUpdater(const BulkTaxApplier& tax)
      : tax_(tax)
  {}

  void CombineWith(const BulkLinearUpdater& other) {
    tax_.count += other.tax_.count;
    add_.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
  }

  Data Collapse(Data origin, IndexSegment segment) const {
    return origin * tax_.ComputeFactor() + add_.delta * segment.length();
  }

private:
  // apply tax first, then add
  BulkTaxApplier tax_;
  BulkMoneyAdder add_;
};


template <typename Data, typename BulkOperation>
class SummingSegmentTree {
public:
  SummingSegmentTree(size_t size) : root_(Build({0, size})) {}

  Data ComputeSum(IndexSegment segment) const {
    return this->TraverseWithQuery(root_, segment, ComputeSumVisitor{});
  }

  void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
    this->TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
  }

private:
  struct Node;
  using NodeHolder = unique_ptr<Node>;

  struct Node {
    NodeHolder left;
    NodeHolder right;
    IndexSegment segment;
    Data data;
    BulkOperation postponed_bulk_operation;
  };

  NodeHolder root_;

  static NodeHolder Build(IndexSegment segment) {
    if (segment.empty()) {
      return nullptr;
    } else if (segment.length() == 1) {
      return make_unique<Node>(Node{nullptr, nullptr, segment, {}, {}});
//      return make_unique<Node>(Node{
//        .left = nullptr,
//        .right = nullptr,
//        .segment = segment,
//      });
    } else {
      const size_t middle = segment.left + segment.length() / 2;
      return make_unique<Node>(Node{Build({segment.left, middle}),
                                    Build({middle, segment.right}),
                                    segment,
                                    {},
                                    {}});
//      return make_unique<Node>(Node{
//        .left = Build({segment.left, middle}),
//        .right = Build({middle, segment.right}),
//        .segment = segment,
//      });
    }
  }

  template <typename Visitor>
  static typename Visitor::ResultType TraverseWithQuery(const NodeHolder& node, IndexSegment query_segment, Visitor visitor) {
    if (!node || !AreSegmentsIntersected(node->segment, query_segment)) {
      return visitor.ProcessEmpty(node);
    } else {
      PropagateBulkOperation(node);
      if (query_segment.Contains(node->segment)) {
        return visitor.ProcessFull(node);
      } else {
        if constexpr (is_void_v<typename Visitor::ResultType>) {
          TraverseWithQuery(node->left, query_segment, visitor);
          TraverseWithQuery(node->right, query_segment, visitor);
          return visitor.ProcessPartial(node, query_segment);
        } else {
          return visitor.ProcessPartial(
              node, query_segment,
              TraverseWithQuery(node->left, query_segment, visitor),
              TraverseWithQuery(node->right, query_segment, visitor)
          );
        }
      }
    }
  }

  class ComputeSumVisitor {
  public:
    using ResultType = Data;

    Data ProcessEmpty(const NodeHolder&) const {
      return {};
    }

    Data ProcessFull(const NodeHolder& node) const {
      return node->data;
    }

    Data ProcessPartial(const NodeHolder&, IndexSegment, const Data& left_result, const Data& right_result) const {
      return left_result + right_result;
    }
  };

  class AddBulkOperationVisitor {
  public:
    using ResultType = void;

    explicit AddBulkOperationVisitor(const BulkOperation& operation)
        : operation_(operation)
    {}

    void ProcessEmpty(const NodeHolder&) const {}

    void ProcessFull(const NodeHolder& node) const {
      node->postponed_bulk_operation.CombineWith(operation_);
      node->data = operation_.Collapse(node->data, node->segment);
    }

    void ProcessPartial(const NodeHolder& node, IndexSegment) const {
      node->data = (node->left ? node->left->data : Data()) + (node->right ? node->right->data : Data());
    }

  private:
    const BulkOperation& operation_;
  };

  static void PropagateBulkOperation(const NodeHolder& node) {
    for (auto* child_ptr : {node->left.get(), node->right.get()}) {
      if (child_ptr) {
        child_ptr->postponed_bulk_operation.CombineWith(node->postponed_bulk_operation);
        child_ptr->data = node->postponed_bulk_operation.Collapse(child_ptr->data, child_ptr->segment);
      }
    }
    node->postponed_bulk_operation = BulkOperation();
  }
};

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
