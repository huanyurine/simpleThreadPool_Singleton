#include "ThreadPool.h"

std::once_flag ThreadPool::once_flag_;
std::unique_ptr<ThreadPool> ThreadPool::instance_;

ThreadPool::ThreadPool(int numThreads) : stop_(false) {
    for (int i = 0; i < numThreads; ++i) {
        this->threads_.emplace_back([this] {
            while (true) {
                std::unique_lock<std::mutex> lock(this->mtx_);
                this->cv_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
                if (this->stop_ && this->tasks_.empty()) {
                    return;
                }
                std::function<void()> task(std::move(this->tasks_.front()));
                this->tasks_.pop();
                lock.unlock();
                task();
            }
            }
        );
    }
}
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(this->mtx_);
        this->stop_ = true;
    }
    this->cv_.notify_all();
    for (std::thread& thread_: this->threads_) {
        thread_.join();
    }
}
void ThreadPool::get_id() {
    std::cout << "object's this  addr: " << this << std::endl;
}
ThreadPool& ThreadPool::getInstance(int numThreads) {
    std::call_once(once_flag_, &ThreadPool::init, numThreads);
    return *instance_;
}
void ThreadPool::init(int numThreads) {
    instance_.reset(new ThreadPool(numThreads));
}
ThreadPool& get_instance(int numThreads) {
    return ThreadPool::getInstance(numThreads);
}
