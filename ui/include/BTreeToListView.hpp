#pragma once

#include <concepts>
#include <iterator>
#include <qlayoutitem.h>
#include <qtreewidget.h>
#include <string>

#include "BTree.hpp"
#include "BTreeNode.hpp"

namespace ui {
static const QString key_item_name{"ключ"};
static const QString node_item_name{"узел"};
static const QString empty_placeholder{"пусто"};

template <std::totally_ordered ValT>
QTreeWidgetItem *leaf_to_treeitem(btree::BTreeNode<ValT> *node) {
  auto item = new QTreeWidgetItem();
  item->setText(0, QString::number(node->subtree_min()) + " - " +
                       QString::number(node->subtree_max()));
  item->setText(1, node_item_name);
  for (const auto &val : *node) {
    auto i = new QTreeWidgetItem();
    i->setText(0, QString::number(val));
    i->setText(1, key_item_name);
    item->addChild(i);
  }
  return item;
}

template <std::totally_ordered ValT>
QTreeWidgetItem *subtree_to_treeitem(btree::BTreeNode<ValT> *node) {
  if (node->leaf()) [[unlikely]] {
    return leaf_to_treeitem(node);
  }
  auto item = new QTreeWidgetItem();

  item->setText(0, QString::number(node->subtree_min()) + " - " +
                       QString::number(node->subtree_max()));
  item->setText(1, node_item_name);

  for (auto vali = node->begin(); vali != node->end(); ++vali) {
    auto left_child = node->left_child(vali);
    item->addChild(subtree_to_treeitem(left_child->get()));

    auto val = new QTreeWidgetItem();
    val->setText(0, QString::number(*vali));
    val->setText(1, key_item_name);
    item->addChild(val);
  }

  auto right_child = node->right_child(std::prev(node->end()));
  item->addChild(subtree_to_treeitem(right_child->get()));
  return item;
}

template <std::totally_ordered ValT>
QTreeWidgetItem *tree_to_treeitem(btree::BTree<ValT> &tree) {
  auto root = tree.root();
  if (root == nullptr) {
    auto item = new QTreeWidgetItem();
    item->setText(0, empty_placeholder);
    return item;
  }
  return subtree_to_treeitem(tree.root());
}

} // namespace ui