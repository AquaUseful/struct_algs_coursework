#pragma once

#include "ui_MainForm.h"

#include <cstdint>
#include <memory>
#include <qobjectdefs.h>
#include <qthread.h>
#include <qtreewidget.h>
#include <qwidget.h>

#include "InteractiveTreeWorker.hpp"
#include "BTree.hpp"

namespace ui {

  class MainForm : public QMainWindow {

    Q_OBJECT

  public:
    using tree_worker_t = InteractiveTreeWorker;
    using value_t = typename InteractiveTreeWorker::value_t;

  public:
    explicit MainForm(QWidget* = nullptr);
    virtual ~MainForm();

  private:
    void configure_slots();
    QThread* create_interactive_tree_thread();

  private slots:
    void updateTree(QTreeWidgetItem*);
    void interactiveInsert();
    void interactiveErase();
    void interacitveReplace();
    void interactiveSearch();

  signals:
    void interactiveInsert(value_t);
    void interactiveErase(value_t);
    void interacitveReplace(value_t, value_t);
    void interactiveSearch(value_t);

  private:
    std::unique_ptr<Ui::MainWindow> ui;
    QThread* m_tree_thread;
    tree_worker_t* m_tree_worker;
  };

} // namespace ui