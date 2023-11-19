#include <iostream>
#include <string>
#include "../friend.pb.h"
#include "mprpcapplication.h" //cmake已经告诉如何查找头文件
#include "rpcprovider.h"
#include <vector>

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid)
    {
        std::cout<<"do GetFriendList service! userid:"<<userid<<std::endl;
        std::vector<std::string> vec;
        for(int i=0;i<5;i++)
        {
            vec.push_back("xiao li "+i);
            // sprintf(name, "%s%s", firstName, lastName);
        }
        return vec;
    }

    //重写基类方法
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendsList = GetFriendsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string& name: friendsList)
        {
            std::string* p = response->add_friends();
            *p = name;
        }
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
    provider.NotifyService(new FriendService()); 

    // 启动一个rpc服务发布节点，Run以后，进入阻塞状态，等待远程的rpc调用请求。
    provider.Run();
    return 0;
}