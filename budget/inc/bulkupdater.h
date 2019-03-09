#ifndef BULKUPDATER_H
#define BULKUPDATER_H

#include <cstdint>
#include <cmath>
#include <iomanip>

#include "indexsegment.h"

struct BulkMoneyAdder {
  double delta = 0.0;
};

struct BulkMoneySubber {
  double delta = 0.0;
};

struct BulkTaxApplier {
  uint32_t count = 0;
  size_t factor = 0;

  double ComputeFactor() const {
    return (1.0 - static_cast<double>(factor) / 100.0);
    //return std::pow(1.0 - static_cast<double>(factor) / 100.0, count);
  }
};

template <typename Data>
class BulkLinearUpdater {
public:
  BulkLinearUpdater() = default;

  BulkLinearUpdater(const BulkMoneyAdder& add)
      : add_(add)
  {}

  BulkLinearUpdater(const BulkMoneySubber& sub)
      : sub_(sub)
  {}

  BulkLinearUpdater(const BulkTaxApplier& tax)
      : tax_(tax)
  {}

  void CombineWith(const BulkLinearUpdater& other) {
    sub_.delta += other.sub_.delta;
    tax_.count += other.tax_.count;

    /* @TODO: Check this!!! */
    tax_.factor += other.tax_.factor;

    add_.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
  }

  Data Collapse(Data origin, IndexSegment segment) const {
    origin = origin - sub_.delta * segment.length();
    return origin * tax_.ComputeFactor() + add_.delta * segment.length();
  }

  std::ostream& operator<<(std::ostream& out) {
    out << "Operation{a: " << std::setw(9) << add_.delta
        << " s: " << std::setw(9) << sub_.delta
        << " c: " << std::setw(9) << tax_.count
        << " f: " << std::setw(9) << tax_.factor << "} ";
    return out;
  }
private:
  // apply tax first, then add
  BulkTaxApplier tax_;
  BulkMoneyAdder add_;
  BulkMoneySubber sub_;
};

#endif // BULKUPDATER_H
