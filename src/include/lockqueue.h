#pragma once
#include <queue>
#include <mutex>  //pthread_mutex_t  互斥锁
#include <condition_variable> //pthread_condition_t 条件变量

//异步写日志的日志队列
template<typename T>  //模板不能写对应的cc文件
class LockQueue
{
public:
    //多个worker线程都会写日志queue
    void Push(const T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one(); //通知等待的那一个线程
    }
    // 一个线程读queue,写日志文件, 进行磁盘IO
    T Pop()    
    {   
        //条件变量和unique_lock进行搭配
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty())
        {
            //日志队列为空, 该线程进入wait状态
            m_condvariable.wait(lock); 
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }//lock自动释放
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};