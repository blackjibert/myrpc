#pragma once

// mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication &GetInstance();

private:
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication &operator=(const MprpcApplication &) = delete;
};
