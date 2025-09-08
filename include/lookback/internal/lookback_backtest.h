#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_BACKTEST_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_BACKTEST_H_

#include "lookback_data_parser.h"
#include "lookback_data_stream.h"
#include "lookback_strategy.h"
#include "lookback_order_manager.h"
#include <memory>

namespace lookback {

class IBacktest {
 public:
  virtual void runBatch() = 0;
  virtual const std::string& getName() const = 0;
  virtual void printStats() const = 0;
  virtual ~IBacktest() = default;
};

struct BacktestSettings {
  Price initialBalance_ = 50'000;
  double commissionRate_ = 0;
  double slippageRate_ = 0;
};

template <Strategy Strat>
class Backtest : public IBacktest {
 public:
  Backtest(
    std::string testName,
    std::shared_ptr<IDataStream> dataStream,
    Strat strategy,
    BacktestSettings settings
  )
  : name_(testName),
    dataStream_(dataStream), 
    strategy_(strategy),
    orderManager_(settings.initialBalance_, settings.commissionRate_, settings.slippageRate_)
    {}

  void runBatch() override {
    for (const Bar& bar : dataStream_->getCurrentBars()) {
      std::optional<Order> order = strategy_.processBar(bar);
      if (order) orderManager_.processOrder(*order, bar);
    }
  }

  const std::string& getName() const override {
    return name_;
  }

  void printStats() const override {
    orderManager_.printStats();
  }

 private:
  std::string name_;
  OrderManager orderManager_;
  std::shared_ptr<IDataStream> dataStream_;
  Strat strategy_;
};
} // namespace: lookback
#endif