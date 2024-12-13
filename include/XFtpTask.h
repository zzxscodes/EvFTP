#pragma once
#include "XTask.h"
#include <string>
class XFtpTask : public XTask
{
public:
    //解析协议
    virtual void Parse(std::string type,std::string msg){}
    
    //回复cmd消息
    void ResCMD(std::string msg);

    //回复数据
    void Send(std::string data);
    void Send(const char* data,int datasize);
    
    //连接数据通道
    void ConnectPORT();

    void Close();
   
    virtual void Read(struct bufferevent* bev){}
    virtual void Write(struct bufferevent* bev){}
    virtual void Event(struct bufferevent* bev,short what){}
    void SetCallBack(struct bufferevent* bev);
    bool Init(){return true;}

    std::string curDir = "/";
    std::string rootDir = ".";
    std::string ip = "";
    int port = 0;
    XFtpTask *cmdTask = 0;
    
protected:
    static void ReadCB(bufferevent* bev,void* arg);
    static void WriteCB(bufferevent* bev,void* arg);
    static void Event_CB(struct bufferevent *bev, short what, void *arg);
    //命令bev
    struct bufferevent* bev = 0;
    FILE* fp = 0;
};