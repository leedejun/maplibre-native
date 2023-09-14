#include "renderThread.hpp"


renderThread* renderThread::m_instance = nullptr;

renderThread::renderThread(/* args */)
{
    m_threadPool = new UTThreadPool(1);
}

renderThread::~renderThread()
{
    if (m_threadPool)
    {
        delete m_threadPool;
        m_threadPool = nullptr;
    }
}

renderThread* renderThread::instance()
{
    if (!m_instance)
    {
        static renderThread tempInstance = renderThread();
        m_instance = &tempInstance;
    }
    return m_instance;
}