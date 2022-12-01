#pragma once

#include <algorithm>
#include <iterator>
#include <queue>

namespace btree {
template <typename NodeT> class BTreeTraverser {
public:
  using node_t = NodeT;

  using node_reference_t = typename node_t::node_reference_t;
  using node_const_reference_t = typename node_t::node_const_reference_t;
  using node_ptr_t = typename node_t::node_ptr_t;
  using node_ptr_iterator_t = typename node_t::node_ptr_iterator_t;
  using node_ptr_const_iterator_t = typename node_t::node_ptr_const_iterator_t;
  using node_ptr_ptr_t = node_ptr_t *;

  using queue_t = std::queue<node_ptr_ptr_t>;

public:
  BTreeTraverser(node_ptr_ptr_t initial) : m_initial_node{initial} { reset(); }

  void reset() {
    m_queue = queue_t();
    m_queue.push(m_initial_node);
  };

  void next_step() {
    if (m_queue.empty()) {
      [[unlikely]] return;
    }
    for (node_ptr_iterator_t i = (*m_queue.front())->children_begin();
         i != (*m_queue.front())->children_end(); std::advance(i, 1)) {
      m_queue.push(&*i);
    }
    m_queue.pop();
  }
  node_ptr_ptr_t current_node() const {
    if (!m_queue.empty()) {
      [[likely]] return m_queue.front();
    } else {
      [[unlikely]] return nullptr;
    }
  }

private:
  node_ptr_ptr_t m_initial_node;
  queue_t m_queue;
};
} // namespace btree