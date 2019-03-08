#ifndef DATES_H
#define DATES_H

#include <ctime>

#include "stringhelper.h"

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value);
class Date {
public:
  static Date FromString(std::string_view str);

  // Weird legacy, can't wait for std::chrono::year_month_day
  time_t AsTimestamp() const {
    std::tm t;
    t.tm_sec  = 0;
    t.tm_min  = 0;
    t.tm_hour = 0;
    t.tm_mday = day_;
    t.tm_mon  = month_ - 1;
    t.tm_year = year_ - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

private:
  int year_;
  int month_;
  int day_;

  Date(int year, int month, int day)
      : year_(year), month_(month), day_(day)
  {}
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from);

#endif // DATES_H
