#ifndef ABSTRACTCONNECTIONSERVER_H
#define ABSTRACTCONNECTIONSERVER_H

#include <cstdint>

class AbstractConnection;
class AbstractNewConnectionHandler;

/*!
 * \brief Базовый класс сервера.
 */
class AbstractConnectionServer
{
public:
    virtual ~AbstractConnectionServer() = default;
    /*!
     * \brief Идентификатор сервера.
     */
    virtual uint64_t listenedId() const = 0;
    /*!
     * \brief Назначить серверу идентификатор и разрешить прием подключений.
     * \param clientId - идентификатор сервера
     * \return false в случае ошибки
     */
    virtual bool listen(uint64_t clientId) = 0;
    /*!
     * \brief Закрыть все соединения и прекратить прием подключений.
     */
    virtual void disconnect() = 0;
    /*!
     * \brief Установить обработчик события приема нового подключения.
     * \param handler - владеющий указатель на обработчик
     */
    virtual void setNewConnectionHandler(AbstractNewConnectionHandler* handler) = 0;
    /*!
     * \brief Невладеющий указатель на объект соединения с клиентом.
     * \param clientId - идентификатор клиента
     * \return nullptr, если соединение с клиентом отсутствует.
     */
    virtual AbstractConnection* connection(uint64_t clientId) const = 0;
};

#endif // ABSTRACTCONNECTIONSERVER_H
