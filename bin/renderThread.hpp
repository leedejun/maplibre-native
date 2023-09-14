#pragma once
#include <functional>
#include <mbgl/util/run_loop.hpp>
#include "ut_thread_pool.h"


using namespace mbgl;


class renderThread
{

public:
    renderThread(/* args */);
    ~renderThread();
    static renderThread* instance();

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

private:
    UTThreadPool* m_threadPool = nullptr;

    static renderThread* m_instance;

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
auto renderThread::Enqueue(F &&f,Args &&...args)->std::future<
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
    typename std::invoke_result<F&&, Args&&...>::type
#else
    typename std::result_of<F&& (Args&&...)>::type
#endif
>
{
    return m_threadPool->Enqueue(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
}

