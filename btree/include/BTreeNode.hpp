#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>

#include "BTreeNode.hpp"

namespace btree {

template <std::totally_ordered ValT> class BTreeNode final {
public:
  using value_t = ValT;
  using value_reference_t = value_t &;
  using value_const_reference_t = const value_t &;
  using value_ptr_t = value_t *;
  using value_iterator_t = value_ptr_t;
  using value_const_iterator_t = const value_ptr_t;
  using value_difference_t =
      decltype(std::distance(value_iterator_t{}, value_iterator_t{}));
  using value_array_t = value_t[];
  using value_array_ptr_t = std::unique_ptr<value_array_t>;

  using child_t = BTreeNode<value_t>;
  using child_ptr_t = std::unique_ptr<child_t>;
  using child_ptr_array_t = child_ptr_t[];
  using child_ptr_array_ptr_t = std::unique_ptr<child_ptr_array_t>;
  using child_ptr_ptr_t = child_ptr_t *;
  using child_ptr_iterator_t = child_ptr_ptr_t;
  using child_ptr_const_iterator_t = const child_ptr_ptr_t;
  using child_ptr_difference_t =
      decltype(std::distance(child_ptr_iterator_t{}, child_ptr_iterator_t{}));

  using parent_t = BTreeNode<value_t>;
  using parent_reference_t = parent_t &;
  using parent_const_reference_r = const parent_t &;
  using parent_ptr_t = parent_t *;

  using size_t = std::size_t;

  struct SplitResult {
    value_t median;
    child_ptr_t sibling;
  };

public:
  BTreeNode(size_t max_size)
      : m_values{std::make_unique<value_array_t>(max_size)},
        m_children{std::make_unique<child_ptr_array_t>(max_size + 2)},
        m_size{0}, m_max_size{max_size}, m_leaf{true} {};

  BTreeNode(size_t max_size, value_t median, child_ptr_t left,
            child_ptr_t right)
      : BTreeNode(max_size) {
    (*begin()) = median;
    (*left_child(begin())) = std::move(left);
    (*right_child(begin())) = std::move(right);
    m_size = 1;
    m_leaf = false;
  }

  ~BTreeNode() = default;

  SplitResult insert(value_t val) { return std::move(splitting_insert(val)); }

  void remove(value_t val) {}

  bool search(value_t val) {
    auto candidate = lower_bound(val);
    if (*candidate != val) {
      if (!m_leaf) {
        return (*left_child(candidate))->search(val);
      }
      return false;
    }
    return true;
  }

  value_iterator_t begin() { return m_values.get(); }
  value_iterator_t end() { return begin() + m_size; }

  child_ptr_iterator_t left_child(value_iterator_t it) {
    return m_children.get() + (it - m_values.get());
  }
  child_ptr_iterator_t right_child(value_iterator_t it) {
    auto children_begin = m_children.get();
    auto val_distance = it - begin();
    auto res = children_begin + val_distance;
    ++res;
    return res;
  }

private:
  value_iterator_t median() {
    return begin() + (std::distance(begin(), end()) / 2);
  }

  value_iterator_t upper_bound(value_t val) {
    return std::upper_bound(begin(), end(), val);
  }

  value_iterator_t lower_bound(value_t val) {
    return std::lower_bound(begin(), end(), val);
  }

  void raw_insert(value_iterator_t pos, value_t val) {
    std::shift_right(pos, end(), 1);
    if (!m_leaf) {
      std::shift_right(right_child(pos), right_child(end()), 1);
    }
    *pos = val;
    ++m_size;
  }

  SplitResult splitting_insert(value_t val) {
    value_iterator_t ipos = upper_bound(val);
    if (m_leaf) {
      raw_insert(ipos, val);
    } else {
      child_ptr_iterator_t lc = left_child(ipos);
      SplitResult insert_res = (*lc)->splitting_insert(val);
      if (insert_res.sibling != nullptr) {
        ipos = upper_bound(insert_res.median);
        raw_insert(ipos, insert_res.median);
        child_ptr_iterator_t rc = right_child(ipos);
        (*rc) = std::move(insert_res.sibling);
      }
    }
    if (max_filled()) {
      return split();
    }
    return SplitResult{0, nullptr};
  }

  SplitResult split() {
    child_ptr_t right_sibling = std::make_unique<child_t>(m_max_size);

    right_sibling->m_leaf = m_leaf;
    right_sibling->move_values(median() + 1, end());
    if (!m_leaf) {
      right_sibling->move_children(right_child(median()),
                                   right_child(end()) + 1);
    }

    SplitResult result{*median(), std::move(right_sibling)};

    m_size = std::distance(begin(), median());

    return result;
  }

  void rotate_right(value_iterator_t pos) {}

  void rotate_left(value_iterator_t pos) {}

  bool min_filled() const { return m_size == (m_max_size / 2); }
  bool max_filled() const { return m_size >= m_max_size; }

  void move_values(value_iterator_t first, value_iterator_t last) {
    std::move(first, last, m_values.get());
    m_size = std::distance(first, last);
  }

  void move_children(child_ptr_iterator_t first, child_ptr_iterator_t last) {
    std::move(first, last, m_children.get());
  }

private:
  child_ptr_array_ptr_t m_children;
  value_array_ptr_t m_values;
  size_t m_size;
  const size_t m_max_size;
  bool m_leaf;
};
} // namespace btree