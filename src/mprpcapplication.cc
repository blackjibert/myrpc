#include "include/mprpcapplication.h"
void MprpcApplication::init(int argc, char** argv)
{

}
MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}