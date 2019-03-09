#ifndef INDEXSEGMENT_H
#define INDEXSEGMENT_H

#include <cstdint>
#include <cmath>

#include "dates.h"

struct IndexSegment {
  std::size_t left;
  std::size_t right;

  std::size_t length() const {
    return right - left;
  }
  bool empty() const {
    return length() == 0;
  }

  bool Contains(IndexSegment other) const {
    return left <= other.left && other.right <= right;
  }

  IndexSegment IntersectSegments(IndexSegment other) const;
  bool AreSegmentsIntersected(IndexSegment other) const;
};

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to, const Date& start_date);

std::ostream& operator<<(std::ostream& out, const IndexSegment& segment);
#endif // INDEXSEGMENT_H
