#ifndef LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_THREAD_POOL_H_
#define LOOKBACK_LOOKBACK_INCLUDE_INTERNAL_LOOKBACK_THREAD_POOL_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>

namespace lookback {

class ThreadPool {
 public: 
  ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) {
    pool_.reserve(numThreads);

    for (std::size_t i = 0; i < numThreads; ++i) {
      pool_.emplace_back([this](){
        while (true) {
          std::function<void()> task;

          {
            std::unique_lock<std::mutex> lock(queueMutex_);
            taskReadySignal_.wait(lock, [this](){
              return !tasks_.empty() || stop_;
            });
           
            if (stop_ && tasks_.empty()) [[unlikely]] {
              return;
            }
           
            task = std::move(tasks_.front());
            tasks_.pop();
          }

          task();
        }
      });
    }
  };

  void enqueueTask(std::function<void()> task) {
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      tasks_.emplace(task);
    }
    taskReadySignal_.notify_one();
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      stop_ = true;
    }

    taskReadySignal_.notify_all();
    for (auto& thread : pool_) {
      thread.join();
    }
  }

 private:
  std::vector<std::thread> pool_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queueMutex_;
  std::condition_variable taskReadySignal_;
  bool stop_ = false;
};
} // namespace: lookback
#endif 