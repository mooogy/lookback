#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_ORDER_MANAGER_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_ORDER_MANAGER_H_

#include "lookback_data_parser.h"
#include "lookback_strategy.h"
#include <format>
#include <iostream>

namespace lookback {

class OrderManager {
public:
    OrderManager(Price initialBalance, double commissionRate, double slippageRate)
      : initialBalance_(initialBalance),
        currentBalance_(initialBalance),
        commissionRate_(commissionRate),
        slippageRate_(slippageRate),
        totalCommission_(0),
        totalSlippage_(0) {}

    void processOrder(const Order& order, const Bar& currentBar) {
        if (!openPosition_) {
          openPosition(order, currentBar);
        } else {
          closePosition(order, currentBar);
          if (order.side_ != Order::OrderSide::CLOSE) {
            openPosition(order, currentBar);
          }
        }
    }

    void printStats() const {
      std::cout << std::format(
        "\tINITIAL BALANCE: {:.2f}\n\tTOTAL COMMISSION: {:.2f}\n\tTOTAL SLIPPAGE: {:.2f}\n\tENDING BALANCE: {:.2f}\n\tTOTAL PROFIT/LOSS: {:.2f}\n",
      initialBalance_, totalCommission_, totalSlippage_, currentBalance_, currentBalance_ - initialBalance_);
    }

private:
    void openPosition(const Order& order, const Bar& bar) {
        Price executedPrice = applySlippage(order.side_, bar.close_);

        Price orderValue = executedPrice * order.volume_;
        Price commission = orderValue * commissionRate_;
        totalCommission_ += commission;

        currentBalance_ -= orderValue + commission;

        totalSlippage_ += std::abs(executedPrice - bar.close_) * order.volume_;

        openPosition_ = order;
        entryPrice_ = executedPrice;
    }

    void closePosition(const Order& order, const Bar& bar) {
        auto closePositionSide = 
        (openPosition_->side_ == Order::OrderSide::BUY ? Order::OrderSide::SELL : Order::OrderSide::BUY);

        Price executedPrice = applySlippage(closePositionSide, bar.close_);
        Price profit = executedPrice * openPosition_->volume_;

        Price commission = profit * commissionRate_;
        totalCommission_ += commission;

        currentBalance_ += profit - commission;

        totalSlippage_ += std::abs(executedPrice - bar.close_) * openPosition_->volume_;

        openPosition_.reset();
    }

    [[nodiscard]]
    Price applySlippage(Order::OrderSide side, Price basePrice) const {
        if (side == Order::BUY) {
            return basePrice * (1.0 + slippageRate_);
        } else if (side == Order::SELL) {
            return basePrice * (1.0 - slippageRate_);
        }
        throw std::runtime_error("Order side not recognized\n");
    }

    std::optional<Order> openPosition_;
    Price entryPrice_ = 0;

    Price initialBalance_;
    Price currentBalance_;

    double commissionRate_;
    double slippageRate_;
    Price totalCommission_;
    Price totalSlippage_;
};
} // namespace: lookback
#endif