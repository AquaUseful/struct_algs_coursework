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

  using node_t = BTreeNode;
  using node_ptr_t = std::unique_ptr<node_t>;

  using node_ptr_array_t = std::vector<node_ptr_t>;
  using node_ptr_iterator_t = typename node_ptr_array_t::iterator;
  using node_ptr_const_iterator_t = typename node_ptr_array_t::const_iterator;

  using insert_result_t = struct InsertResult {
    bool splitted{false};
    value_t median{};
    node_ptr_t sibling{nullptr};
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
            node_ptr_iterator_t children_first)
      : BTreeNode(sibling.m_order, sibling.m_leaf) {
    m_values.resize(std::distance(first, sibling.end()));
    std::move(first, sibling.end(), begin());
    if (!m_leaf) {
      m_children.resize(std::distance(children_first, sibling.children_end()));
      std::move(children_first, sibling.children_end(), children_begin());
    }
  }

  BTreeNode(const size_t order, value_t median, node_ptr_t left,
            node_ptr_t right)
      : BTreeNode(order, false) {
    value_iterator_t ipos = insert_nosplit(begin(), median);
    (*left_child(ipos)) = std::move(left);
    (*right_child(ipos)) = std::move(right);
  }

  bool search(value_t value) {
    value_iterator_t lb = lower_bound(value);
    if (*lb == value) {
      return true;
    } else {
      if (m_leaf) {
        return false;
      } else {
        return (*left_child(lb))->search(value);
      }
    }
  }

  insert_result_t insert(value_t value) {
    value_iterator_t ipos = upper_bound(value);
    if (m_leaf) {
      insert_nosplit(ipos, value);
    } else {
      node_ptr_iterator_t lc = left_child(ipos);
      insert_result_t res = (*lc)->insert(value);
      if (res.splitted) {
        ipos = insert_nosplit(ipos, res.median);
        node_ptr_iterator_t rc = right_child(ipos);
        (*rc) = std::move(res.sibling);
      }
    }
    if (overfilled()) {
      return split();
    }
    return insert_result_t{};
  }

  void erase(value_t value) {
    value_iterator_t lb = lower_bound(value);
    if (*lb == value) {
      if (m_leaf) {
        m_values.erase(lb);
      } else {
        *lb = (*left_child(lb))->pop_max();
      }
    } else {
      if (!m_leaf) {
        node_ptr_iterator_t lc = left_child(lb);
        (*lc)->erase(value);
        refill(lc);
      }
    }
  }

  value_const_reference_t subtree_min() const {
    if (m_leaf) {
      return front();
    } else {
      return m_children.front().subtree_min();
    }
  }
  value_const_reference_t subtree_max() const {
    if (m_leaf) {
      return back();
    } else {
      return m_children.back().subtree_max();
    }
  }

  bool empty() const { return m_values.empty(); }
  node_ptr_t root_replacement() { return std::move(*children_begin()); }

  value_const_iterator_t cbegin() const { return m_values.cbegin(); }
  value_const_iterator_t cend() const { return m_values.cend(); }

  value_const_reference_t front() const { return m_values.front(); }
  value_const_reference_t back() const { return m_values.back(); }

