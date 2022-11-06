#pragma once

#include <concepts>
#include <memory>

#include "BTreeNode.hpp"

namespace btree {
template <std::totally_ordered ValT, std::size_t order> class BTree final {
public:
  using value_t = ValT;

  using node_t = BTreeNode<value_t, order>;
  using node_ptr_t = std::unique_ptr<node_t>;

  BTree() : m_root{nullptr} {}
  BTree(const BTree &) = default;
  BTree(BTree &&) noexcept = default;
  ~BTree() = default;

  BTree &operator=(const BTree &) = default;
  BTree &operator=(BTree &&) noexcept = default;

  void insert(value_t val) {
    if (m_root == nullptr) {
      m_root = std::make_unique<node_t>();
    }
    m_root->insert(val);
  }

  bool search(value_t val) {
    if (m_root == nullptr) {
      return false;
    }
    return m_root->search(val);
  }

private:
  node_ptr_t m_root;
};
} // namespace btree