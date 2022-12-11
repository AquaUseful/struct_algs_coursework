#include "MainForm.hpp"
#include "BenchmarkWorker.hpp"
#include "ui_MainForm.h"

#include <cstddef>
#include <memory>
#include <qboxlayout.h>
#include <qchart.h>
#include <qchartview.h>
#include <qlayout.h>
#include <qlineseries.h>
#include <qmainwindow.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <qsplineseries.h>
#include <qthread.h>
#include <qwidget.h>

#include <iostream>

#include "BTreeToListView.hpp"
#include "InteractiveTreeWorker.hpp"

ui::MainForm::MainForm(QWidget* parent)
  : QMainWindow(parent),
    ui {std::make_unique<Ui::MainWindow>()},
    m_tree_thread {new QThread(this)},
    m_tree_worker(new InteractiveTreeWorker()),
    m_bench_thread {new QThread(this)},
    m_bench_worker(new BenchmarkWorker()) {
  ui->setupUi(this);
  ui->retranslateUi(this);

  m_tree_worker->moveToThread(m_tree_thread);
  m_bench_worker->moveToThread(m_bench_thread);

  configure_slots();

  // m_chart->addSeries(m_tree_series);
  // m_chart->createDefaultAxes();

  m_tree_thread->start();
  m_bench_thread->start();
}

ui::MainForm::~MainForm() {
  m_tree_thread->quit();
  m_tree_thread->wait();

  m_bench_thread->quit();
  m_tree_thread->wait();
}

void ui::MainForm::configure_slots() {
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(interactiveInsert()));
  connect(ui->delButton, SIGNAL(clicked()), this, SLOT(interactiveErase()));
  connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(interacitveReplace()));
  connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(interactiveSearch()));

  connect(ui->updateChart, SIGNAL(clicked()), this, SLOT(updateChart()));

  qRegisterMetaType<value_t>("value_t");
  connect(this, SIGNAL(interactiveInsert(value_t)), m_tree_worker, SLOT(insert(value_t)));
  connect(this, SIGNAL(interactiveErase(value_t)), m_tree_worker, SLOT(erase(value_t)));
  connect(this, SIGNAL(interacitveReplace(value_t, value_t)), m_tree_worker, SLOT(replace(value_t, value_t)));

  qRegisterMetaType<order_t>("order_t");
  qRegisterMetaType<size_t>("size_t");
  connect(this, SIGNAL(startChartWorker(size_t, order_t)), m_bench_worker, SLOT(startMeasurements(size_t, order_t)));

  connect(m_tree_worker, SIGNAL(treeUpdated(QTreeWidgetItem*)), this, SLOT(updateTree(QTreeWidgetItem*)));
  connect(m_bench_worker,
          SIGNAL(measurementsFinished(QtCharts::QSplineSeries*, QtCharts::QSplineSeries*)),
          this,
          SLOT(addSeries(QtCharts::QSplineSeries*, QtCharts::QSplineSeries*)));
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

void ui::MainForm::updateChart() {
  std::cout << "update chart" << '\n';
  const size_t size = ui->maxElements->value();
  const order_t order = ui->treeOrder->value();
  emit startChartWorker(size, order);
}

void ui::MainForm::addSeries(QtCharts::QSplineSeries* arr_series, QtCharts::QSplineSeries* tree_series) {
  std::cout << "got series" << '\n';
  auto chart = new QtCharts::QChart();
  chart->addSeries(arr_series);
  chart->addSeries(tree_series);
  chart->createDefaultAxes();

  //auto chart_view = new QtCharts::QChartView(chart);
  ui->chart->setChart(chart);
  //auto layout = ui->chartTab->layout();
  
 // layout->addWidget(chart_view);
}