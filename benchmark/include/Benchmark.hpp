#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <random>
#include <vector>
#include <chrono>

#include "Arr.hpp"
#include "BTree.hpp"

namespace benchmark {
  template <std::totally_ordered ValT> class Benchmark {
  public:
    using value_t = ValT;

    using array_t = arr::Arr<value_t>;
    using tree_t = btree::BTree<value_t>;

    using size_t = typename array_t::size_t;
    using order_t = typename tree_t::order_t;

    using clock_t = std::chrono::high_resolution_clock;
    using time_unit_t = std::chrono::microseconds;
    using rep_t = typename time_unit_t::rep;

    using measurement_t = struct Measurement {
      std::double_t filled_part;
      typename time_unit_t::rep array_time;
      typename time_unit_t::rep tree_time;
    };

    using rand_dev_t = std::random_device;
    using rand_gen_t = std::mt19937_64;
    using rand_distrib_t = std::uniform_int_distribution<value_t>;

  public:
    Benchmark(size_t max_size = 1000, order_t tree_order = 1024, value_t grow = 10)
      : m_array {max_size}, m_tree {tree_order}, m_rdev {}, m_rgen {m_rdev()}, m_clock {} {
    }

    void change_order(order_t tree_order) {
      m_tree = tree_t(tree_order);
    }

    void change_size(size_t size) {
      m_array.resize(size);
    }

    void randomize(value_t grow = 10) {
      m_array.randomize(grow);
    }

    measurement_t measure(std::double_t part) {
      m_tree.clear();
      auto first = m_array.cbegin();
      auto last = m_array.part_cend(part);

      for (auto i = first; i != last; std::advance(i, 1)) {
        m_tree.insert(*i);
      }

      rand_distrib_t distrib(*first, *std::prev(last));
      std::vector<value_t> search_vals(search_repeat_count);

      std::generate(search_vals.begin(), search_vals.end(), [this, &distrib]() {
        return distrib(m_rgen);
      });

      const auto arr_search_start = m_clock.now();
      for (const auto& val : search_vals) {
        std::binary_search(m_array.cbegin(), m_array.part_cend(part), val);
      }
      const auto arr_search_end = m_clock.now();

      const auto tree_search_start = m_clock.now();
      for (const auto& val : search_vals) {
        m_tree.search(val);
      }
      const auto tree_search_end = m_clock.now();

      return measurement_t {part,
                            ((arr_search_end - arr_search_start).count() / static_cast<rep_t>(search_repeat_count)),
                            ((tree_search_end - tree_search_start).count() / static_cast<rep_t>(search_repeat_count))};
    }

  private:
    size_t m_max_size;
    array_t m_array;
    tree_t m_tree;
    static const size_t search_repeat_count {100'000};

    rand_dev_t m_rdev;
    rand_gen_t m_rgen;

    clock_t m_clock;
  };
}