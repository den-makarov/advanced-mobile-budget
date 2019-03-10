#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "stringhelper.h"
#include "dates.h"

using namespace std;

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = static_cast<size_t>(ComputeDaysDiff(END_DATE, START_DATE));
static const size_t DAY_COUNT_P2 = 1 << 16;
static const size_t VERTEX_COUNT = DAY_COUNT_P2 * 2;

array<double, VERTEX_COUNT> tree_values, tree_add, tree_sub, tree_factor, tree_spends;

void Init() {
  tree_values.fill(0);
  tree_spends.fill(0);
  tree_add.fill(0);
  tree_sub.fill(0);
  tree_factor.fill(1);
}

void Pop(size_t v, size_t l, size_t r) {
  for (size_t w = v * 2; w <= v * 2 + 1; ++w) {
    if (w < VERTEX_COUNT) {
      tree_sub[w] += tree_sub[v];
      tree_spends[w] += tree_sub[v] * (r - l) / 2;
    }
  }
  tree_sub[v] = 0;
}

void Push(size_t v, size_t l, size_t r) {
  for (size_t w = v * 2; w <= v * 2 + 1; ++w) {
    if (w < VERTEX_COUNT) {
      tree_factor[w] *= tree_factor[v];
      (tree_add[w] *= tree_factor[v]) += tree_add[v];
      (tree_values[w] *= tree_factor[v]) += tree_add[v] * (r - l) / 2;
    }
  }
  tree_factor[v] = 1;
  tree_add[v] = 0;
}

double ComputeSum(size_t v, size_t l, size_t r, size_t ql, size_t qr) {
  if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
    return 0;
  }
  Push(v, l, r);
  Pop(v, l, r);
  if (ql <= l && r <= qr) {
    return tree_values[v] - tree_spends[v];
  }
  return ComputeSum(v * 2, l, (l + r) / 2, ql, qr)
      + ComputeSum(v * 2 + 1, (l + r) / 2, r, ql, qr);
}

void Sub(size_t v, size_t l, size_t r, size_t ql, size_t qr, double value) {
  if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
    return;
  }
  Pop(v, l, r);
  if (ql <= l && r <= qr) {
    tree_sub[v] += value;
    tree_spends[v] += value * (r - l);
    return;
  }
  Sub(v * 2, l, (l + r) / 2, ql, qr, value);
  Sub(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
  tree_spends[v] =
      (v * 2 < VERTEX_COUNT ? tree_spends[v * 2] : 0)
      + (v * 2 + 1 < VERTEX_COUNT ? tree_spends[v * 2 + 1] : 0);
}

void Add(size_t v, size_t l, size_t r, size_t ql, size_t qr, double value) {
  if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
    return;
  }
  Push(v, l, r);
  if (ql <= l && r <= qr) {
    tree_add[v] += value;
    tree_values[v] += value * (r - l);
    return;
  }
  Add(v * 2, l, (l + r) / 2, ql, qr, value);
  Add(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
  tree_values[v] =
      (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : 0)
      + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : 0);
}

void Multiply(size_t v, size_t l, size_t r, size_t ql, size_t qr, double value) {
  if (v >= VERTEX_COUNT || qr <= l || r <= ql) {
    return;
  }
  Push(v, l, r);
  if (ql <= l && r <= qr) {
    tree_factor[v] *= value;
    tree_add[v] *= value;
    tree_values[v] *= value;
    return;
  }
  Multiply(v * 2, l, (l + r) / 2, ql, qr, value);
  Multiply(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
  tree_values[v] =
      (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : 0)
      + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : 0);
}


int main() {
  cout.precision(25);
  assert(DAY_COUNT <= DAY_COUNT_P2 && DAY_COUNT_P2 < DAY_COUNT * 2);

  Init();

  int q;
  cin >> q;

  for (int i = 0; i < q; ++i) {
    string query_type;
    cin >> query_type;

    string date_from_str, date_to_str;
    cin >> date_from_str >> date_to_str;

    auto idx_from = ComputeDayIndex(Date::FromString(date_from_str), START_DATE);
    auto idx_to = ComputeDayIndex(Date::FromString(date_to_str), START_DATE) + 1;

    if (query_type == "ComputeIncome") {
      cout << ComputeSum(1, 0, DAY_COUNT_P2, idx_from, idx_to) << endl;
    } else if (query_type == "PayTax") {
      size_t value;
      cin >> value;
      Multiply(1, 0, DAY_COUNT_P2, idx_from, idx_to, 1.0 - value/100.0);
    } else if (query_type == "Earn") {
      double value;
      cin >> value;
      Add(1, 0, DAY_COUNT_P2, idx_from, idx_to, value / (idx_to - idx_from));
    } else if (query_type == "Spend") {
      double value;
      cin >> value;
      Sub(1, 0, DAY_COUNT_P2, idx_from, idx_to, value / (idx_to - idx_from));
    }
  }

  return 0;
}
