#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_DATA_STREAM_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_DATA_STREAM_H_

#include "lookback_data_parser.h"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace lookback {

template <size_t N>
struct FilePathLiteral {
  consteval FilePathLiteral(const char (&str)[N]) {
    std::copy_n(str, N, value);
  }

  char value[N];
};

class IDataStream {
  public:
  virtual const Bars& getCurrentBars() const = 0;
  virtual void commitNextBars() = 0;
  virtual void prepareNextBars() = 0;
  virtual bool empty() = 0;
  ~IDataStream() = default;
};

template <FilePathLiteral Filepath, char Delimiter, size_t BatchSize>
class DataStream : public IDataStream {
  public:
  using FileParser = 
  std::conditional_t<static_cast<std::string_view>(Filepath.value).ends_with(".csv"), CsvParser, void>;

  DataStream() {
    if (!fs::exists(static_cast<fs::path>(Filepath.value))) {
      throw std::runtime_error("File: " + static_cast<fs::path>(Filepath.value).string() + " could not be found.");
    }
    file_ = std::ifstream{Filepath.value};
    rawBarsBuffer_.reserve(BatchSize);
    preparedBarsBuffer_.reserve(BatchSize);
    currentBarsBuffer_.reserve(BatchSize);
  }

  [[nodiscard]] const Bars& getCurrentBars() const override {
    return currentBarsBuffer_;
  }

  void prepareNextBars() override {
    static_assert(!std::same_as<FileParser, void>, "Currently supported file types: .csv");

    rawBarsBuffer_.clear();

    std::string line;
    while (rawBarsBuffer_.size() != rawBarsBuffer_.capacity() && getline(file_, line)) {
      rawBarsBuffer_.push_back(line);
    }

    preparedBarsBuffer_ = FileParser::linesToBars(rawBarsBuffer_, Delimiter);
  }

  void commitNextBars() override {
    currentBarsBuffer_ = std::move(preparedBarsBuffer_);
  }

  [[nodiscard]] bool empty() override {
    return file_.eof();
  }

  private:
  std::ifstream file_;

  std::vector<std::string> rawBarsBuffer_;
  Bars preparedBarsBuffer_;
  Bars currentBarsBuffer_;
};
} // namespace: lookback
#endif