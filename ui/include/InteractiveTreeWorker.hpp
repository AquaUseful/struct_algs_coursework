#pragma once

#include <cstdint>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qthread.h>
#include <qtreewidget.h>

#include "BTree.hpp"

namespace ui {

  class InteractiveTreeWorker : public QObject {

    Q_OBJECT

  public:
    using tree_t = btree::BTree<std::int32_t>;
    using tree_reference_t = tree_t&;
    using value_t = typename tree_t::value_t;
    using order_t = typename tree_t::order_t;

  public:
    InteractiveTreeWorker(QObject* = nullptr);
    virtual ~InteractiveTreeWorker() = default;

  public slots:
    void insert(value_t);
    void erase(value_t);
    void replace(value_t, value_t);
    void search(value_t);
    void changeOrder(order_t);

  signals:
    void treeUpdated(QTreeWidgetItem*);
    void searchFinished(bool);

  private:
    QTreeWidgetItem* tree_to_item();

  private:
    tree_t m_tree;
    static constexpr order_t default_order {5};
  };

} // namespace ui