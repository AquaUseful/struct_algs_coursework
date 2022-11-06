#pragma once

#include "ui_MainForm.h"

#include <memory>
#include <qthread.h>
#include <qwidget.h>

#include "BTree.hpp"

namespace ui {

class MainForm : public QMainWindow {

  Q_OBJECT

public:
  explicit MainForm(QWidget * = nullptr);
  virtual ~MainForm();

private:
  void configure_slots();

private slots:
  void add_value();

private:
  std::unique_ptr<Ui::MainWindow> ui;
  QThread m_interactive_tree;
};

} // namespace ui