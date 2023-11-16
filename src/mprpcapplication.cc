#include "include/mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config; // 初始化静态成员变量

void ShowArgsHelp()
{
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

void MprpcApplication::init(int argc, char** argv)
{
    if(argc <2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c=0;
    std::string configfile ;
    while((c=getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            configfile = optarg;
            break;
        case '?':
            std::cout<<"invalid args"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            std::cout<<"need <configfile>"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件 rpcserver_ip = xxx, rpcserver_port=xxx, zookeeper_ip=xxx, zookeeper_port=xxx,
    m_config.LoadConfigFile(configfile.c_str());

    // std::cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<std::endl;
    // std::cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<std::endl;
    // std::cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<std::endl;
    // std::cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<std::endl;

}
MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}