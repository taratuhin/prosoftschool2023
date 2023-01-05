#ifndef CLIENTCONNECTIONMOCK_H
#define CLIENTCONNECTIONMOCK_H

#include "../common.h"
#include <handlers/abstractaction.h>
#include <server/abstractclientconnection.h>

class TaskQueue;
class ConnectionChannel;

/*!
 * \brief Макет клиента для подключения к серверу.
 */
class ClientConnectionMock final : public AbstractClientConnection
{
    NON_COPYABLE(ClientConnectionMock)

    class DisconnectedHandler : public AbstractAction
    {
    public:
        DisconnectedHandler(ClientConnectionMock* client) :
            m_client(client) {}
        ~DisconnectedHandler() { delete m_customDisconnectedHandler; }

        void setCustomDisconnectedHandler(AbstractAction* handler);
        void operator()();

    private:
        ClientConnectionMock* m_client = nullptr;
        AbstractAction* m_customDisconnectedHandler = nullptr;
    };

public:
    ClientConnectionMock(TaskQueue& taskQueue);
    ~ClientConnectionMock();

    // AbstractClientConnection interface
    uint64_t bindedId() const final { return m_clientId; }
    bool bind(uint64_t clientId) final;
    bool connectToHost(uint64_t serverId) final;
    void setConnectedHandler(AbstractAction* handler) final;
    // AbstractConnection interface
    uint64_t peerId() const final;
    bool connected() const final;
    void disconnect() final;
    void sendMessage(const std::string& message) final;
    void setMessageHandler(AbstractMessageHandler* handler) final;
    void setDisconnectedHandler(AbstractAction* handler) final;

    /*!
     * \brief Найти клиент по его идентификатору.
     * \param id - идентификатор клиента
     * \return nullptr, если клиент не найден
     */
    static ClientConnectionMock* findClient(uint64_t id);
    /*!
     * \brief Установить канал передачи сообщений.
     * \param channel - владеющий указатель на канал передачи сообщений
     */
    void setChannel(ConnectionChannel* channel);
    /*!
     * \brief Обработчик разрыва соединения.
     */
    void onDisconnected();

private:
    TaskQueue& m_taskQueue;
    uint64_t m_clientId = 0;
    ConnectionChannel* m_channel = nullptr;
    AbstractMessageHandler* m_messageHandler = nullptr;
    AbstractAction* m_connectedHandler = nullptr;
    DisconnectedHandler m_disconnectedHandler;
};

#endif // CLIENTCONNECTIONMOCK_H
