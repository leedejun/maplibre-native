#ifndef UT_THREAD_POOL_H
#define UT_THREAD_POOL_H

#include <thread>
#include <algorithm>
#include <stdint.h>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <map>

/**
 * @brief 实现跨平台线程池
 */
class UTThreadPool
{
public:
    /**
     * @brief 构造函数
     * @param threads 线程池中，线程数量
     */
    explicit UTThreadPool(const uint64_t threads=
        std::max(2u,std::thread::hardware_concurrency()));
    /**
     * @brief 析构函数
     */
    ~UTThreadPool();
public:
    /**
     * @brief 提交任务给线程池
     * @tparam F     函数类型
     * @tparam Args  参数类型
     * @param  f     执行函数
     * @param  args  函数参数
     * @return 返回std::future对象，供用户获取执行结果
     */
    template<typename F,typename ...Args>
    auto Enqueue(F &&f,Args &&...args)->std::future<
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
        typename std::invoke_result<F&&, Args&&...>::type
#else
        typename std::result_of<F&& (Args&&...)>::type
#endif
    >;
    /**
     * @brief 等待任务全部执行完成
     */
    void WaitUntilEmpty();
    /**
     * @brief 等待全部正在执行的任务完成
     */
    void WaitUntilNothingInFlight();
    /**
     * @brief 设置线程池任务队列最大尺寸
     * @param limit 尺寸上限
     */
    void SetQueueSizeLimit(const uint64_t limit);
    /**
     * @brief 设置当前池子可容纳线程数量
     * @param limit 数量上限
     */
    void SetPoolSize(uint64_t limit);
    /**
     * @brief 获得调用此函数所在线程在线程池内的workerid，如果当前线程不是线程池内线程则返回false
     * @param id workerid
     */        
    bool GetWorkerID(uint64_t & id);

private:
    /**
     * @brief 内部类，用来控制待执行任务计数
     */
    class HandleInFlightDecremnet
    {
    public:
        /**
         * @brief 构造函数
         * @param pool  线程池
         */
        HandleInFlightDecremnet(UTThreadPool &pool);
        /**
         * @brief 析构函数
         */
        ~HandleInFlightDecremnet();
    private:
        UTThreadPool &m_refPool; 
    };
private:
    /**
     * @brief 启动一条工作线程
     * @param workerNumber  工作线程号
     * @param lock          互斥锁
     */
    void StartWorker(const uint64_t workerNumber,
        const std::unique_lock<std::mutex> &lock);
private:
    UTThreadPool(UTThreadPool&&)=delete;
    UTThreadPool(const UTThreadPool&)=delete;
    UTThreadPool &operator=(UTThreadPool&&)=delete;
    UTThreadPool &operator=(const UTThreadPool&)=delete;
private:
    uint64_t m_ui64PoolSize;
    bool m_bStop;
    uint64_t m_ui64MaxQueueSize;
    std::vector<std::thread> *m_pVecWorkers;
    std::queue<std::function<void()>> *m_pQueTasks;
    std::mutex *m_pMutexQueue;
    std::condition_variable *m_pConditionProducers;
    std::condition_variable *m_pConditionConsumers;
    std::mutex *m_pMutexInFlight;
    std::condition_variable *m_pConditionInFlight;
    std::atomic<uint64_t> *m_pui64InFlight;
    std::map<std::thread::id,uint64_t> m_threadID;
};

/**
 * @brief 提交任务给线程池
 * @tparam F     函数类型
 * @tparam Args  参数类型
 * @param  f     执行函数
 * @param  args  函数参数
 * @return 返回std::future对象，供用户获取执行结果
 */
template<typename F,typename ...Args>
auto UTThreadPool::Enqueue(F &&f,Args &&...args)->std::future<
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
    typename std::invoke_result<F&&, Args&&...>::type
#else
    typename std::result_of<F&& (Args&&...)>::type
#endif
>
{
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
    using ReturnType=typename std::invoke_result<F&&, Args&&...>::type;
#else
    using ReturnType=typename std::result_of<F&& (Args&&...)>::type;
#endif

    auto task=std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f),std::forward<Args>(args)...));

    std::future<ReturnType> res=task->get_future();

    std::unique_lock<std::mutex> lock(*m_pMutexQueue);
    if (m_pQueTasks->size()>=m_ui64MaxQueueSize)
    {
        // wait for the queue to empty or be stopped
        m_pConditionProducers->wait(lock,[this]
        {
            return m_pQueTasks->size()<m_ui64MaxQueueSize||m_bStop;
        });
    }

    if(m_bStop)
    {
        throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    m_pQueTasks->emplace([task](){(*task)();});
    std::atomic_fetch_add_explicit(m_pui64InFlight,
        uint64_t(1),std::memory_order_relaxed);
    m_pConditionConsumers->notify_one();

    return res;
}

#endif // !UT_THREAD_POOL_H