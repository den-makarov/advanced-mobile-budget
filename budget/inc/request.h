#ifndef REQUEST_H
#define REQUEST_H

#include <string>

#include "budgetmanager.h"

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
  enum class Type {
    COMPUTE_INCOME,
    EARN,
    SPEND,
    PAY_TAX
  };

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(std::string_view input) = 0;
  virtual ~Request() = default;

  const Type type;
};

template <typename ResultType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const BudgetManager& manager) const = 0;
  virtual ~ReadRequest() = default;
};

struct ModifyRequest : Request {
  using Request::Request;
  virtual void Process(BudgetManager& manager) const = 0;
  virtual ~ModifyRequest() = default;
};

#endif // REQUEST_H
