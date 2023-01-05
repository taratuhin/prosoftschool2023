#ifndef ABSTRACTMESSAGEHANDLER_H
#define ABSTRACTMESSAGEHANDLER_H

#include <string>

/*!
 * \brief Базовый класс функтора-обработчика события поступления нового сообщения.
 */
class AbstractMessageHandler
{
public:
    virtual ~AbstractMessageHandler() = default;
    /*!
     * \brief Перегрузка оператора вызова.
     * \param message - сообщение
     */
    virtual void operator()(const std::string& message) = 0;
};

#endif // ABSTRACTMESSAGEHANDLER_H
