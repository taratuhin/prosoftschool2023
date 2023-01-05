#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <cstdint>
#include <string>

class AbstractAction;
class AbstractMessageHandler;

/*!
 * \brief Базовый класс клиент-серверного соединения.
 */
class AbstractConnection
{
public:
    virtual ~AbstractConnection() = default;
    /*!
     * \brief Идентификатор связанного узла (клиента/сервера).
     */
    virtual uint64_t peerId() const = 0;
    /*!
     * \brief Закрыть соединение.
     */
    virtual void disconnect() = 0;
    /*!
     * \brief Соединение активно.
     */
    virtual bool connected() const = 0;
    /*!
     * \brief Отправить сообщение.
     * \param message - сообщение
     */
    virtual void sendMessage(const std::string& message) = 0;
    /*!
     * \brief Установить обработчик события приема сообщения.
     * \param handler - владеющий указатель на обработчик
     */
    virtual void setMessageHandler(AbstractMessageHandler* handler) = 0;
    /*!
     * \brief Установить обработчик события разрыва соединения.
     * \param handler - владеющий указатель на обработчик
     */
    virtual void setDisconnectedHandler(AbstractAction* handler) = 0;
};

#endif // ABSTRACTCONNECTION_H
