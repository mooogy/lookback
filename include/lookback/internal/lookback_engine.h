#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_ENGINE_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_ENGINE_H_

#include "lookback_backtest.h"
#include "lookback_data_stream.h"
#include "lookback_strategy.h"
#include "lookback_thread_pool.h"
#include <stdexcept>
#include <string>
#include <latch>
#include <iostream>

namespace lookback {

class BacktestingEngine {
 public:
  static BacktestingEngine& getEngine() {
    static BacktestingEngine instance;
    return instance;
  }

  template <FilePathLiteral Filepath, char Delimiter = ',', int BatchSize = 256>
  void addDataset() {
    dataStreams_.emplace(Filepath.value,
      std::make_shared<DataStream<Filepath, Delimiter, BatchSize>>());
  }

  template <FilePathLiteral Filepath, Strategy Strat>
  void createBacktest(std::string name, Strat strategy, BacktestSettings config = BacktestSettings{}) {
    if (!dataStreams_.contains(Filepath.value)) 
      throw std::runtime_error("File :" + std::string(Filepath.value) + " was not added to the engine");

    backtests_[Filepath.value].push_back(
      std::make_unique<Backtest<Strat>>(Backtest<Strat>(name, dataStreams_[Filepath.value], strategy, config))
    );
  }

  void run() {
    for (const auto& [filename, stream] : dataStreams_) {
      auto& associatedBacktests = backtests_[filename];
      int batches = 0;
      stream->prepareNextBars();

      while (!stream->empty()) {
        stream->commitNextBars();
        ++batches;

        std::latch batch_done(associatedBacktests.size());

        for (auto& backtest : associatedBacktests) {
          threadPool_.enqueueTask([&](){
            backtest->runBatch();
            batch_done.count_down();
          });
        }

        batch_done.wait();
        stream->prepareNextBars();
      }
    }
    printBacktestReports();
  }

 private:
  void printBacktestReports() const {
    for (const auto& [k, v] : backtests_) {
      for (const auto& backtest : v) {
        std::cout << "REPORT FOR \"" << backtest->getName() << "\"\n";
        backtest->printStats();
        std::cout << '\n';
      }
    }
  }

  ThreadPool threadPool_;
  std::unordered_map<std::string, std::shared_ptr<IDataStream>> dataStreams_;
  std::unordered_map<std::string, std::vector<std::unique_ptr<IBacktest>>> backtests_;
};
} // namespace: lookback

#endif
