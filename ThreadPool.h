#ifndef ThreadPool_H
#define ThreadPool_H
//Comments in the document are Chinese
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <future>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <condition_variable>

class ThreadPool {
public:
    static ThreadPool& getInstance(int numThreads);
    ~ThreadPool();
    void get_id();

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
private:
    ThreadPool(int numThreads);
    ThreadPool(const ThreadPool&) = delete;    //ֻ���ڵ�һ��������ɾ������
    ThreadPool& operator=(const ThreadPool&) = delete;
    static void init(int numThreads);
    bool stop_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
    static std::once_flag once_flag_;
    std::queue<std::function<void()>> tasks_;
    static std::unique_ptr<ThreadPool> instance_;   //std::unique_ptr���ڴ���Դ����Ȩ��ת�Ƶ���һ unique_ptr������ԭʼ unique_ptr ����ӵ�д���Դ
};
ThreadPool& get_instance(int numThreads);

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (stop_) { throw std::runtime_error("enqueue on stopped ThreadPool"); }   //ֹͣ�غ������Ŷ�
        tasks_.emplace([task]() {(*task)(); });
    }
    cv_.notify_one();
    return res;
}

#endif // !ThreadPool_H
