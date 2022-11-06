#pragma once

#include <qobject.h>
#include <qobjectdefs.h>
#include <qthread.h>

#include "BTree.hpp"

namespace ui {

class InteractiveTreeWorker : public QObject {

  Q_OBJECT

public slots:
  void add(const int &);
  void handleAddResult();

signals:
  void addFinished();

private:
  btree::BTree<int, 5> m_tree;
};

} // namespace ui