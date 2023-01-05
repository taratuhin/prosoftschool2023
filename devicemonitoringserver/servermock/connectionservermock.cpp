#include "connectionservermock.h"
#include "connectionmock.h"
#include <handlers/abstractnewconnectionhandler.h>

#include <cassert>
#include <unordered_map>

namespace
{

std::unordered_map<uint64_t, ConnectionServerMock*>& serversMap()
{
    static std::unordered_map<uint64_t, ConnectionServerMock*> map;
    return map;
}

bool serverAlreadyRegistered(uint64_t id)
{
    return serversMap().count(id) > 0;
}

void registerServer(uint64_t id, ConnectionServerMock* server)
{
    serversMap().insert({ id, server });
}

void unregisterServer(uint64_t id)
{
    serversMap().erase(id);
}

} // namespace

ConnectionServerMock* ConnectionServerMock::findServer(uint64_t id)
{
    const auto& map = serversMap();
    const auto it = map.find(id);
    return it != map.cend() ? it->second : nullptr;
}

ConnectionServerMock::ConnectionServerMock(TaskQueue& taskQueue) :
    m_taskQueue(taskQueue) {}

ConnectionServerMock::~ConnectionServerMock()
{
    delete m_newConnectionHandler;
    for (auto it = m_connections.cbegin(); it != m_connections.cend(); ++it)
        delete it->second;
    unregisterServer(m_serverId);
}

void ConnectionServerMock::addChannel(ConnectionChannel* channel)
{
    auto* conn = new ConnectionMock(this);
    conn->setChannel(channel);
    addConnection(conn);
}

void ConnectionServerMock::addConnection(ConnectionMock* connection)
{
    const uint64_t clientId = connection->peerId();
    if (m_connections.count(clientId) > 0)
    {
        delete connection;
        return;
    }
    m_connections.insert({ clientId, connection });
    if (m_newConnectionHandler)
        (*m_newConnectionHandler)(connection);
}

bool ConnectionServerMock::listen(uint64_t serverId)
{
    if (serverAlreadyRegistered(serverId) || m_serverId || !serverId)
        return false;
    m_serverId = serverId;
    registerServer(m_serverId, this);
    return true;
}

void ConnectionServerMock::disconnect()
{
    for (const auto& conn : m_connections)
        conn.second->disconnect();
    unregisterServer(m_serverId);
    m_serverId = 0;
}

void ConnectionServerMock::onDisconnected(uint64_t clientId)
{
    const auto it = m_connections.find(clientId);
    if (it != m_connections.cend())
    {
        delete it->second;
        m_connections.erase(it);
    }
}

AbstractConnection* ConnectionServerMock::connection(uint64_t clientId) const
{
    const auto it = m_connections.find(clientId);
    return it != m_connections.cend() ? it->second : nullptr;
}

void ConnectionServerMock::setNewConnectionHandler(AbstractNewConnectionHandler* handler)
{
    delete m_newConnectionHandler;
    m_newConnectionHandler = handler;
}
