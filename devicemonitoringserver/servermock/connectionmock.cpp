#include "connectionmock.h"
#include "connectionchannel.h"
#include "connectionservermock.h"
#include <handlers/abstractmessagehandler.h>

ConnectionMock::ConnectionMock(ConnectionServerMock* server) :
    m_disconnectedHandler(server, this)
{
}

ConnectionMock::~ConnectionMock()
{
    delete m_channel;
    delete m_messageHandler;
}

void ConnectionMock::setChannel(ConnectionChannel* channel)
{
    if (m_channel)
    {
        delete channel;
        return;
    }
    m_channel = channel;
    m_channel->setMessageHandler(m_messageHandler);
    m_channel->setDisconnectedHandler(&m_disconnectedHandler);
}

uint64_t ConnectionMock::peerId() const
{
    return m_channel ? m_channel->peerId() : 0;
}

bool ConnectionMock::connected() const
{
    return m_channel != nullptr;
}

void ConnectionMock::disconnect()
{
    if (m_channel)
        m_channel->disconnect();
}

void ConnectionMock::sendMessage(const std::string& message)
{
    if (m_channel)
        m_channel->sendMessage(message);
}

void ConnectionMock::setMessageHandler(AbstractMessageHandler* handler)
{
    delete m_messageHandler;
    m_messageHandler = handler;
    if (m_channel)
        m_channel->setMessageHandler(m_messageHandler);
}

void ConnectionMock::setDisconnectedHandler(AbstractAction* handler)
{
    m_disconnectedHandler.setCustomDisconnectedHandler(handler);
}

void ConnectionMock::DisconnectedHandler::setCustomDisconnectedHandler(AbstractAction* handler)
{
    delete m_customDisconnectedHandler;
    m_customDisconnectedHandler = handler;
}

void ConnectionMock::DisconnectedHandler::operator()()
{
    if (m_customDisconnectedHandler)
        (*m_customDisconnectedHandler)();
    m_server->onDisconnected(m_conn->peerId());
}
