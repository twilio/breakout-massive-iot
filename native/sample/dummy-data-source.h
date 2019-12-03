#ifndef DUMMY_DATA_SOURCE_H
#define DUMMY_DATA_SOURCE_H

#include <chrono>
#include <random>
#include <cmath>

template <typename DT>
class IDataSource {
 public:
  virtual bool ready() = 0;
  virtual DT data()    = 0;  // can throw or return arbitrary data if not ready
};

class DummyDataSource : IDataSource<float> {
 public:
  static constexpr float exp_int_s  = 5.0;  // expected data interval is 5s
  static constexpr float exp_data_s = 0.5;  // expected data value is 0.5
  DummyDataSource()
      : ts(std::chrono::system_clock::now()),
        time_distribution(0.0, 2 * exp_int_s),
        data_distribution(0.0, 2 * exp_data_s),
        next_data_in_s(time_distribution(generator)) {
  }

  bool ready() {
    std::chrono::duration<float> elapsed = std::chrono::system_clock::now() - ts;

    if (elapsed.count() >= next_data_in_s) {
      ts             = std::chrono::system_clock::now();
      next_data_in_s = time_distribution(generator);

      datapoint = data_distribution(generator);
    }

    return !std::isnan(datapoint);
  }

  float data() {
    float res = datapoint;

    if (!std::isnan(datapoint)) {
      datapoint = NAN;
    }

    return res;
  }

 private:
  float datapoint = NAN;
  std::default_random_engine generator;
  std::uniform_real_distribution<float> time_distribution;
  std::uniform_real_distribution<float> data_distribution;
  std::chrono::system_clock::time_point ts;
  float next_data_in_s;
};

#endif  // DUMMY_DATA_SOURCE_H
