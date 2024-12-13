#include <event2/event.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include <iostream>
#include <string.h>c
#ifndef _WIN32
#include <signal.h>
#endif
#include "XThreadPool.h"
#include "XFtpServerCMD.h"
using namespace std;
#define SPORT 5001

void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr * a, int socklen, void *arg)
{
    cout<<"listen_cb"<<endl;
    XTask *task = new XFtpServerCMD();
    task->sock = s;
    XThreadPool::Get()->Dispatch(task);
}

int main()
{
#ifdef _WIN32
    WSADATA wsa
    WSAStartup(MAKEWORD(2,2),&wsa);
#else
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif

    XThreadPool::Get()->Init(10);

#ifdef _WIN32
    event_config* conf= event_config_new();
    event_config_set_flag(conf,EVENT_BASE_FLAG_STARTUP_IOCP);
    evthread_use_windows_threads();
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    event_config_set_num_cpus_hint(conf,si.dwNumberOfProcessors);
    event_base* base = event_base_new_with_config(conf);
    if(!base)
    {
        cout << "event_base_new_with_config failed!"<<endl;
        event_base *base = event_base_new();
        if (!base)
        {
            cout << "event_base_new failed!" << endl;
            return -1;
        }
    }
    event_config_free(conf);
#else
    event_base* base = event_base_new();
    if(!base)
    {
        cout << "event_base_new failed!"<<endl;
        return -1;
    }
#endif

    //监听端口
    sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);
    evconnlistener *ev = evconnlistener_new_bind(base,
        listen_cb,
        base,
        LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
        10, 
        (sockaddr*)&sin,
        sizeof(sin)               
    );

    if (base)
        event_base_dispatch(base);
    if (ev)
        evconnlistener_free(ev);
    if (base)
        event_base_free(base);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}