#include "InteractiveTreeWorker.hpp"

#include <qobject.h>
#include <qobjectdefs.h>
#include <qtreewidget.h>

#include "BTreeToListView.hpp"

ui::InteractiveTreeWorker::InteractiveTreeWorker(QObject* parent) : QObject(parent), m_tree {default_order} {
}

void ui::InteractiveTreeWorker::insert(value_t value) {
  m_tree.insert(value);
  emit treeUpdated(tree_to_item());
}

void ui::InteractiveTreeWorker::erase(value_t value) {
  m_tree.erase(value);
  emit treeUpdated(tree_to_item());
}

void ui::InteractiveTreeWorker::replace(value_t from, value_t to) {
  if (m_tree.search(from)) {
    m_tree.erase(from);
    m_tree.insert(to);
  }
  emit treeUpdated(tree_to_item());
}

void ui::InteractiveTreeWorker::search(value_t value) {
  emit searchFinished(m_tree.search(value));
}

void ui::InteractiveTreeWorker::changeOrder(order_t order) {
  m_tree = tree_t(order);
  emit treeUpdated(tree_to_item());
}

QTreeWidgetItem* ui::InteractiveTreeWorker::tree_to_item() {
  return ui::tree_to_treeitem(m_tree);
}
