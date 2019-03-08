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
