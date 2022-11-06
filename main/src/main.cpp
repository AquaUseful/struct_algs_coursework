#include <cstddef>
#include <iostream>
#include <memory>

#include "BTree.hpp"

int main() {

  btree::BTree<int> tree(8);

  for (std::size_t i = 1; i <= 1'000'000; ++i) {
    tree.insert(i);
  }

for (std::size_t i = 0; i <= 1'000'000 +10; ++i) {
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