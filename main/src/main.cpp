#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "Arr.hpp"
#include "BTree.hpp"

int main() {

  btree::BTree<std::uint64_t> tree(7);

  arr::Arr<std::uint64_t> arr(50);

  arr.randomize(3);

  /* for (std::size_t i = 1; i <= 1'000'000; ++i) {
     tree.insert(i);
   }*/

  tree.from_array_data(arr);

  for (std::size_t i = 0; i <= 100; ++i) {
    if (!tree.search(i)) {
      std::cout << i << " not found\n";
    }
  }

  /* tree.insert(1);
   tree.insert(2);
   tree.insert(3);
   tree.insert(4);
   tree.insert(5);
   tree.insert(6);
   tree.insert(7);
   tree.insert(8);*/

  return 0;
}