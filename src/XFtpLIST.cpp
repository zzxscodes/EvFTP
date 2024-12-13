#include "XFtpLIST.h"
#include <string>
#include <event2/event.h>
#include <event2/bufferevent.h>
#ifdef _WIN32
#include <io.h>
#endif
using namespace std;

void XFtpLIST::Write(struct bufferevent* bev)
{
    ResCMD("226 Transfer complete\r\n");
    Close();
}

void XFtpLIST::Event(struct bufferevent* bev,short what)
{
    if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
    {
        Close();
    }
    else if(what&BEV_EVENT_CONNECTED)
    {

    }
}

void XFtpLIST::Parse(std::string type,std::string msg)
{
    std::string resmsg = "";
    if(type == "PWD")
    {
        resmsg = "257 \"";
        resmsg+=cmdTask->curDir;
        resmsg+="\" is current dir.\r\n";

        ResCMD(resmsg);
    }
    else if(type == "LIST")
    {
        ConnectPORT();

        ResCMD("150 Here comes the directory listing.\r\n");

        string listdata = GetListData(cmdTask->rootDir + cmdTask->curDir);
        //string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";
        Send(listdata);
        
    }
    else if(type == "CWD")
    {
        int pos = msg.rfind(" ")+1;
        string path = msg.substr(pos,msg.size()-pos-2);
        if(path[0]=='/')
        {
            cmdTask->curDir = path;
        }
        else 
        {
            if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/')
                cmdTask->curDir += "/";
            cmdTask->curDir += path + "/";
        }

        ResCMD("250 Directory success changed.\r\n");
    }
    else if(type == "CDUP")
    {
        string path = cmdTask->curDir;
        if(path[path.size()-1]=='/')
        {
            path = path.substr(0,path.size()-1);
        }
        int pos = path.rfind("/");
        path = path.substr(0,pos);
        cmdTask->curDir = path;
        ResCMD("250 Directory success changed.\r\n");
    }
}

string XFtpLIST::GetListData(string path)
{
    //-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
    string data = "";
#ifdef _WIN32
    //存储文件信息
    _finddata_t file;
    //目录上下文
    path += "/*.*";
    intptr_t dir = _findfirst(path.c_str(),&file);
    if(dir<0)
        return data;
    do
    {
        string tmp = "";
        if(file.attrib & A_SUBDIR)
        {
            if(strcmp(file.name,".")==0||strcmp(file.name,"..")==0)
            {
                continue;
            }
            tmp = "drwxrwxrwx 1 root group ";
        }else
        {
            tmp = "-rwxrwxrwx 1 root group ";
        }
        char buf[1024];
        sprintf(buf,"%u",file.size);
        tmp += buf;
        strftime(buf,sizeof(buf)-1,"%b %d %H:%M ",localtime(&file.time_write));
        tmp += buf;
        tmp += file.name;
        tmp += "\r\n";
        data += tmp;
        
    } while(_findnext(dir,&file)==0)
#else
    string cmd = "ls -l";
    cmd += path;
    FILE* f = popen(cmd.c_str(),"r");
    if(!f)
        return data;
    char buffer[1024]={0};
    for(;;)
    {
        int len = fread(buffer,1,sizeof(buffer)-1,f);
        if(len<=0)break;
        buffer[len]='\0';
        data += buffer;
    }
    pclose(f);
#endif

    return data;
}