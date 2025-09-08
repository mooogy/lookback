# Lookback - Backtesting Library

# LOOKBACK IS STILL UNDER ACTIVE DEVELOPMENT

## Welcome to Lookback

**Lookback** is a lightweight, header-only **C++20 backtesting library** designed for speed and flexibility, utilizing C++20 concepts 
to ensure type-safe code.

Lookback takes a modular approach, allowing you to plug in custom trading strategies and run them efficiently on historical datasets.

## Example Usage

```cpp
int main() {
    auto& engine = lookback::BacktestingEngine::getEngine();

    // Register dataset (CSV parser chosen automatically by extension)
    engine.registerNewDataset("nasdaq.csv");

    // Add a backtest using an SMA30 strategy
    engine.createNewBacktest("SMA30 vs NASDAQ", "nasdaq.csv", SMA30{});

    // Run all backtests
    engine.run();
}
```
## Features

- **Strategy-first design**: Write trading strategies with a simple interface.
- **Compile-time safety**: Concepts enforce correct strategy and parser interfaces.
- **Batch processing**: Efficient buffer-based design for multi-threaded loading and processing.
- **Backtest engine**: Manage datasets and run multiple strategies on the same data.

