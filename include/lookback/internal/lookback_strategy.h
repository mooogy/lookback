#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_STRATEGY_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_STRATEGY_H_

#include <concepts>
#include <optional>
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

} // namespace: lookback
#endif