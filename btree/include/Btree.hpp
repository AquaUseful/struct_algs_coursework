#pragma once

#include <cstddef>
#include <memory>

#include "BTreeKey.hpp"
#include "BTreeNode.hpp"

namespace btree {
template <BTreeKey ValueT, std::size_t order> class Btree final {
public:
  using node_type = BtreeNode<ValueT, order>;
  using node_ptr_type = std::unique_ptr<node_type>;

  void insert(typename node_type::value_const_reference_type value) {
    auto insertion_node = m_root;
  }

private:
  node_ptr_type m_root;
};
} // namespace btree