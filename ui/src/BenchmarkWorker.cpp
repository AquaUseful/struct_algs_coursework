#include "BenchmarkWorker.hpp"

#include <cmath>
#include <qsplineseries.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <iostream>

ui::BenchmarkWorker::BenchmarkWorker(QObject* parent) : QObject(parent) {
}

void ui::BenchmarkWorker::startMeasurements(size_t size, order_t order) {
  m_benchmark.change_size(size);
  m_benchmark.change_order(order);
  m_benchmark.randomize();

  auto arr_series = new QtCharts::QSplineSeries {};
  auto tree_series = new QtCharts::QSplineSeries {};

  arr_series->setName("Массив");
  tree_series->setName("Дерево");

  const std::double_t step_grow = 0.01;
  for (std::double_t part = step_grow; part <= 1.0; part += step_grow) {
    const auto measurement = m_benchmark.measure(part);
    const auto arr_point = QPointF(measurement.filled_part, measurement.array_time);
    const auto tree_point = QPointF(measurement.filled_part, measurement.tree_time);
    arr_series->append(arr_point);
    tree_series->append(tree_point);
    emit measurementStepFinished(part, measurement.array_time, measurement.tree_time);
  }

  emit measurementsFinished(arr_series, tree_series);
}
