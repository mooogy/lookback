#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_DATA_PARSER_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_DATA_PARSER_H_

#include <charconv>
#include <concepts>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <string>

namespace lookback {
using Date = std::string;
using Price = double;
using Volume = unsigned int;

struct Bar {
  enum : char {
    DATE,
    OPEN,
    HIGH,
    LOW,
    CLOSE,
    VOLUME,
    ENUM_LENGTH
  };

  Date date_;
  Price open_;
  Price high_;
  Price low_;
  Price close_;
  Volume volume_;
};

using Bars = std::vector<Bar>;

template <typename T>
concept Parser = requires(const std::vector<std::string>& lines, const char delimiter) {
  { T::linesToBars(lines, delimiter) } -> std::same_as<Bars>;
};

class CsvParser {
 public:
  [[nodiscard]] static Bars linesToBars(const std::vector<std::string>& lines, const char delimiter) {
    Bars processedBars;
    processedBars.reserve(lines.size());
    for (const auto& line : lines) {
        processedBars.push_back(CsvParser::processLine(line, delimiter));
    }
    return processedBars;
  }

 private:
  [[nodiscard]] static Bar processLine(std::string_view line, const char delimiter) {
    std::string_view cols[Bar::ENUM_LENGTH];
    size_t start = 0;
    size_t colIndex = 0;

    for (size_t i = 0; i <= line.size(); ++i) {
      if (i == line.size() || line[i] == delimiter) {
        if (colIndex >= Bar::ENUM_LENGTH) break;

        cols[colIndex] = line.substr(start, i - start);
        ++colIndex;
        start = i + 1;
      }
    }

    if (colIndex == Bar::ENUM_LENGTH) {
      const auto open = convert<Price>(cols[Bar::OPEN]);
      const auto high = convert<Price>(cols[Bar::HIGH]);
      const auto low = convert<Price>(cols[Bar::LOW]);
      const auto close = convert<Price>(cols[Bar::CLOSE]);
      const auto volume = convert<Volume>(cols[Bar::VOLUME]);

      if (open && high && low && close && volume) {
        return Bar(
          std::string(cols[Bar::DATE]),
          *open,
          *high,
          *low,
          *close,
          *volume
        );
      }

    }
    throw std::runtime_error("Could not convert Bar from: " + std::string(line));
  }

  template <typename T>
  [[nodiscard]] static std::optional<T> convert(std::string_view field) noexcept {
    T value{};
    const auto res = std::from_chars(field.begin(), field.end(), value);
    if (res.ec == std::errc{} && res.ptr == field.end()) {
      return value;
    }
    return std::nullopt;
  }
};
} // namespace: lookback
#endif