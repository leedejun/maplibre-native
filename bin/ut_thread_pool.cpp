#include "ut_thread_pool.h"
#include <cassert>

/**
 * @brief 构造函数
 * @param threads 线程池中，线程数量
 */
UTThreadPool::UTThreadPool(const uint64_t threads
    /*=std::max(2u,std::thread::hardware_concurrency())*/)
    :m_ui64PoolSize(threads)
    ,m_bStop(false)
    ,m_ui64MaxQueueSize(100000)
    ,m_pVecWorkers(new(std::nothrow) std::vector<std::thread>)
    ,m_pQueTasks(new(std::nothrow) std::queue<std::function<void()>>)
    ,m_pMutexQueue(new(std::nothrow) std::mutex)
    ,m_pConditionProducers(new(std::nothrow) std::condition_variable)
    ,m_pConditionConsumers(new(std::nothrow) std::condition_variable)
    ,m_pMutexInFlight(new(std::nothrow) std::mutex)
    ,m_pConditionInFlight(new(std::nothrow) std::condition_variable)
    ,m_pui64InFlight(new(std::nothrow) std::atomic<uint64_t>(0))
{
    assert(nullptr!=m_pVecWorkers);
    assert(nullptr!=m_pQueTasks);
    assert(nullptr!=m_pMutexQueue);
    assert(nullptr!=m_pConditionProducers);
    assert(nullptr!=m_pConditionConsumers);
    assert(nullptr!=m_pMutexInFlight);
    assert(nullptr!=m_pConditionInFlight);
    assert(nullptr!=m_pui64InFlight);
    std::unique_lock<std::mutex> lock(*m_pMutexQueue);
    for(uint64_t i=0;i<threads;++i)
    {
        StartWorker(i,lock);
    }
}

/**
 * @brief 析构函数
 */
UTThreadPool::~UTThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(*m_pMutexQueue);
        m_bStop=true;
        m_ui64PoolSize=0;
        m_pConditionConsumers->notify_all();
        m_pConditionProducers->notify_all();
        m_pConditionConsumers->wait(lock,[this]{ return m_pVecWorkers->empty(); });
        // assert(*m_pui64InFlight==0); 
    }

    delete m_pVecWorkers;
    delete m_pQueTasks;
    delete m_pMutexQueue;
    delete m_pConditionProducers;
    delete m_pConditionConsumers;
    delete m_pMutexInFlight;
    delete m_pConditionInFlight;
    delete m_pui64InFlight;
}

/**
 * @brief 等待任务全部执行完成
 */
void UTThreadPool::WaitUntilEmpty()
{
    std::unique_lock<std::mutex> lock(*m_pMutexQueue);
    m_pConditionProducers->wait(lock,[this]
    {
        return m_pQueTasks->empty();
    });
}

/**
 * @brief 等待全部正在执行的任务完成
 */
void UTThreadPool::WaitUntilNothingInFlight()
{
    std::unique_lock<std::mutex> lock(*m_pMutexInFlight);
    m_pConditionInFlight->wait(lock,[this]
    { 
        return *m_pui64InFlight==0; 
    });
}

/**
 * @brief 设置线程池任务队列最大尺寸
 * @param limit 尺寸上限
 */
void UTThreadPool::SetQueueSizeLimit(const uint64_t limit)
{
    std::unique_lock<std::mutex> lock(*m_pMutexQueue);
    if (m_bStop)
    {
        return;
    }
    const uint64_t old_limit=m_ui64MaxQueueSize;
    m_ui64MaxQueueSize=std::max(limit,uint64_t(1));
    if (old_limit<m_ui64MaxQueueSize)
    {
        m_pConditionProducers->notify_all();
    }
}

/**
 * @brief 设置当前池子可容纳线程数量
 * @param limit 数量上限
 */
