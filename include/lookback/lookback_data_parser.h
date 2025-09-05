#pragma once

#include <charconv>
#include <string>

namespace lookback {

using Date = std::string;
using Price = double;
using Volume = unsigned int;

struct Bar {
  Date date_;
  Price open_;
  Price high_;
  Price low_;
  Price close_;
  Volume volume_;
};

template <typename T>
concept DataStreamParser = requires(T parser, const std::string_view line) {
   { parser.processLine(line) } -> std::same_as<Bar>;
};

class CsvDataParser {
  public:
  Bar processLine(const std::string_view line) {
    Bar ohlcv;

    auto comma = line.find(',');
    ohlcv.date_ = std::string(line.begin(), line.begin() + comma);
    auto res = std::from_chars(line.data() + comma + 1, line.end(), ohlcv.open_);
    res = std::from_chars(res.ptr + 1, line.end(), ohlcv.high_);
    res = std::from_chars(res.ptr + 1, line.end(), ohlcv.low_);
    res = std::from_chars(res.ptr + 1, line.end(), ohlcv.close_);
    res = std::from_chars(res.ptr + 1, line.end(), ohlcv.volume_);

    return ohlcv;
  }
};
} // namespace: lookback
