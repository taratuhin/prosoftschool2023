#ifndef ABSTRACTACTION_H
#define ABSTRACTACTION_H

/*!
 * \brief Базовый класс функтора задачи на выполнение.
 */
class AbstractAction
{
public:
    virtual ~AbstractAction() = default;
    /*!
     * \brief Перегрузка оператора вызова.
     */
    virtual void operator()() = 0;
};

#endif // ABSTRACTACTION_H
