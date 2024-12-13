#pragma once
#include "XFtpTask.h"

class XFtpRETR : public XFtpTask
{
public:
    //解析协议
    virtual void Parse(std::string type,std::string msg);
    virtual void Write(struct bufferevent* bev);
    virtual void Event(struct bufferevent* bev,short what);
private:
    char buf[8192] = {0};
};

