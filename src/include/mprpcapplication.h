#pragma once

#include "mprpcconfig.h"

// mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication &GetInstance();
    static MprpcConfig &GetConfig();

private:
    static MprpcConfig m_config;

    MprpcApplication(){};
    MprpcApplication(const MprpcApplication &) = delete;             // 禁用拷贝构造函数
    MprpcApplication &operator=(const MprpcApplication &) = delete;  // 禁用赋值运算符重载
    MprpcApplication &operator=(const MprpcApplication &&) = delete; // 禁用移动赋值运算符
};
