#include "ThreadPool.h"

std::once_flag ThreadPool::thread_pool_once_flag_;
std::unique_ptr<ThreadPool> ThreadPool::thread_pool_instance_;

ThreadPool::ThreadPool(int numThreads) : stop_(false) {
    for (int i = 0; i < numThreads; ++i) {
        this->threads_.emplace_back([this] {    //��һ��lambda������Ϊ threads_ �����룬���������߳�
            while (true) {
                std::unique_lock<std::mutex> lock(this->mtx_);
                this->cv_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });  //���̱߳�����ʱ��wait()�Զ�����std::mutex��unlock()�ͷ��������������̻߳��֪ͨ(notify_one��notify_all)��wait()�Զ�����std::mutex��lock()��
                if (this->stop_ && this->tasks_.empty()) { return; }
                std::function<void()> task(std::move(this->tasks_.front()));
                //this->get_info();   //����ʱ��Ϣ
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
