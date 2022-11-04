#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>

namespace btree {
template <std::totally_ordered ValT, std::size_t order> class BTreeNode final {
  static_assert(order > 1, "Order must be more than 1");

public:
  using value_t = ValT;
  using value_reference_t = value_t &;
  using value_const_reference_t = const value_t &;
  using value_array_t = std::array<value_t, order>;
  using value_iterator_t = typename value_array_t::iterator;
  using value_const_iterator_t = typename value_array_t::const_iterator;
  using value_array_size_t = typename value_array_t::size_type;

  using child_t = BTreeNode<value_t, order>;
  using child_ptr_t = std::unique_ptr<child_t>;
  using child_ptr_array_t = std::array<child_ptr_t, order + 1>;
  using child_ptr_iterator_t = typename child_ptr_array_t::iterator;
  using child_ptr_const_iterator_t = typename child_ptr_array_t::const_iterator;

  using parent_t = BTreeNode<value_t, order>;
  using parent_ptr_t = parent_t *;

public:
  BTreeNode() : m_value_count(0), m_leaf(true), m_parent(nullptr) {}

  void insert(value_t val) {
    if (m_leaf) {
      unbalanced_insert(val);
      balance();
    } else {
      auto insert_pos = std::upper_bound(begin(), end(), val);
      auto lc = left_child(insert_pos);
      (*lc)->insert(val);
    }
  }

  bool search(value_t val) {
    auto candidate = std::lower_bound(begin(), end(), val);
    if (*candidate != val) {
      if (!m_leaf) {
        return (*left_child(candidate))->search(val);
      }
      return false;
    }
    return true;
  }

private:
  value_iterator_t begin() { return m_values.begin(); }
  value_iterator_t end() { return begin() + m_value_count; }

  child_ptr_iterator_t left_child(value_iterator_t it) {
    return m_children.begin() + std::distance(begin(), it);
  }
  child_ptr_iterator_t right_child(value_iterator_t it) {
    return m_children.begin() + std::distance(begin(), it) + 1;
  }

  value_iterator_t median() { return begin() + (m_value_count / 2); }

  value_iterator_t unbalanced_insert(value_t val) {
    auto insert_pos = std::upper_bound(begin(), end(), val);
    std::shift_right(insert_pos, end() + 1, 1);
    if (!m_leaf) {
      std::shift_right(right_child(insert_pos), right_child(end()) + 1, 1);
    }
    *insert_pos = val;
    ++m_value_count;
    return insert_pos;
  }

  void balance() {
    if (m_value_count == order) {
      split();
    }
  }

  void split() {
    if (m_parent == nullptr) {
      split_root();
      return;
    }
    child_ptr_t right_sibling = std::make_unique<child_t>();

    right_sibling->m_parent = m_parent;

    right_sibling->move_values(median() + 1, end());

    if (!m_leaf) {
      right_sibling->move_children(right_child(median()),
                                   right_child(end()) + 1);
    }

    value_iterator_t ins_point = m_parent->unbalanced_insert(*median());
    *(m_parent->right_child(ins_point)) = std::move(right_sibling);

    m_value_count = std::distance(begin(), median());

    m_parent->balance();
  }
  void split_root() {
    child_ptr_t new_left = std::make_unique<child_t>();
    child_ptr_t new_right = std::make_unique<child_t>();

    new_left->m_parent = this;
    new_right->m_parent = this;

    new_left->move_values(begin(), median());
    new_right->move_values(median() + 1, end());

    if (!m_leaf) {
      new_left->move_children(left_child(begin()), right_child(median()));
      new_right->move_children(right_child(median()), right_child(end()) + 1);
      new_left->m_leaf = false;
      new_right->m_leaf = false;
    }

    *begin() = *median();
    m_value_count = 1;
    m_leaf = false;

    *left_child(begin()) = std::move(new_left);
    *right_child(begin()) = std::move(new_right);
  }

  void move_values(value_iterator_t first, value_iterator_t last) {
    std::move(first, last, m_values.begin());
    m_value_count = std::distance(first, last);
  }

  void move_children(child_ptr_iterator_t first, child_ptr_iterator_t last) {
    std::move(first, last, m_children.begin());
  }

private:
  value_array_t m_values;
  child_ptr_array_t m_children;
  parent_ptr_t m_parent;

  value_array_size_t m_value_count;

  bool m_leaf;
};
} // namespace btree