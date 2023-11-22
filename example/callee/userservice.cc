#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcapplication.h" //cmake已经告诉如何查找头文件
#include "rpcprovider.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
class UserService : public fixbug::UserServiceRpc // 使用在rpc服务发布端(rpc服务提供者)
{
public:
    bool Login(std::string name, std::string pw)
    {
        std::cout << "doing local service:Login" << std::endl;
        std::cout << "name:" << name << " password:" << pw << std::endl;
        return true;
    }
    bool Register(uint32_t id, std::string name, std::string pw)
    {
        std::cout << "doing local service:register" << std::endl;
        std::cout <<"id: " << id<< " name:" << name << " password: " << pw << std::endl;
        return true;
    }

    /*
        重写基类UserServiceRpc的虚函数，都是框架直接调用的
        1.caller ===> Login(LoginRequest) =>muduo => callee
        2.callee ===> Login(LoginRequest)=>交到下面重写的这个Login方法
    */
    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数LoginRequest，业务获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        //可以开始做有效性检查

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入 包括错误码、错误消息、返回值
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);
        // 调用回调操作,执行响应消息的序列化和网络发送（都是由框架完成）
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //1
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        //2
        bool ret = Register(id, name, pwd);
        //3
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);
        //4
        done->Run();
    }
    
};

int main(int argc, char** argv)
{
    // UserService us;
    // us.Login("xxx", "xxx");
    // return 0;
    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::init(argc, argv);

    // rpc是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider; //数据的序列化和反序列化以及网络数据的收发
    provider.NotifyService(new UserService());   //注册服务

    // 启动一个rpc服务发布节点，Run以后，进入阻塞状态，等待远程的rpc调用请求。
    provider.Run();
    return 0;
}