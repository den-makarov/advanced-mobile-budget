#ifndef SEGMENTTREE_H
#define SEGMENTTREE_H

#include <memory>

#include "bulkupdater.h"

template <typename Data, typename BulkOperation>
class SummingSegmentTree {
public:
  SummingSegmentTree(size_t size) : root_(Build({0, size})) {}

  Data ComputeSum(IndexSegment segment) const {
    return this->TraverseWithQuery(root_, segment, ComputeSumVisitor{});
  }

  void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
    this->TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
  }

  std::ostream& operator<<(std::ostream& out) {
    root_.get()->operator<<(out);
    return out;
  }

private:
  struct Node;
  using NodeHolder = std::unique_ptr<Node>;

  struct Node {
    NodeHolder left;
    NodeHolder right;
    IndexSegment segment;
    Data data;
    BulkOperation postponed_bulk_operation;
    std::ostream& operator<<(std::ostream& out) {
      out << segment << data;
      postponed_bulk_operation.operator<<(out);
      out << std::endl;
      if(left) {
        left.get()->operator<<(out);
      }
      if(right) {
        right.get()->operator<<(out);
      }
      return out;
    }
  };

  NodeHolder root_;

  static NodeHolder Build(IndexSegment segment) {
    if (segment.empty()) {
      return nullptr;
    } else if (segment.length() == 1) {
      return std::make_unique<Node>(Node{nullptr, nullptr, segment, {}, {}});
//      return make_unique<Node>(Node{
//        .left = nullptr,
//        .right = nullptr,
//        .segment = segment,
//      });
    } else {
      const size_t middle = segment.left + segment.length() / 2;
      return std::make_unique<Node>(Node{Build({segment.left, middle}),
                                    Build({middle, segment.right}),
                                    segment,
                                    {},
                                    {}});
//      return make_unique<Node>(Node{
//        .left = Build({segment.left, middle}),
//        .right = Build({middle, segment.right}),
//        .segment = segment,
//      });
    }
  }

  template <typename Visitor>
  static typename Visitor::ResultType TraverseWithQuery(const NodeHolder& node, IndexSegment query_segment, Visitor visitor) {
    if (!node || !node->segment.AreSegmentsIntersected(query_segment)) {
      return visitor.ProcessEmpty(node);
    } else {
      PropagateBulkOperation(node);
      if (query_segment.Contains(node->segment)) {
        return visitor.ProcessFull(node);
      } else {
        if constexpr (std::is_void_v<typename Visitor::ResultType>) {
          TraverseWithQuery(node->left, query_segment, visitor);
          TraverseWithQuery(node->right, query_segment, visitor);
          return visitor.ProcessPartial(node, query_segment);
        } else {
          return visitor.ProcessPartial(
              TraverseWithQuery(node->left, query_segment, visitor),
              TraverseWithQuery(node->right, query_segment, visitor)
          );
        }
      }
    }
  }

  class ComputeSumVisitor {
  public:
    using ResultType = Data;

    Data ProcessEmpty(const NodeHolder&) const {
      return {};
    }

    Data ProcessFull(const NodeHolder& node) const {
      return node->data;
    }

    Data ProcessPartial(const Data& left_result, const Data& right_result) const {
    //Data ProcessPartial(const NodeHolder&, IndexSegment, const Data& left_result, const Data& right_result) const {
      return left_result + right_result;
    }
  };

  class AddBulkOperationVisitor {
  public:
    using ResultType = void;

    explicit AddBulkOperationVisitor(const BulkOperation& operation)
        : operation_(operation)
    {}

    void ProcessEmpty(const NodeHolder&) const {}

    void ProcessFull(const NodeHolder& node) const {
      node->postponed_bulk_operation.CombineWith(operation_);
      node->data = operation_.Collapse(node->data, node->segment);
    }

    void ProcessPartial(const NodeHolder& node, IndexSegment) const {
      node->data = (node->left ? node->left->data : Data()) + (node->right ? node->right->data : Data());
    }

  private:
    const BulkOperation& operation_;
  };

  static void PropagateBulkOperation(const NodeHolder& node) {
    for (auto* child_ptr : {node->left.get(), node->right.get()}) {
      if (child_ptr) {
        child_ptr->postponed_bulk_operation.CombineWith(node->postponed_bulk_operation);
        child_ptr->data = node->postponed_bulk_operation.Collapse(child_ptr->data, child_ptr->segment);
      }
    }
    node->postponed_bulk_operation = BulkOperation();
  }
};

#endif // SEGMENTTREE_H
