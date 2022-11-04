#include <iostream>
#include <memory>

#include "BTreeNode.hpp"

int main() {

  auto node = std::make_unique<btree::BTreeNode<int, 3>>();

  node->insert(1);
  node->insert(2);
  node->insert(3);
  node->insert(4);
  node->insert(5);
  node->insert(6);
  node->insert(7);
  node->insert(8);

  return 0;
}