private:
  value_iterator_t begin() { return m_values.begin(); }
  value_iterator_t end() { return m_values.end(); }

  value_iterator_t midpoint() {
    return std::next(begin(), std::distance(begin(), end()) / 2);
  }

  node_ptr_iterator_t children_begin() { return m_children.begin(); }
  node_ptr_iterator_t children_end() { return m_children.end(); }

  node_ptr_const_iterator_t children_cbegin() const {
    return m_children.cbegin();
  }
  node_ptr_const_iterator_t children_cend() const { return m_children.cend(); }

  node_ptr_iterator_t children_midpoint() {
    return std::next(children_begin(),
                     std::distance(children_begin(), children_end()) / 2);
  }

  node_ptr_iterator_t left_child(value_const_iterator_t pos) {
    return std::next(m_children.begin(), std::distance(cbegin(), pos));
  }
  node_ptr_iterator_t right_child(value_const_iterator_t pos) {
    return std::next(m_children.begin(), std::distance(cbegin(), pos) + 1);
  }

  node_ptr_const_iterator_t left_child(value_const_iterator_t pos) const {
    return std::next(m_children.cbegin(), std::distance(cbegin(), pos));
  }
  node_ptr_const_iterator_t right_child(value_const_iterator_t pos) const {
    return std::next(m_children.cbegin(), std::distance(cbegin(), pos) + 1);
  }

  value_iterator_t left_value(node_ptr_iterator_t node) {
    return std::next(begin(), std::distance(children_begin(), node) - 1);
  }
  value_iterator_t right_value(node_ptr_iterator_t node) {
    return std::next(begin(), std::distance(children_begin(), node));
  }

  value_iterator_t upper_bound(value_t value) {
    return std::upper_bound(begin(), end(), value);
  }
  value_iterator_t lower_bound(value_t value) {
    return std::lower_bound(begin(), end(), value);
  }

  bool underfilled() const { return m_values.size() <= (m_order / 2); }
  bool overfilled() const { return m_values.size() >= m_order; }

  value_iterator_t insert_nosplit(value_const_iterator_t pos, value_t value) {
    if (!m_leaf) {
      m_children.insert(right_child(pos), nullptr);
    }
    return m_values.insert(pos, value);
  }

  value_iterator_t insert_nosplit_last(value_t value) {
    return insert_nosplit(end(), value);
  }

  value_iterator_t insert_nosplit_first(value_t value) {
    return insert_nosplit(begin(), value);
  }

  value_t pop_max() {
    value_iterator_t maxi = std::prev(end());
    if (m_leaf) {
      value_t max = *maxi;
      m_values.erase(maxi);
      return max;
    } else {
      node_ptr_iterator_t maxc = std::prev(children_end());
      value_t max = (*maxc)->pop_max();
      refill(maxc);
      return max;
    }
  }

  value_t pop_min() {
    value_iterator_t mini = begin();
    if (m_leaf) {
      value_t min = *mini;
      m_values.erase(mini);
      return min;
    } else {
      node_ptr_iterator_t minc = children_begin();
      value_t min = (*minc)->pop_min();
      refill(minc);
      return min;
    }
  }

  insert_result_t split() {
    value_iterator_t median = midpoint();
    node_ptr_iterator_t ch_midpoint = children_midpoint();
    node_ptr_t sibling =
        std::make_unique<node_t>(*this, std::next(median), ch_midpoint);

    insert_result_t result{true, *median, std::move(sibling)};
    m_values.erase(median, cend());
    if (!m_leaf) {
      m_children.erase(ch_midpoint, children_cend());
    }

    return result;
  }

  void refill(node_ptr_iterator_t chld) {
    if (!((*chld)->underfilled())) {
      return;
    }
    if ((*chld)->m_leaf) {
      if (borrow(chld)) {
        return;
      }
    }
    if (chld == std::prev(children_end())) {
      [[unlikely]] merge_with_left(chld);
    } else {
      [[likely]] merge_with_right(chld);
    }
  }

  void merge_with_left(node_ptr_iterator_t chld) {
    value_iterator_t lval = left_value(chld);
    node_ptr_iterator_t lsib = std::prev(chld);
    (*lsib)->merge(*lval, std::move(*chld));
    m_values.erase(lval);
    m_children.erase(chld);
  }

  void merge_with_right(node_ptr_iterator_t chld) {
    value_iterator_t rval = right_value(chld);
    node_ptr_iterator_t rsib = std::next(chld);
    (*chld)->merge(*rval, std::move(*rsib));
    m_values.erase(rval);
    m_children.erase(rsib);
  }

  bool borrow_from_left(node_ptr_iterator_t chld) {
    value_iterator_t val = left_value(chld);
    node_ptr_iterator_t sibling = std::prev(chld);
    if (!(*sibling)->underfilled()) {
      (*chld)->insert_nosplit_first(*val);
      (*val) = (*sibling)->pop_max();
      return true;
    }
    return false;
  };

  bool borrow_from_right(node_ptr_iterator_t chld) {
    value_iterator_t val = right_value(chld);
    node_ptr_iterator_t sibling = std::next(chld);
    if (!(*sibling)->underfilled()) {
      (*chld)->insert_nosplit_last(*val);
      (*val) = (*sibling)->pop_min();
      return true;
    }
    return false;
  };

  bool borrow(node_ptr_iterator_t chld) {
    node_ptr_iterator_t left = std::prev(chld);
    node_ptr_iterator_t right = std::next(chld);
    if (chld == children_begin()) {
      return borrow_from_right(chld);
    } else if (chld == std::prev(children_end())) {
      return borrow_from_left(chld);
    } else {
      if (borrow_from_left(chld)) {
        return true;
      }
      return borrow_from_right(chld);
    }
  }

  void merge(value_t mid, node_ptr_t right) {
    value_iterator_t ipos = insert_nosplit_last(mid);
    m_values.resize(m_values.size() + right->m_values.size());
    m_children.resize(m_children.size() + right->m_children.size());
    if (!m_leaf) {
      std::move(right->children_begin(), right->children_end(),
                right_child(ipos));
    }
    std::move(right->begin(), right->end(), std::next(ipos));
  };

private:
  value_array_t m_values;
  node_ptr_array_t m_children;
  bool m_leaf;
  const size_t m_order;
};
} // namespace btree
