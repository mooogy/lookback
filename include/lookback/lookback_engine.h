#pragma once

#include "lookback/lookback_backtest.h"
#include "lookback/lookback_data_stream.h"
#include "lookback/lookback_strategy.h"
#include "lookback/lookback_thread_pool.h"
#include <memory>
#include <latch>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <vector>

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
  void createBacktest(Strat strategy) {
    if (!dataStreams_.contains(Filepath.value)) 
      throw std::runtime_error("File :" + std::string(Filepath.value) + " was not added to the engine");

    backtests_[Filepath.value].push_back(
      std::make_unique<Backtest<Strat>>(Backtest<Strat>(dataStreams_[Filepath.value], strategy))
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
  }

 private:
  unsigned int batchSize_ = 128;
  ThreadPool threadPool_;
  std::unordered_map<std::string, std::shared_ptr<IDataStream>> dataStreams_;
  std::unordered_map<std::string, std::vector<std::unique_ptr<IBacktest>>> backtests_;
};

}
