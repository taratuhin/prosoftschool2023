#ifndef ABSTRACTNEWCONNECTIONHANDLER_H
#define ABSTRACTNEWCONNECTIONHANDLER_H

class AbstractConnection;

/*!
 * \brief Базовый класс функтора-обработчика события
 * приема нового подключения к серверу.
 */
class AbstractNewConnectionHandler
{
public:
    virtual ~AbstractNewConnectionHandler() = default;
    /*!
     * \brief Перегрузка оператора вызова.
     * \param conn - невладеющий указатель на соединение
     */
    virtual void operator()(AbstractConnection* conn) = 0;
};

#endif // ABSTRACTNEWCONNECTIONHANDLER_H
