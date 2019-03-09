#include "pay_tax_request.h"

using namespace std;

void PayTaxRequest::ParseFrom(string_view input) {
  date_from = Date::FromString(ReadToken(input));
  date_to = Date::FromString(ReadToken(input));
  percentage = static_cast<size_t>(ConvertToInt(input));
}

