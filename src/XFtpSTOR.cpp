#include "XFtpSTOR.h"
#include <string>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

void XFtpSTOR::Read(struct bufferevent* bev)
{
    if(!fp)return;
    for(;;)
    {
        int len = bufferevent_read(bev,buf,sizeof(buf));
        if(len<=0)
            return;
        int size = fwrite(buf,1,len,fp);
    }
}

void XFtpSTOR::Event(struct bufferevent* bev,short what)
{
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
    {
        Close();
        ResCMD("226 Transfer complete.\r\n");
    }
    else if(what&BEV_EVENT_CONNECTED)
    {
        return;
    }
}

void XFtpSTOR::Parse(std::string type,std::string msg)
{
    int pos = msg.rfind(" ")+1;
    string filename = msg.substr(pos,msg.size()-pos-2);
    string path = cmdTask->rootDir;
    path += cmdTask->curDir;
    path += filename;
    fp = fopen(path.c_str(),"wb");
    if(fp)
    {
        ConnectPORT();

        ResCMD("125 File OK\r\n");

        //触发读取事件
        bufferevent_trigger(bev,EV_READ,0);
    }
    else
    {
        ResCMD("450 file open failed!\r\n");
    }
}
