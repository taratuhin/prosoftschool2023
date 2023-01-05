#ifndef ABSTRACTCLIENTCONNECTION_H
#define ABSTRACTCLIENTCONNECTION_H

#include "abstractconnection.h"

/*!
 * \brief Базовый класс клиента.
 */
class AbstractClientConnection : public AbstractConnection
{
public:
    /*!
     * \brief Идентификатор клиента.
     */
    virtual uint64_t bindedId() const = 0;
    /*!
     * \brief Назначить клиенту идентификатор.
     * \param clientId - новый идентификатор клиента
     * \return false в случае ошибки
     */
    virtual bool bind(uint64_t clientId) = 0;
    /*!
     * \brief Выполнить соединение клиента с сервером.
     * \param serverId - идентификатор сервера
     * \return false в случае ошибки.
     */
    virtual bool connectToHost(uint64_t serverId) = 0;
    /*!
     * \brief Установить обработчик события установки соединения с сервером.
     * \param handler - владеющий указатель на обработчик
     */
    virtual void setConnectedHandler(AbstractAction* handler) = 0;
};

#endif // ABSTRACTCLIENTCONNECTION_H
