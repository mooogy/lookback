#ifndef LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_BACKTEST_H_
#define LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_BACKTEST_H_

#include "lookback/lookback_data_parser.h"
#include "lookback/lookback_data_stream.h"
#include "lookback/lookback_strategy.h"
#include <optional>

namespace lookback {

  class IBacktest {
   public:
    virtual void run() = 0;
    virtual ~IBacktest() = default;
  };

  template <TradingStrategy Strategy, DataStream DataStream>
  class Backtest : public IBacktest {
    public:
    Backtest(const std::string& name, DataStream& dataStream)
    : testName_(name), dataStream_(dataStream) {}

    void run() override {
      const auto& batch = dataStream_.getCurrentBatch();
      for (unsigned int i = 0; i < batch.size(); ++i) {
        currentBar = batch[i];
        std::optional<Order> order = strategy_.processOpenHighLowClose(currentBar);
        processOrder(order);
      }
    }

    private:

    void processOrder(const std::optional<Order>& order) {
      if (!order) return;

      switch (order->type_) {
        case OrderType::BUY:
        case OrderType::SELL:
          openPosition_ = (*order);
          break;
        case OrderType::CLOSE:
          if (openPosition_) {
            closeOpenPosition();
            openPosition_.reset();
          }
      }
    }

    void closeOpenPosition() {
      auto pnl = (*openPosition_->entryPrice_ - currentBar.close_) * *openPosition_->quantity_;

      if (openPosition_->type_ == OrderType::SELL) {
        pnl = -pnl; // short flips the sign
      }

      std::cout << "Closed " << (openPosition_->type_ == OrderType::BUY ? "LONG" : "SHORT")
            << " for P&L = " << pnl << "\n";
    }

    std::string testName_;
    Strategy strategy_;
    DataStream& dataStream_;

    OpenHighLowCloseVolume currentBar;
    std::optional<Order> openPosition_;
  };


} // namespace: lookback






#endif