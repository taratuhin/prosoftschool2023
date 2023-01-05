#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "../common.h"

#include <queue>

/*!
 * \brief Базовый класс функтора задачи на выполнение.
 */
class AbstractTask
{
public:
    virtual ~AbstractTask() = default;
    /*!
     * \brief Перегрузка оператора вызова.
     */
    virtual void operator()() = 0;
};

/*!
 * \brief Очередь задач.
 */
class TaskQueue
{
    NON_COPYABLE(TaskQueue)
public:
    TaskQueue() = default;
    ~TaskQueue();
    /*!
     * \brief Запланировать задачу на выполнение.
     * \param task - владеющий указатель на задачу.
     */
    void scheduleTask(AbstractTask* task);
    /*!
     * \brief Обработать одну задачу из очереди.
     * \return false, если задач на выполнение нет.
     */
    bool processTask();

private:
    std::queue<AbstractTask*> m_tasks;
};

#endif // TASKQUEUE_H
