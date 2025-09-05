#ifndef LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_DATA_STREAM_H_
#define LOOKBACK_INCLUDE_LOOKBACK_LOOKBACK_DATA_STREAM_H_

#include <ios>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "lookback_data_parser.h"

namespace lookback {

template <typename T>
concept DataStream = requires(T stream) {
  { stream.prepareNextBatch() } -> std::same_as<void>;
  { stream.commitNextBatch() } -> std::same_as<void>;
  { stream.getCurrentBatch() } -> std::same_as<const std::vector<OpenHighLowCloseVolume>&>;
};

template<DataStreamParser Parser = CsvDataParser, int MaxBatchSize = 128>
class BasicDataStream {
 public:
  BasicDataStream(const std::string filename) : filename_(filename), file_(filename) {
    if (!file_.is_open()) throw std::ios_base::failure("Failed to open file: " + filename_);
    rawDataBuffer_.reserve(MaxBatchSize);
    processedDataBatch_.reserve(MaxBatchSize);
    processedDataBuffer_.reserve(MaxBatchSize);
  }

  const std::vector<OpenHighLowCloseVolume>& getCurrentBatch() const { return processedDataBatch_; }

  void prepareNextBatch() {
    fillRawDataBuffer();
    
    for (const std::string_view line : rawDataBuffer_) {
      processedDataBuffer_.emplace_back(parser_.processLine(line));
    }
  }

  void commitNextBatch() {
    processedDataBatch_.clear();
    processedDataBuffer_.swap(processedDataBatch_);
  }

  bool empty() const { return file_.eof(); }

 private:
  void fillRawDataBuffer() {
    rawDataBuffer_.clear();

    while (rawDataBuffer_.size() != rawDataBuffer_.capacity() && !file_.eof()) {
      std::string line;
      std::getline(file_, line);
      rawDataBuffer_.emplace_back(std::move(line));
    }
  }

  Parser parser_;
  std::string filename_;
  std::ifstream file_;
  std::vector<std::string> rawDataBuffer_;
  std::vector<OpenHighLowCloseVolume> processedDataBatch_;
  std::vector<OpenHighLowCloseVolume> processedDataBuffer_;
};
} // namespace: lookback

#endif