#pragma once

#include "lookback/lookback_data_parser.h"
#include <concepts>
#include <optional>
#include <deque>

namespace lookback {

enum class OrderType {
  LONG,
  SHORT,
  CLOSE
};

struct Order {
  Date date_;
  OrderType type_;
  std::optional<Volume> quantity_;
};

template <typename T>
concept TradingStrategy = requires(T a, const Bar& ohlc, Price bal) {
  { a.processBar(ohlc, bal) } -> std::same_as<std::optional<Order>>;
};

class SMA30 {
  static constinit const unsigned int periodTimeframe = 30;

  public:
  SMA30() {
    closePoints_.resize(periodTimeframe);
  }

  std::optional<Order> processBar(const Bar& ohlc, Price bal) {
    Price previousDayClose;
    if (!closePoints_.empty()) previousDayClose = closePoints_.front();

    closePoints_.push_front(ohlc.close_);
    cumulativeSum_ += ohlc.close_;

    if (closePoints_.size() < periodTimeframe) {
      return std::nullopt;
    }

    const auto sma30 = cumulativeSum_ / periodTimeframe;

    cumulativeSum_ -= closePoints_.back();
    closePoints_.pop_back();

    if (!isLong_ && ohlc.close_ > sma30) {
      isLong_ = true;
      return std::make_optional<Order>(ohlc.date_, OrderType::LONG, 100);
    }

    if (!isShort_ && ohlc.close_ < sma30) {
      isShort_ = true;
      return std::make_optional<Order>(ohlc.date_, OrderType::SHORT, 100);
    }

    if (isLong_ && ohlc.close_ < previousDayClose) {
      isLong_ = false;
      return std::make_optional<Order>(ohlc.date_, OrderType::CLOSE);
    }

    if (isShort_ && ohlc.close_ > previousDayClose) {
      isShort_ = false;
      return std::make_optional<Order>(ohlc.date_, OrderType::CLOSE);
    }

    return std::nullopt;
  }

  private:
  std::deque<Price> closePoints_;
  Price cumulativeSum_ = 0;
  bool isLong_ = false;
  bool isShort_ = false;
};

}