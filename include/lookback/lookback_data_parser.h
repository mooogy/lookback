#pragma once

#include <charconv>
#include <string>

namespace lookback {

using Date = std::string;
using Price = double;
using Volume = unsigned int;

struct OpenHighLowCloseVolume {
  Date date_;
  Price open_;
  Price high_;
  Price low_;
  Price close_;
  Volume volume_;
};

template <typename T>
concept DataStreamParser = requires(T parser, const std::string_view line) {
   { parser.processLine(line) } -> std::same_as<OpenHighLowCloseVolume>;
};

class CsvDataParser {
  public:
  OpenHighLowCloseVolume processLine(const std::string_view line) {
    OpenHighLowCloseVolume ohlcv;

    auto comma = line.find(',');
    ohlcv.date_ = std::string(line.begin(), line.begin() + comma);
    auto res = std::from_chars(line.data() + comma, line.end(), ohlcv.open_);
    res = std::from_chars(res.ptr, line.end(), ohlcv.high_);
    res = std::from_chars(res.ptr, line.end(), ohlcv.low_);
    res = std::from_chars(res.ptr, line.end(), ohlcv.close_);
    res = std::from_chars(res.ptr, line.end(), ohlcv.volume_);

    return ohlcv;
  }
};
} // namespace: lookback
