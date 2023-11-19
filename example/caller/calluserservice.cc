#include<iostream>
#include "mprpcapplication.h" //cmake已经告诉如何查找头文件
#include "../user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char** argv)
{   
    //整个程序启动以后, 想使用mprpc框架来使用rpc服务调用, 一定需要先调用框架的的初始化函数(初始化一次)
    MprpcApplication::init(argc, argv);

    //演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel()); 
    //rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    //rpc方法的响应
    fixbug::LoginResponse response;
    //发起rpc方法的调用 同步的rpc调用过程 MprpcChannel::callMethod()方法调用
    stub.Login(nullptr, &request, &response, nullptr);//RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    
    //一次调用完成, 读调用的结果
    if(response.result().errcode() == 0)
    {
        std::cout<<"rpc login response success: "<<response.success()<<std::endl;
    }
    else
    {
        std::cout<<"rpc login error: "<<response.result().errmsg()<<std::endl;
    }
    
    return 0;
}