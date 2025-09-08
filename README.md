# Lookback - Backtesting Library

## Welcome to Lookback

**Lookback** is a lightweight, header-only **C++20 backtesting library** designed for speed and flexibility, it leverages C++20 concepts 
to ensure type-safe and robust code.

Lookback takes a modular approach, allowing you to plug in custom trading strategies and run them efficiently on historical datasets, 
making it easy to experiment with new ideas and optimize your trading algorithms.

## Features

- **Strategy-first design**: Write and backtest custom trading strategies with a simple interface.
- **Batch processing**: Efficient double buffer-based design for multi-threaded processing.
- **Compile-time safety**: Concepts enforce correct strategy and parser creation.
- **Backtest engine**: Manage datasets and run multiple strategies on the same data.

## Example Usage

```cpp
int main() {
    // Single header include
    #include <lookback/lookback.h>

    auto& engine = lookback::BacktestingEngine::getEngine();

    // Add datasets like this
    engine.addDataset<"nasdaq_futures.csv">();

    // Create new backtests for your custom strategies like this
    engine.createBacktest<"nasdaq_futures.csv">("SMA50", SMA{50});
    engine.createBacktest<"nasdaq_futures.csv">("SMA100", SMA{100});

    // Adjusts initial balance, commission rate, and slippage rate using a config object
    engine.createBacktest<"nasdaq_futures.csv">("SMA200", SMA{200}, {100'000, 0, 0});

    // Run all your backtests and have the results printed
    engine.run();

    /* EXAMPLE OUTPUT
        REPORT FOR "SMA200"
                INITIAL BALANCE: 50000.00
                TOTAL COMMISSION: 0.00
                TOTAL SLIPPAGE: 0.00
                ENDING BALANCE: 51332.75
                TOTAL PROFIT/LOSS: 1332.75
    */
}
```