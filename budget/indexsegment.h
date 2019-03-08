#ifndef INDEXSEGMENT_H
#define INDEXSEGMENT_H

#include <cstdint>
#include <cmath>

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

#endif // INDEXSEGMENT_H
