#include "compute_request.h"

using namespace std;

void ComputeIncomeRequest::ParseFrom(string_view input) {
  date_from = Date::FromString(ReadToken(input));
  date_to = Date::FromString(input);
}

