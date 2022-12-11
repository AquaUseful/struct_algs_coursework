#include "MainForm.hpp"
#include "ui_MainForm.h"

#include <cstddef>
#include <memory>
#include <qmainwindow.h>
#include <qobjectdefs.h>
#include <qthread.h>
#include <qwidget.h>

#include "BTreeToListView.hpp"
#include "InteractiveTreeWorker.hpp"

ui::MainForm::MainForm(QWidget* parent)
  : QMainWindow(parent),
    ui {std::make_unique<Ui::MainWindow>()},
    m_tree_thread {new QThread(this)},
    m_tree_worker(new InteractiveTreeWorker()) {
  ui->setupUi(this);
  ui->retranslateUi(this);

  m_tree_worker->moveToThread(m_tree_thread);

  configure_slots();

  m_tree_thread->start();
}

ui::MainForm::~MainForm() {
  m_tree_thread->quit();
  m_tree_thread->wait();
}

void ui::MainForm::configure_slots() {
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(interactiveInsert()));
  connect(ui->delButton, SIGNAL(clicked()), this, SLOT(interactiveErase()));
  connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(interacitveReplace()));
  connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(interactiveSearch()));

  qRegisterMetaType<value_t>("value_t");
  connect(this, SIGNAL(interactiveInsert(value_t)), m_tree_worker, SLOT(insert(value_t)));
  connect(this, SIGNAL(interactiveErase(value_t)), m_tree_worker, SLOT(erase(value_t)));
  connect(this, SIGNAL(interacitveReplace(value_t, value_t)), m_tree_worker, SLOT(replace(value_t, value_t)));

  connect(m_tree_worker, SIGNAL(treeUpdated(QTreeWidgetItem*)), this, SLOT(updateTree(QTreeWidgetItem*)));
}

void ui::MainForm::interactiveInsert() {
  emit interactiveInsert(ui->addVal->value());
}

void ui::MainForm::interactiveErase() {
  emit interactiveErase(ui->delVal->value());
}

void ui::MainForm::interacitveReplace() {
  emit interacitveReplace(ui->replFromVal->value(), ui->replToVal->value());
}

void ui::MainForm::interactiveSearch() {
  emit interactiveSearch(0);
}

void ui::MainForm::updateTree(QTreeWidgetItem* item) {
  ui->visualisation->clear();
  ui->visualisation->addTopLevelItem(item);
  ui->visualisation->expandAll();
}
