#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <random>
#include <vector>

namespace arr {
template <std::integral ValT>
  requires std::totally_ordered<ValT>
class Arr {
public:
  using value_t = ValT;
  using value_array_t = std::vector<value_t>;
  using iterator_t = typename value_array_t::iterator;
  using const_iterator_t = typename value_array_t::const_iterator;

  using size_t = typename value_array_t::size_type;

  using rand_dev_t = std::random_device;
  using rand_gen_t = std::mt19937_64;
  using rand_distrib_t = std::uniform_int_distribution<value_t>;

  Arr(size_t size) : m_arr(size), m_rdev() {}

  void randomize(value_t grow) {
    m_rgen.seed(m_rdev());
    rand_distrib_t distribution(0, grow);

    m_arr.front() = distribution(m_rgen);

    for (auto i = std::next(m_arr.begin()); i != m_arr.end();
         std::advance(i, 1)) {
      *i = *std::prev(i) + distribution(m_rgen);
    }
  }

  iterator_t begin() { return m_arr.begin(); }
  iterator_t end() { return m_arr.end(); }

  const_iterator_t cbegin() const { return m_arr.cbegin(); }
  const_iterator_t cend() const { return m_arr.cend(); }

private:
  value_array_t m_arr;

  rand_dev_t m_rdev;
  rand_gen_t m_rgen;
};
} // namespace arr