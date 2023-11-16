 - protoc test.proto --cpp_out=./
 - 编译处理
 - g++ main.cc test.pb.cc -lprotobuf   //g++ *.cc -lprotobuf 
 - ./a.out


 - 进入bin目录
 - ./provider -i test.conf
