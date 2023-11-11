#include <iostream>
#include <string>
#include "user.pb.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists    
*/
class UserService:public fixbug::UserServiceRpc //使用在rpc服务发布端(rpc服务提供者)
{
    public:
    bool login(std::string name, std::string pw)
    {
        std::cout<<"doing local service:Login"<<std::endl;
        std::cout<<"name:"<<name<<" password:"<<pw<<std::endl;
    }
    //重写基类UserServiceRpc的虚函数，都是框架直接调用的
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
    
    }
};

int main()
{
    UserService us;
    us.login("xxx", "xxx");
    return 0;
}