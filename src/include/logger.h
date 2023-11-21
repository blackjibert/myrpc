#pragma once
#include<string>

#include "lockqueue.h"

enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};
// Mprpc提供的日志系统
class Logger
{
public:
    //获取日志的单例
    static Logger& GetInstance();
    //设置日志级别
    void SetLogLevel(LogLevel);
    //写日志
    void Log(std::string msg);

private:
    int m_loglevel; // 记录日历级别
    LockQueue<std::string> m_lckQue;//日志缓冲队列

    Logger();
    Logger operator=(const Logger&) = delete; //禁用拷贝赋值运算符
    Logger(const Logger&) = delete; //禁用拷贝构造函数
    Logger(Logger&&) = delete;  //禁用移动构造函数
};

//定义宏 LOG_INFO("xxx %d %s ", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger& logger = Logger::GetInstance();\
        logger.SetLogLevel(INFO);\
        char c[1024]={0};\
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.Log(c);\
    }while(0); 

#define LOG_ERROR(logmsgformat, ...) \
    do \
    {  \
        Logger& logger = Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char c[1024]={0};\
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__);\
        logger.Log(c);\
    }while(0); 