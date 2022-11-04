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
