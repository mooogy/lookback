#pragma once

#include "lookback/lookback_data_stream.h"
#include "lookback/lookback_strategy.h"
#include <memory>
#include <iostream>
#include <thread>

namespace lookback {

class IBacktest {
 public:
  void virtual runBatch() = 0;
  virtual ~IBacktest() = default;
};

template <Strategy Strat>
class Backtest : public IBacktest {
 public:
  Backtest(std::shared_ptr<IDataStream> dataStream, Strat strategy)
  : dataStream_(dataStream), strategy_(strategy) {}
  void runBatch() {
    auto order = strategy_.processBar(Bar{});
    std::cout << "PROCESSED A BATCH IN THREAD: " << std::this_thread::get_id() << '\n';
  }

 private:
  std::shared_ptr<IDataStream> dataStream_;
  Strat strategy_;
};

class OrderManager {
 public:
  OrderManager() {}
  void processOrder() {}
 private:
  void openPosition() {}
  void closePosition() {}

  Price initialBalance_;
  Price currentBalance_;

  double commissionRate_;
  double slippageRate_;
  Price totalCommission_;
  Price totalSlippage_;
};



} // namespace: lookback