#include "taskqueue.h"

TaskQueue::~TaskQueue()
{
    while (m_tasks.size() > 0)
    {
        auto* task = m_tasks.front();
        delete task;
        m_tasks.pop();
    }
}

void TaskQueue::scheduleTask(AbstractTask* task)
{
    m_tasks.push(task);
}

bool TaskQueue::processTask()
{
    if (m_tasks.size() == 0)
        return false;
    auto* task = m_tasks.front();
    m_tasks.pop();
    (*task)();
    delete task;
    return true;
}