void UTThreadPool::SetPoolSize(uint64_t limit)
{
    if(limit<1)
    {
        limit=1;
    }

    std::unique_lock<std::mutex> lock(*m_pMutexQueue);

    if (m_bStop)
    {
        return;
    }

    const uint64_t old_size = m_ui64PoolSize;
    assert(m_pVecWorkers->size()>=old_size);

    m_ui64PoolSize=limit;
    if (m_ui64PoolSize>old_size)
    {
        for (uint64_t i=old_size;i!=m_ui64PoolSize;++i)
        {
            StartWorker(i,lock);
        }
    }
    else if(m_ui64PoolSize<old_size)
    {
        m_pConditionConsumers->notify_all();
    }
}

bool UTThreadPool::GetWorkerID(uint64_t & id) {
    bool bResult = false;
    do {
        std::thread::id tid = std::this_thread::get_id();
        auto fItem = m_threadID.find(tid);
        if(fItem == m_threadID.end())
            break;        
        id = fItem->second;
        bResult = true;
    }while(0);
    return bResult;
}

/**
 * @brief 构造函数
 * @param pool  线程池
 */
UTThreadPool::HandleInFlightDecremnet::HandleInFlightDecremnet(UTThreadPool &pool)
    :m_refPool(pool)
{
}

/**
 * @brief 析构函数
 */
UTThreadPool::HandleInFlightDecremnet::~HandleInFlightDecremnet()
{
    uint64_t prev = std::atomic_fetch_sub_explicit(
        m_refPool.m_pui64InFlight,uint64_t(1),std::memory_order_acq_rel);
    if(1==prev)
    {
        std::unique_lock<std::mutex> guard(*(m_refPool.m_pMutexInFlight));
        m_refPool.m_pConditionInFlight->notify_all();
    }
}

/**
 * @brief 启动一条工作线程
 * @param workerNumber  工作线程号
 * @param lock          互斥锁
 */
void UTThreadPool::StartWorker(const uint64_t workerNumber,
    const std::unique_lock<std::mutex> &lock)
{
    assert(lock.owns_lock()&&lock.mutex()==m_pMutexQueue);
    assert(workerNumber<=m_pVecWorkers->size());

    auto WorkFunc=[this,workerNumber]
    {
        for(;;)
        {
            std::function<void()> task;
            bool notify=false;
            {
                std::unique_lock<std::mutex> lock(*m_pMutexQueue);
                m_pConditionConsumers->wait(lock,[this,workerNumber]
                {
                    return m_bStop||!m_pQueTasks->empty()||
                        m_ui64PoolSize<workerNumber+1; 
                });

                if ((m_bStop&&m_pQueTasks->empty())||
                    (!m_bStop&&m_ui64PoolSize<workerNumber+1))
                {
                    (*m_pVecWorkers)[workerNumber].detach();

                    while(m_pVecWorkers->size()>m_ui64PoolSize&&
                        !m_pVecWorkers->back().joinable())
                    {
                        m_pVecWorkers->pop_back();
                    }

                    if (m_pVecWorkers->empty())
                    {
                        m_pConditionConsumers->notify_all();
                    }

                    return;
                }
                else if (!m_pQueTasks->empty())
                {
                    task=std::move(m_pQueTasks->front());
                    m_pQueTasks->pop();
                    notify = m_pQueTasks->size()+1==m_ui64MaxQueueSize
                        || m_pQueTasks->empty();
                }
                else
                {
                    continue;
                }
            }

            HandleInFlightDecremnet guard(*this);

            if (notify)
            {
                std::unique_lock<std::mutex> lock(*m_pMutexQueue);
                m_pConditionProducers->notify_all();
            }

            task();
        }
    };

    if(workerNumber<m_pVecWorkers->size())
    {
        std::thread &worker=m_pVecWorkers->at(workerNumber);
        // start only if not already running
        if(!worker.joinable()) 
        {
            worker=std::thread(WorkFunc);
        }
        m_threadID[worker.get_id()] = workerNumber;
    }
    else
    {
        m_pVecWorkers->push_back(std::thread(WorkFunc));
        std::thread &worker = m_pVecWorkers->at(m_pVecWorkers->size()-1);
        m_threadID[worker.get_id()] = m_pVecWorkers->size() - 1;
    }
}