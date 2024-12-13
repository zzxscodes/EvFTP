#include "XThreadPool.h"
#include "XThread.h"
#include <thread>
#include "XTask.h"
using namespace std;

void XThreadPool::Init(int threadCount)
{
    this->threadCount = threadCount;
    this->lastThread = -1;
    for(int i = 0; i<threadCount; i++)
    {
        XThread* t = new XThread();
        t->id = i + 1;
        t->Start();
        threads.push_back(t);
    }
}

void XThreadPool::Dispatch(XTask* task)
{
    if(!task)return;
    int tid = (lastThread+1)%threadCount;//轮询
    XThread* t = threads[tid];
    t->AddTask(task);
    t->Activate();
}
