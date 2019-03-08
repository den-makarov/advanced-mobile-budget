#ifndef BULKUPDATER_H
#define BULKUPDATER_H

#include <cstdint>
#include <cmath>

#include "indexsegment.h"

struct BulkMoneyAdder {
  double delta = 0.0;
};

constexpr uint8_t TAX_PERCENTAGE = 13;

struct BulkTaxApplier {
  static constexpr double FACTOR = 1.0 - TAX_PERCENTAGE / 100.0;
  uint32_t count = 0;

  double ComputeFactor() const {
    return std::pow(FACTOR, count);
  }
};

template <typename Data>
class BulkLinearUpdater {
public:
  BulkLinearUpdater() = default;

  BulkLinearUpdater(const BulkMoneyAdder& add)
      : add_(add)
  {}

  BulkLinearUpdater(const BulkTaxApplier& tax)
      : tax_(tax)
  {}

  void CombineWith(const BulkLinearUpdater& other) {
    tax_.count += other.tax_.count;
    add_.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
  }

  Data Collapse(Data origin, IndexSegment segment) const {
    (void)segment;
    return origin;// * tax_.ComputeFactor() + add_.delta * segment.length();
  }

private:
  // apply tax first, then add
  BulkTaxApplier tax_;
  BulkMoneyAdder add_;
};

#endif // BULKUPDATER_H
