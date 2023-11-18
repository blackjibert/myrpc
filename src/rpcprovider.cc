#include "include/rpcprovider.h"
#include "include/mprpcapplication.h"
#include "rpcheader.pb.h"
/*
service_name() =>service描述 =>service* 记录服务对象，包含method_name=>method方法对象。

json 和protobuf 的对比，
- protobuf 是基于二进制存储的，json是二进制存储的。
- protobuf是紧密存储的，不携带其他信息，json是用key-value。
protobuf占用的带宽更少。
- protobuf还提供了rpc 服务方法的使用
*/
//这里是框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service* service)
{      
    ServiceInfo service_info;

    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();

    //获取服务的名字
    std::string service_name = pserviceDesc->name();

    //获取服务对象service的方法的数量
    int methodCnt = pserviceDesc->method_count();

    std::cout<<"service_name:"<<service_name<<std::endl;

    for(int i=0;i<methodCnt;i++)
    {   
        //获取服务对象指定下标的服务方法的描述(抽象描述)，举例：比如调用UserService服务的Login方法
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc}); 

        std::cout<<"method_name:"<<method_name<<std::endl;
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
};

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    //组合TcpServer
    // std::unique_ptr<muduo::net::TcpServer>m_tcpserverPtr;
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip, port);
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address,"RpcProvider");
    //绑定连接回调和消息读写回调方法,分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1,
    std::placeholders::_2, std::placeholders::_3));

    //设置muduo库线程数量
    server.setThreadNum(4); //1 io,3 worker

    std::cout<<"RpcProvider start service at ip: "<<ip <<" "<<" port:"<<port<<std::endl;
    //启动网络服务
    server.start();
    m_eventLoop.loop();
};

/*下面两个方法都是基于moduo库进行回调*/
//新的socket连接回调, rpc请求是短连接,响应完请求之后, 就会关闭连接
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        //rpc client的连接断开了
        conn->shutdown();
    }


}

/*
在框架内部，RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型
service_name, method_name, args; 定义proto的message类型，进行数据头的序列化和反序列化：service_name, method_name, args_size

header_size(4个字节) + header_str + args_str
*/
//已经建立连接用户的读写事件回调，如果远程有一个rpc服务的调用请求，那么onMessage方法就会响应
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn, 
                            muduo::net::Buffer* buffer, muduo::Timestamp timestamp)
{   
    // 网络上接收的远程rpc调用请求的字符流,包含有方法名:Login  参数: args
    std::string revc_buf = buffer->retrieveAllAsString();

    //从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    revc_buf.copy((char*)&header_size, 4, 0); //从0开始，拷贝4个字节的内容到header_size
    
    //根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = revc_buf.substr(4, header_size);//从0开始，读取header_size的内容，包括service_name, method_name, args_size
    mprpc::RpcHeader rpcHeader;
    std::string service_name, method_name;
    uint32_t args_size;
    if(rpcHeader.ParseFromString(rpc_header_str)) //数据存到rpcHeader中
    {
        //数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        //数据头反序列化失败
        std::cout<<"rpc_header_str: "<<rpc_header_str<<"parse error!"<<std::endl;
        return ;
    }

    //获取rpc方法"参数"的字符流数据
    std::string args_str = revc_buf.substr(4 + header_size, args_size);
    //打印调试信息
    std::cout<<"----------------------------"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;

    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"----------------------------"<<std::endl;

    //获取service对象和method对象
    auto it =m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        std::cout<<"service_name:"<<service_name<<"is not exist!"<<std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end())
    {
        std::cout<<service_name<<":"<<method_name<<"is not exist!"<<std::endl;
        return;
    }

    google::protobuf::Service* service = it->second.m_service; //获取service对象 UserService
    const google::protobuf::MethodDescriptor* method = mit->second; //获取method对象 Login

    //生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(request->ParseFromString(args_str))
    {
        std::cout<<"request parse error! content:"<<args_str <<std::endl;
        return;
    }
    google::protobuf::Message* response  = service->GetResponsePrototype(method).New();
    
    //给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::NewCallback()

    //在框架上根据远端rpc请求, 调用当前rpc节点上发布的方法
    //new UserService().Login(controller, request, response, done)
    service->CallMethod(method, nullptr, request, response, )
}  

 //Closure的回调操作, 用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* message)
{

}