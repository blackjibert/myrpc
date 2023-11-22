#include "include/zookeeperutil.h"
#include "include/mprpcapplication.h"
#include <string>

//全局的watcher观察器，zkserver给zkclient的通知
void global_watcher(zhandle_t* zh, int type, 
                        int state, const char* path, void* watcherCtx)                 
{
    if(type == ZOO_SESSION_EVENT)//回调的消息类型，是和会话相关的消息类型
    {
        if(state ==  ZOO_CONNECTED_STATE)   //zkclient和zkserver连接成功
        {
            sem_t* sem = (sem_t*)zoo_get_context(zh); //
            sem_post(sem); //+1 
        }
    }
}

ZKclient::ZKclient():m_zhandle(nullptr)
{
}

ZKclient::~ZKclient()
{
    if(m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);//关闭句柄，释放资源 MySQL_Conn
    }
}

//zkclient启动连接zkserver，是一个异步调用，要绑定一个全局的回调函数global_watcher
void ZKclient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr =  host +":"+ port ;
    std::cout<<"connstr:"<<connstr<<std::endl;
    /* 
    zookeeper_mt:多线程版本
    zookeeper的API客户端程序提供了三个线程:
    API调用线程
    网络IO线程==>调用pthread_create，创建线程，使用poll
    watcher回调线程 pthread_create
    */
    //创建句柄m_zhandle
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(nullptr == m_zhandle)
    {   
        std::cout<<" zookeeper_init error!"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);  //给句柄添加信息
    sem_wait(&sem);
    std::cout<<" zookeeper_init success!"<<std::endl;
}
//在zkserver上根据指定的path创建znode节点，state=0为永久性节点
void ZKclient::Create(const char* path, const char* data, int datalen, int state)
{
    char path_buff[128];
    int bufferlen = sizeof(path_buff);
    int flag;
    //先判断path表示的znode节点是否存在，如果存在，不再重复创建
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if(ZNONODE ==flag)  //path表示的znode节点不存在
    {   
        //创建指定path的znode节点
        flag = zoo_create(m_zhandle, path, data, datalen,
                            &ZOO_OPEN_ACL_UNSAFE, state, path_buff, bufferlen);
        if(flag == ZOK)
        {
            std::cout<<" znode create success...path: "<<path<<std::endl;
        }
        else
        {
            std::cout<<"flag: "<<flag<<std::endl;
            std::cout<<" znode create error...path: "<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
};
//根据参数指定的znode节点路径，获取znode节点的值
std::string ZKclient::GetData(const char* path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if(flag != ZOK)
    {
        std::cout<<" GET znode error ...path: "<<path<<std::endl;
        return "";
    }
    else
    {
        return buffer;
    }

};

//测试用例
// int main() {
//     // 创建一个 MprpcApplication 实例，通常包含配置信息等
//     MprpcApplication& app = MprpcApplication::GetInstance();

//     // 创建并启动 ZooKeeper 客户端
//     ZKclient zk;
//     zk.Start();

//     // 测试创建 znode 节点
//     const char* testPath = "/test_node";
//     const char* testData = "Hello, ZooKeeper!";
//     zk.Create(testPath, testData, sizeof(testData), 0);

//     // 测试获取 znode 节点的数据
//     std::string data = zk.GetData(testPath);
//     if (!data.empty()) {
//         std::cout << "Data retrieved from znode " << testPath << ": " << data << std::endl;
//     } else {
//         std::cout << "Failed to retrieve data from znode " << testPath << std::endl;
//     }

//     return 0;
// }