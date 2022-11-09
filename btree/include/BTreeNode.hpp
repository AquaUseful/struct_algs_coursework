#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

namespace btree {
template <std::totally_ordered ValT> class BTreeNode final {
public:
  using size_t = std::size_t;
  using difference_t = std::ptrdiff_t;

  using value_t = ValT;
  using value_reference_t = value_t &;
  using value_const_reference_t = const value_t &;

  using value_array_t = std::vector<value_t>;
  using value_iterator_t = typename value_array_t::iterator;
  using value_const_iterator_t = typename value_array_t::const_iterator;

  using child_t = BTreeNode<value_t>;
  using child_ptr_t = std::unique_ptr<child_t>;

  using child_ptr_array_t = std::vector<child_ptr_t>;
  using child_ptr_iterator_t = typename child_ptr_array_t::iterator;
  using child_ptr_const_iterator_t = typename child_ptr_array_t::const_iterator;

  using insert_result_t = struct InsertResult {
    bool splitted{false};
    value_t median{};
    child_ptr_t sibling{nullptr};
  };

public:
  BTreeNode(const size_t order, bool leaf = true)
      : m_order{order}, m_leaf{leaf} {
    m_values.reserve(m_order);
    if (!m_leaf) {
      m_children.reserve(m_order + 1);
      m_children.push_back(nullptr);
    }
  }

  BTreeNode(BTreeNode &sibling, value_iterator_t first,
            child_ptr_iterator_t children_first)
      : BTreeNode(sibling.m_order, sibling.m_leaf) {
    m_values.resize(std::distance(first, sibling.end()));
    std::move(first, sibling.end(), begin());
    if (!m_leaf) {
      m_children.resize(std::distance(children_first, sibling.children_end()));
      std::move(children_first, sibling.children_end(), children_begin());
    }
  }

  BTreeNode(const size_t order, value_t median, child_ptr_t left,
            child_ptr_t right)
      : BTreeNode(order, false) {
    value_iterator_t ipos = insert_nosplit(begin(), median);
    // m_children.resize(2);
    (*left_child(ipos)) = std::move(left);
    (*right_child(ipos)) = std::move(right);
  }

  bool search(value_t value) {
    value_const_iterator_t candidate = lower_bound(value);
    [[likely]] if (*candidate != value) {
      [[likely]] if (!m_leaf) {
        return (*left_child(candidate))->search(value);
      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  insert_result_t insert(value_t value) {
    value_iterator_t ipos = upper_bound(value);
    if (m_leaf) {
      insert_nosplit(ipos, value);
    } else {
      child_ptr_iterator_t lc = left_child(ipos);
      insert_result_t res = (*lc)->insert(value);
      if (res.splitted) {
        ipos = insert_nosplit(ipos, res.median);
        child_ptr_iterator_t rc = right_child(ipos);
        (*rc) = std::move(res.sibling);
      }
    }
    if (max_filled()) {
      return split();
    }
    return insert_result_t{};
  }

private:
  value_iterator_t begin() { return m_values.begin(); }
  value_iterator_t end() { return m_values.end(); }

  value_const_iterator_t cbegin() const { return m_values.cbegin(); }
  value_const_iterator_t cend() const { return m_values.cend(); }

  value_iterator_t midpoint() {
    return std::next(begin(), std::distance(begin(), end()) / 2);
  }

  child_ptr_iterator_t children_begin() { return m_children.begin(); }
  child_ptr_iterator_t children_end() { return m_children.end(); }

  child_ptr_const_iterator_t children_cbegin() const {
    return m_children.cbegin();
  }
  child_ptr_const_iterator_t children_cend() const { return m_children.cend(); }

  child_ptr_iterator_t children_midpoint() {
    return std::next(children_begin(),
                     std::distance(children_begin(), children_end()) / 2);
  }

  child_ptr_iterator_t left_child(value_const_iterator_t pos) {
    return std::next(m_children.begin(), std::distance(cbegin(), pos));
  }
  child_ptr_iterator_t right_child(value_const_iterator_t pos) {
    return std::next(m_children.begin(), std::distance(cbegin(), pos) + 1);
  }

  child_ptr_const_iterator_t left_child(value_const_iterator_t pos) const {
    return std::next(m_children.cbegin(), std::distance(cbegin(), pos));
  }
  child_ptr_const_iterator_t right_child(value_const_iterator_t pos) const {
    return std::next(m_children.cbegin(), std::distance(cbegin(), pos) + 1);
  }

  value_iterator_t upper_bound(value_t value) {
    return std::upper_bound(begin(), end(), value);
  }
  value_iterator_t lower_bound(value_t value) {
    return std::lower_bound(begin(), end(), value);
  }

  bool min_filled() const { return m_values.size() == (m_order / 2); }
  bool max_filled() const { return m_values.size() == m_order; }

  value_iterator_t insert_nosplit(value_const_iterator_t pos, value_t value) {
    if (!m_leaf) {
      m_children.insert(right_child(pos), nullptr);
    }
    return m_values.insert(pos, value);
  }

  insert_result_t split() {
    value_iterator_t median = midpoint();
    child_ptr_iterator_t ch_midpoint = children_midpoint();
    child_ptr_t sibling =
        std::make_unique<child_t>(*this, std::next(median), ch_midpoint);

    insert_result_t result{true, *median, std::move(sibling)};

    // m_values.erase(median);
    m_values.erase(median, cend());
    if (!m_leaf) {
      m_children.erase(ch_midpoint, children_cend());
    }

    return result;
  }

private:
  value_array_t m_values;
  child_ptr_array_t m_children;
  bool m_leaf;
  const size_t m_order;
};
} // namespace btree