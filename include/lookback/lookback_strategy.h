#pragma once

#include <concepts>
#include <deque>
#include <optional>
#include <numeric>
#include "lookback_data_parser.h"

namespace lookback {

struct Order {
  enum OrderSide {
    BUY,
    SELL,
    CLOSE
  };

  OrderSide side_;
  Date date_;
  Volume volume_; // if side_ = CLOSE, 
};

template <typename S>
concept Strategy = requires(S strat, const Bar& bar) {
  { strat.processBar(bar) } -> std::same_as<std::optional<Order>>;
};

class SMA30 {
 public:
  SMA30(std::size_t period = 30, Volume tradeVolume = 1)
      : period_(period), tradeVolume_(tradeVolume) {}

  std::optional<Order> processBar(const Bar& bar) {
    prices_.push_back(bar.close_);
    if (prices_.size() > period_) {
      prices_.pop_front();
    }

    // not enough data yet
    if (prices_.size() < period_) {
      return std::nullopt;
    }

    double sma = std::accumulate(prices_.begin(), prices_.end(), 0.0) / period_;

    std::optional<Order> order = std::nullopt;

    if (!hasPosition_) {
      if (bar.close_ > sma) {
        order = Order{Order::BUY, bar.date_, tradeVolume_};
        hasPosition_ = true;
        lastSide_ = Order::BUY;
      } else if (bar.close_ < sma) {
        order = Order{Order::SELL, bar.date_, tradeVolume_};
        hasPosition_ = true;
        lastSide_ = Order::SELL;
      }
    } else {
      if (lastSide_ == Order::BUY && bar.close_ < sma) {
        order = Order{Order::CLOSE, bar.date_, tradeVolume_};
        hasPosition_ = false;
      } else if (lastSide_ == Order::SELL && bar.close_ > sma) {
        order = Order{Order::CLOSE, bar.date_, tradeVolume_};
        hasPosition_ = false;
      }
    }

    return order;
  }

 private:
  std::size_t period_;
  Volume tradeVolume_;
  std::deque<double> prices_;
  bool hasPosition_{false};
  Order::OrderSide lastSide_;
};

}