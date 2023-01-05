#ifndef CONNECTIONSERVERMOCK_H
#define CONNECTIONSERVERMOCK_H

#include "../common.h"
#include "taskqueue.h"
#include <server/abstractconnectionserver.h>

#include <unordered_map>

class AbstractConnection;
class ConnectionChannel;
class ConnectionMock;

/*!
 * \brief Макет сервера для приема подключений.
 */
class ConnectionServerMock final : public AbstractConnectionServer
{
    NON_COPYABLE(ConnectionServerMock)
public:
    ConnectionServerMock(TaskQueue& taskQueue);
    ~ConnectionServerMock() final;

    /*!
     * \brief Добавить новый канал передачи сообщений.
     * \param channel - владеющий указатель
     */
    void addChannel(ConnectionChannel* channel);
    /*!
     * \brief Обработчик закрытия соединения.
     * \param clientId - идентификатор клиента
     */
    void onDisconnected(uint64_t clientId);
    /*!
     * \brief Найти сервер по его идентификатору.
     * \param id - идентификатор сервера
     * \return nullptr, если сервер не найден
     */
    static ConnectionServerMock* findServer(uint64_t id);

    // AbstractConnectionServer interface
    uint64_t listenedId() const final { return m_serverId; }
    bool listen(uint64_t serverId) final;
    void disconnect() final;
    void setNewConnectionHandler(AbstractNewConnectionHandler* handler) final;
    AbstractConnection* connection(uint64_t clientId) const final;

private:
    void addConnection(ConnectionMock* connection);

private:
    TaskQueue& m_taskQueue;
    uint64_t m_serverId = 0;
    std::unordered_map<uint64_t, ConnectionMock*> m_connections;
    AbstractNewConnectionHandler* m_newConnectionHandler = nullptr;
};

#endif // CONNECTIONSERVERMOCK_H
