#include "XThread.h"  
#include <thread>
#include <iostream>
#include <event2/event.h>
#include "XTask.h"
#ifndef _WIN32
#include <unistd.h>
#endif
using namespace std;

//激活线程的回调函数
static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
    XThread* t = (XThread*)arg;
    t->Notify(fd,which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
    char buf[2] = {0};
#ifdef _WIN32
    int re = recv(fd,buf,1,0);
#else
    int re = read(fd,buf,1);
#endif
    if(re<=0)
    {
        return;
    }
    XTask* task = NULL;
    tasks_mutex.lock();
    if(tasks.empty())
    {
        tasks_mutex.unlock();
        return;
    }
    task = tasks.front();
    tasks.pop_front();
    tasks_mutex.unlock();
    task->Init();
}

void XThread::AddTask(XTask* t)
{
    if(!t)return;
    t->base = this->base;
    tasks_mutex.lock();
    tasks.push_back(t);
    tasks_mutex.unlock();
}

void XThread::Activate()
{
#ifdef _WIN32
    int re = send(this->notify_send_fd,"c",1,0);
#else
    int re = write(this->notify_send_fd,"c",1);
#endif
    if (re <= 0)
    {
        return;
    }
}

void XThread::Start()
{
    Setup();
    thread th(&XThread::Main,this);
    th.detach();
}

void XThread::Main()
{
    event_base_dispatch(base);
    event_base_free(base);
}

bool XThread::Setup()
{
#ifdef _WIN32
    //创建一个socketpair，可以互相通信，fds[0]读，fds[1]写
    evutil_socket_t fds[2];
    if(evutil_socketpair(AF_INET,SOCK_STREAM,0,0,fds)<0)
    {
        return false
    }
    evutil_make_socket_nonblocking(fds[0]);
    evutil_make_socket_nonblocking(fds[1]);
#else
    //创建管道，只能用read write
    int fds[2];
    if(pipe(fds))
    {
        return false;
    }
#endif
    //读取绑定到event事件中，写入要保存
    notify_send_fd = fds[1];

    event_config* ev_conf = event_config_new();
    event_config_set_flag(ev_conf,EVENT_BASE_FLAG_NOLOCK);
    this->base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);
    if(!base)
    {
        return false;
    }
    
    event* ev = event_new(base,fds[0],EV_READ|EV_PERSIST,NotifyCB,this);
    event_add(ev,0);

    return true;
}

XThread::XThread()
{

}

XThread::~XThread()
{
    
}
