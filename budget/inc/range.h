#ifndef RANGE_H
#define RANGE_H

#include <cstdint>

template<typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }
  std::size_t size() const { return distance(begin_, end_); }

private:
  It begin_;
  It end_;
};

#endif // RANGE_H
