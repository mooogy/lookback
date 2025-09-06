#pragma once

#include "lookback/lookback_data_stream.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace lookback {

class BacktestingEngine {
 public:
  static BacktestingEngine& getEngine() {
    static BacktestingEngine instance;
    return instance;
  }

  template <FilePathLiteral Filepath, char Delimiter, size_t BatchSize>
  void registerDataStream() {
    dataStreams_.emplace(Filepath.value,
      std::make_unique<DataStream<Filepath, Delimiter, BatchSize>>());
  }
  std::unordered_map<std::string, std::unique_ptr<IDataStream>> dataStreams_;
};

}