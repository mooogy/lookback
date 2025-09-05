#ifndef LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_BACKTEST_H_
#define LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_BACKTEST_H_

#include "lookback/lookback_data_parser.h"
#include "lookback/lookback_data_stream.h"
#include "lookback/lookback_strategy.h"
#include <optional>
#include <format>
#include <vector>

namespace lookback {

  class IBacktest {
   public:
    virtual void run() = 0;
    virtual ~IBacktest() = default;
  };

  struct Position {
    Order order_;
    Price entryPrice_;
  };

  struct EntryExitPairs {
    Date entryDate_;
    Date exitDate_;
    OrderType side_;
    Price returns;
    void log() const {
      std::cout << std::format("{}: \t{} -> {} | PROFIT/LOSS: {}\n",
      (side_ == OrderType::LONG ? "LONG" : "SHORT"), exitDate_, entryDate_, returns);
    }
  };

  template <TradingStrategy Strategy, DataStream DataStream>
  class Backtest : public IBacktest {
    public:
    Backtest(const std::string& name, DataStream& dataStream)
    : testName_(name), dataStream_(dataStream) {}

    void run() override {
      const auto& batch = dataStream_.getCurrentBatch();
      for (unsigned int i = 0; i < batch.size(); ++i) {
        const Bar& currentBar = batch[i];
        std::optional<Order> order = strategy_.processBar(currentBar, balance_);
        if (order) processOrder(*order, currentBar.close_);
      }
    }

    void printLogs() {
      for (auto iter = positionLog_.rbegin(); iter != positionLog_.rend(); ++iter) {
        iter->log();
      }
    }

    private:

    void processOrder(const Order& order, Price currentPrice) {
      auto slippage = slippageRate_ * currentPrice;

      switch (order.type_) {
        case OrderType::LONG:
        case OrderType::SHORT: {
          if (hasOpenPosition_) closeCurrentPosition(order.date_, currentPrice);

          Price totalPositionCost = (*order.quantity_ * currentPrice);

          totalPositionCost -= slippage;
          totalSlippage_ += slippage;

          Price commision = totalPositionCost * commisionRate_;
          totalCommision_ += commision;

          openPosition_ = Position(order, currentPrice);
          break;
        }
        case OrderType::CLOSE:
        closeCurrentPosition(order.date_, currentPrice);
      }
    }

    void closeCurrentPosition(const std::string& date, Price closingPrice) {
        auto slippage = slippageRate_ * closingPrice;
        Order& orderToClose = openPosition_.order_;
        Price closingPositionTotal = (*orderToClose.quantity_ * closingPrice);

        closingPositionTotal -= slippage;
        totalSlippage_ += slippage;

        Price commision = closingPositionTotal * commisionRate_;
        totalCommision_ += commision;

        auto returns = (*orderToClose.quantity_ * openPosition_.entryPrice_) - closingPositionTotal;

        if (orderToClose.type_ == OrderType::SHORT) {
          returns = -returns;
        }

        positionLog_.emplace_back(orderToClose.date_, date, orderToClose.type_, returns);

        pnl += returns;
    }

    std::string testName_;
    Strategy strategy_;
    DataStream& dataStream_;
    bool hasOpenPosition_ = false;
    Position openPosition_;
    std::vector<EntryExitPairs> positionLog_;


    Price pnl = 0.0;
    double initialCash_ = 100'000.00;
    double balance_ = initialCash_;
    double commisionRate_ = 0.0;
    double slippageRate_ = 0.0;
    Price totalCommision_ = 0.0;
    Price totalSlippage_ = 0.0;
  };


} // namespace: lookback






#endif