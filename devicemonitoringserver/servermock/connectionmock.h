#ifndef CONNECTIONMOCK_H
#define CONNECTIONMOCK_H

#include "../common.h"
#include <handlers/abstractaction.h>
#include <server/abstractconnection.h>

class ConnectionChannel;
class ConnectionServerMock;

class ConnectionMock final : public AbstractConnection
{
    NON_COPYABLE(ConnectionMock)

    class DisconnectedHandler : public AbstractAction
    {
    public:
        DisconnectedHandler(ConnectionServerMock* server, ConnectionMock* conn) :
            m_server(server), m_conn(conn) {}
        ~DisconnectedHandler() { delete m_customDisconnectedHandler; }

        void setCustomDisconnectedHandler(AbstractAction* handler);
        void operator()();

    private:
        ConnectionServerMock* m_server = nullptr;
        ConnectionMock* m_conn = nullptr;
        AbstractAction* m_customDisconnectedHandler = nullptr;
    };

public:
    ConnectionMock(ConnectionServerMock* server);
    ~ConnectionMock() final;

    /*!
     * \brief Установить канал передачи сообщений.
     * \param channel - владеющий указатель
     */
    void setChannel(ConnectionChannel* channel);

    // AbstractConnection interface
    uint64_t peerId() const final;
    bool connected() const final;
    void disconnect() final;
    void sendMessage(const std::string& message) final;
    void setMessageHandler(AbstractMessageHandler* handler) final;
    void setDisconnectedHandler(AbstractAction* handler) final;

private:
    DisconnectedHandler m_disconnectedHandler;
    ConnectionChannel* m_channel = nullptr;
    AbstractMessageHandler* m_messageHandler = nullptr;
};

#endif // CONNECTIONMOCK_H
