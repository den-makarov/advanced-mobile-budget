#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <exception>
#include <tuple>
#include <iterator>

template<typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }
  size_t size() const { return std::distance(begin_, end_); }

private:
  It begin_;
  It end_;
};

struct SimpleDate {
  int year;
  int month;
  int day;
  SimpleDate() {}
  explicit SimpleDate(int new_year, int new_month, int new_day)
    : year(new_year)
    , month(new_month)
    , day(new_day)
  {}

  bool operator<(const SimpleDate& other) {
    return std::tie(year, month, day) < std::tie(other.year, other.month, other.day);
  }
};

class Date
{
public:
  Date();
  Date(const SimpleDate& _date);

  time_t AsTimeStamp() const;
  SimpleDate AsSimpleDate() const {
    return date;
  }

  bool operator<(const Date& other) {
    return date < other.date;
  }
private:
  SimpleDate date;
};

struct DateHasher {
  size_t operator()(const Date& d) const {
    SimpleDate date = d.AsSimpleDate();
    size_t a = i_hash(date.year);
    size_t b = i_hash(date.month);
    size_t c = i_hash(date.day);
    size_t x = 85;
    return (((x * a) + b) * x) + c;
  }
  std::hash<int> i_hash;
};



bool operator==(const SimpleDate& rhs, const SimpleDate& lhs);
bool operator==(const Date& lhs, const Date& rhs);
SimpleDate ParseInputDate(std::istream& date_stream);
int ComputeDaysDiff(const Date& date_to, const Date& date_from);
std::istream& operator>>(std::istream& in, SimpleDate& date);
std::ostream& operator<<(std::ostream& out, SimpleDate date);

#endif // DATE_H
