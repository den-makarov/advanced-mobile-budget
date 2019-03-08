#include "date.h"

using namespace std;

Date::Date()
  : date(2000, 01, 01)
{}

Date::Date(const SimpleDate& _date)
  : date(_date)
{}

time_t Date::AsTimeStamp() const {
  tm t;
  t.tm_year = date.year - 1900;
  t.tm_mon  = date.month - 1;
  t.tm_mday = date.day;
  t.tm_hour = 0;
  t.tm_min  = 0;
  t.tm_sec  = 0;
  t.tm_isdst = 0;
  return mktime(&t);
}

SimpleDate ParseInputDate(istream& date_stream) {
  bool ok = true;

  int year = 2000;
  ok = ok && (date_stream >> year);
  ok = ok && (date_stream.peek() == '-');
  date_stream.ignore(1);

  int month = 01;
  ok = ok && (date_stream >> month);
  ok = ok && (date_stream.peek() == '-');
  date_stream.ignore(1);

  int day = 01;
  ok = ok && (date_stream >> day);

  if (!ok) {
    throw logic_error("Wrong date format");
  }
  return SimpleDate(year, month, day);
}

istream& operator>>(istream& in, SimpleDate& date) {
  date = ParseInputDate(in);
  return in;
}

ostream& operator<<(ostream& out, SimpleDate date) {
  out << setw(4) << setfill('0') << date.year <<
      "-" << setw(2) << setfill('0') << date.month <<
      "-" << setw(2) << setfill('0') << date.day;
  return out;
}

bool operator==(const SimpleDate& rhs, const SimpleDate& lhs) {
  return lhs.year == rhs.year
      && lhs.month == rhs.month
      && lhs.day == rhs.day;
}

bool operator==(const Date& lhs, const Date& rhs) {
  return lhs.AsSimpleDate() == rhs.AsSimpleDate();
}

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimeStamp();
  const time_t timestamp_from = date_from.AsTimeStamp();
  static const int SECONDS_IN_DAY = 60 * 60 * 24;
  return static_cast<int>((timestamp_to - timestamp_from) / SECONDS_IN_DAY);
}
