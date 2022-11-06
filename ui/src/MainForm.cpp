#include "MainForm.hpp"
#include "ui_MainForm.h"
#include <memory>
#include <qmainwindow.h>
#include <qwidget.h>

ui::MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent), ui{std::make_unique<Ui::MainWindow>()} {
  ui->setupUi(this);
  ui->retranslateUi(this);
}

ui::MainForm::~MainForm() {
  m_interactive_tree.quit();
  m_interactive_tree.wait();
}

void ui::MainForm::configure_slots() {
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add_value()));
}

void ui::MainForm::add_value() {}