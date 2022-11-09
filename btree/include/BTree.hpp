#pragma once

#include <algorithm>
#include <concepts>
#include <memory>

#include "Arr.hpp"
#include "BTreeNode.hpp"

namespace btree {
template <std::totally_ordered ValT> class BTree final {
public:
  using value_t = ValT;

  using node_t = BTreeNode<value_t>;
  using node_ptr_t = std::unique_ptr<node_t>;

  using value_iterator_t = typename node_t::value_iterator_t;

  using order_t = typename node_t::size_t;

  BTree(order_t order) : m_order{order}, m_root{nullptr} {}
  BTree(const BTree &) = default;
  BTree(BTree &&) noexcept = default;
  ~BTree() = default;

  BTree &operator=(const BTree &) = default;
  BTree &operator=(BTree &&) noexcept = default;

  void insert(value_t val) {
    if (m_root == nullptr) {
      m_root = std::make_unique<node_t>(m_order);
    }
    root_splitting_insert(val);
  }

  bool search(value_t val) {
    if (m_root == nullptr) {
      return false;
    }
    return m_root->search(val);
  }

  void remove(value_t val) {}

private:
  void root_splitting_insert(value_t val) {
    auto res = m_root->insert(val);
    if (res.splitted) {
      auto new_root = std::make_unique<node_t>(m_order, res.median, std::move(m_root),
                                std::move(res.sibling));
      m_root = std::move(new_root);
    }
  }

  node_ptr_t m_root;
  order_t m_order;
};
} // namespace btree
