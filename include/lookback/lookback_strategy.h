#pragma once

#include <concepts>

enum class OrderOption {
  BUY,
  SELL,
  HOLD
};


template <typename T>
concept TradingStrategy = requires(T a) {
  { a.processBatch() } -> std::same_as<OrderOption>;
};
