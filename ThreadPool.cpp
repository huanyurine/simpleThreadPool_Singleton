#include "ThreadPool.h"

std::once_flag ThreadPool::thread_pool_once_flag_;
std::unique_ptr<ThreadPool> ThreadPool::thread_pool_instance_;

ThreadPool::ThreadPool(int numThreads) : stop_(false) {
    for (int i = 0; i < numThreads; ++i) {
        this->threads_.emplace_back([this] {    //用一个lambda函数作为 threads_ 的输入，创建单个线程
            while (true) {
                std::unique_lock<std::mutex> lock(this->mtx_);
                this->cv_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });  //当线程被阻塞时，wait()自动调用std::mutex的unlock()释放锁；当被阻塞线程获得通知(notify_one或notify_all)，wait()自动调用std::mutex的lock()。
                if (this->stop_ && this->tasks_.empty()) { return; }
                std::function<void()> task(std::move(this->tasks_.front()));
                //this->get_info();   //测试时信息
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
    for (std::thread& thread_ : this->threads_) { thread_.join(); }
}
void ThreadPool::get_info() {
    std::cout << " tasks_.size() = " << this->tasks_.size() << " is running in thread " << std::this_thread::get_id() << std::endl;
}
ThreadPool& ThreadPool::getInstance(int numThreads) {
    std::call_once(thread_pool_once_flag_, &ThreadPool::thread_pool_init, numThreads);
    return *thread_pool_instance_;
}
void ThreadPool::thread_pool_init(int numThreads) {
    thread_pool_instance_.reset(new ThreadPool(numThreads));
}
ThreadPool& get_thread_pool_instance(int numThreads) {
    return ThreadPool::getInstance(numThreads);
}
