#pragma once

#include <cstddef>
#include <cstdint>
#include <qsplineseries.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <qlineseries.h>
#include <qchart.h>

#include "Benchmark.hpp"

namespace ui {
  class BenchmarkWorker : public QObject {

    Q_OBJECT

  public:
    using value_t = std::int32_t;
    using benchmark_t = benchmark::Benchmark<value_t>;
    using order_t = typename benchmark_t::order_t;
    using size_t = typename benchmark_t::size_t;

  public:
    BenchmarkWorker(QObject* = nullptr);
    virtual ~BenchmarkWorker() = default;

  signals:
    void measurementsFinished(QtCharts::QSplineSeries*, QtCharts::QSplineSeries*);

  public slots:
    void startMeasurements(size_t, order_t);

  private:
    benchmark_t m_benchmark;
  };
}