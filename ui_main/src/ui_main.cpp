#include <qapplication.h>

#include "MainForm.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ui::MainForm main_form{};

  main_form.show();
  return app.exec();
}