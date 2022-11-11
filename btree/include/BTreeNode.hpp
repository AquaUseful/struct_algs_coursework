#pragma once

#include "BTreeNodeIterator.hpp"
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

  using iterator_t = BTreeNodeIterator<BTreeNode>;

  using insert_result_t = struct InsertResult {
    bool splitted{false};
    value_t median{};
    node_ptr_t sibling{nullptr};
  };

  using search_result_t = struct SearchResult {
    bool found{false};
    value_iterator_t lower_bound;
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
    search_result_t res = search_single_node(value);
    if (m_leaf) {
      return res.found;
    } else {
      return (*res.search_resume_node)->search(value);
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
    if (max_filled()) {
      return split();
    }
    return insert_result_t{};
  }

  void erase(value_t value) {
    search_result_t sres = search_single_node(value);
    if (sres.found) {
      if (m_leaf) {
        erase_leaf(sres.lower_bound);
      } else {
      }
    } else {
      (*sres.search_resume_node)->erase(value);
    }
  }

private:
  value_iterator_t begin() { return m_values.begin(); }
  value_iterator_t end() { return m_values.end(); }

  value_const_iterator_t cbegin() const { return m_values.cbegin(); }
  value_const_iterator_t cend() const { return m_values.cend(); }

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
    return std::next(begin(), std::distance(children_begin(), node));
  }
  value_iterator_t right_value(node_ptr_iterator_t node) {
    return std::next(begin(), std::distance(children_begin(), node) + 1);
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

  void erase_leaf(value_const_iterator_t pos) { m_values.erase(pos); }

  value_t pop_max() {
    value_iterator_t maxi = std::prev(end());
    if (m_leaf) {
      value_t max = *maxi;
      m_values.erase(maxi);
      return max;
    } else {
      node_ptr_iterator_t mc = std::prev(children_end());
      return (*mc)->pop_max();
    }
  }

  search_result_t search_single_node(value_t value) {
    value_iterator_t lb = lower_bound(value);
    return search_result_t{*lb == value, lb, left_child(lb)};
  }

  insert_result_t split() {
    value_iterator_t median = midpoint();
    node_ptr_iterator_t ch_midpoint = children_midpoint();
    node_ptr_t sibling =
        std::make_unique<node_t>(*this, std::next(median), ch_midpoint);

    insert_result_t result{true, *median, std::move(sibling)};

    // m_values.erase(median);
    m_values.erase(median, cend());
    if (!m_leaf) {
      m_children.erase(ch_midpoint, children_cend());
    }

    return result;
  }

  void rotate_right(value_iterator_t pos) {
    node_ptr_iterator_t lc = left_child(pos);
    node_ptr_iterator_t rc = right_child(pos);
    (*rc)->insert_nosplit((*rc)->begin(), *pos);
    *pos = (*lc)->pop_max();
  };

  void rotate_left(value_iterator_t pos) {
    node_ptr_iterator_t lc = left_child(pos);
    node_ptr_iterator_t rc = right_child(pos);
    (*lc)->insert_nosplit((*rc)->end(), *pos);
    *pos = (*rc)->pop_min();
  }

  void merge(value_t mid){};

private:
  value_array_t m_values;
  node_ptr_array_t m_children;
  bool m_leaf;
  const size_t m_order;
};
} // namespace btree