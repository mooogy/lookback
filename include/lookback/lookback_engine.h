#pragma once

namespace lookback {

class BacktestingEngine {
 public:
  static BacktestingEngine& getEngine() {
    static BacktestingEngine instance;
    return instance;
  }


};

}