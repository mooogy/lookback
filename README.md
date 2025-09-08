# Lookback - Backtesting Library

## Welcome to Lookback

**Lookback** is a lightweight, header-only **C++20 backtesting library** designed for speed and flexibility.\
It leverages C++20 concepts to help ensure type-safe and robust code.

Lookback takes a modular approach, allowing you to plug in custom trading strategies and run them efficiently on historical datasets.\
This makes it easy to experiment with new ideas and optimize your trading algorithms.

## Features

- **Strategy-first Design**: Write and backtest custom trading strategies with a simple interface.
- **Batch Processing**: Efficient double buffer-based design for multi-threaded batch processing.
- **Compile-time Type Safety**: Concepts guide compatible custom strategy and parser creation.
- **Backtest Engine**: Manage datasets and run multiple strategies on the same data.

## Example Usage

```cpp
// Single header include
#include <lookback/lookback.h>

int main() {
    auto& engine = lookback::BacktestingEngine::getEngine();

    // Add datasets like this
    engine.addDataset<"nasdaq_futures.csv">();

    // Create new backtests for your custom strategies like this
    engine.createBacktest<"nasdaq_futures.csv">("SMA50", YourSMA{50});
    engine.createBacktest<"nasdaq_futures.csv">("SMA100", YourSMA{100});

    // Adjusts initial balance, commission rate, and slippage rate using a config object
    engine.createBacktest<"nasdaq_futures.csv">("SMA200", YourSMA{200}, {100'000, 0, 0});

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

## Installation

Lookback is a **header-only library**, so no compilation or linking is required. You can include it in your project using your preferred build system.

### 1. Using CMake `FetchContent`

Add Lookback as a dependency with CMake:
```cmake
include(FetchContent)

FetchContent_Declare(
  lookback
  GIT_REPOSITORY https://github.com/mooogy/lookback.git
  GIT_TAG main  # or a specific release tag
)

FetchContent_MakeAvailable(lookback)

target_link_libraries(YourApp PRIVATE Lookback::lookback)
```

### 2. Manual download

1. Download the `lookback` folder from the repository and place it in your project, e.g.:
    ```idk
    your_project/
    └── external/
        └── lookback/
            ├── lookback.h
            └── internal/
    ```
    
2. Add the folder to your include paths in your build system. For example, with CMake:
    ```cmake
    target_include_directories(YourApp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/lookback
    )
    ```

3. Then include it in your code:
    ```cpp
    #include <lookback/lookback.h>
    ```

## Contributing
Contributions are welcome! Here’s how to get started:
- Fork the repository and create a new branch for your feature or bugfix.
- Make sure your code follows the C++20 standard and matches the existing coding style.
- Submit a pull request with a clear description of your changes.

I appreciate contributions that improve:
- Performance and efficiency
- Readability and maintainability
- Documentation and examples

Have ideas or major changes? I'd love to hear from you! Please open an issue first to discuss your plan.
