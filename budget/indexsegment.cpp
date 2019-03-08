#include "indexsegment.h"

using namespace std;

IndexSegment IndexSegment::IntersectSegments(IndexSegment other) const {
  const size_t left = max(left, other.left);
  const size_t right = min(right, other.right);
  return {left, max(left, right)};
}

bool IndexSegment::AreSegmentsIntersected(IndexSegment other) const {
  return !(other.right <= left || other.right <= left);
}

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to, const Date& start_date) {
  return {ComputeDayIndex(date_from, start_date), ComputeDayIndex(date_to, start_date) + 1};
}
