#pragma once

#include <concepts>

namespace btree {
template <typename T>
concept BTreeKey = requires() { std::totally_ordered<T>; };
} // namespace btree