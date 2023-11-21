# aux_source_directory(. SRC_LIST)
set(SRC_LIST 
        mprpcapplication.cc 
        mprpcconfig.cc 
        rpcheader.pb.cc 
        rpcprovider.cc 
        mprpcchannel.cc
        mprpccontroller.cc 
        logger.cc
        zookeeperutil.cc)

# add_library(mprpc SHARED ${SRC_LIST}) # 动态库
add_library(mprpc ${SRC_LIST})          # 静态库
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)