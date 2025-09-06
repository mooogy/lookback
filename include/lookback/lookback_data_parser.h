#pragma once

#include <algorithm>
#include <charconv>
#include <concepts>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <string>
#include <iostream>

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
    std::transform(lines.begin(), lines.end(), std::back_inserter(processedBars),
                  [delimiter](const std::string& line){ return CsvParser::processLine(line, delimiter); });
    return processedBars;
  }

 private:
  [[nodiscard]] static Bar processLine(std::string_view line, const char delimiter) {
    if (!line.empty() && (unsigned char)line[0] == 0xEF) {
      line.remove_prefix(3); // skip UTF-8 BOM
    }

    std::string_view cols[Bar::ENUM_LENGTH];
    unsigned int start = 0;
    unsigned int colIndex = 0;

    for (unsigned int i = 0; i <= line.size(); ++i) {
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
        return {
          std::string(cols[Bar::DATE]),
          *open,
          *high,
          *low,
          *close,
          *volume
        };
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