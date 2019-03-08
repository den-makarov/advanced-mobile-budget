#ifndef BUDGETMANAGER_H
#define BUDGETMANAGER_H

#include "moneystate.h"
#include "segmenttree.h"

class BudgetManager : public SummingSegmentTree<MoneyState, BulkLinearUpdater<MoneyState>> {
public:
    BudgetManager(size_t size) : SummingSegmentTree(size) {}
//    auto MakeDateRange(const Date& date_from, const Date& date_to) const {
//      const auto segment = MakeDateSegment(date_from, date_to);
//      return Range(begin() + segment.left, begin() + segment.right);
//    }
};

#endif // BUDGETMANAGER_H
