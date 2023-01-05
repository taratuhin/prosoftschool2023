#include "servertests.h"
#include "test_runner.h"
#include <handlers/abstractaction.h>
#include <handlers/abstractmessagehandler.h>
#include <handlers/abstractnewconnectionhandler.h>
#include <servermock/clientconnectionmock.h>
#include <servermock/connectionchannel.h>
#include <servermock/connectionservermock.h>
#include <servermock/object.h>
#include <servermock/taskqueue.h>

namespace
{
int allocationCounter = 0;
}

void taskQueueTest()
{
    struct TestTask : public AbstractTask
    {
        TestTask(int& processedCounter) :
            m_processedCounter(processedCounter) { ++allocationCounter; }
        ~TestTask() { --allocationCounter; }

        void operator()() final { ++m_processedCounter; }

    private:
        int& m_processedCounter;
    };

    int processedCounter = 0;

    auto* task1 = new TestTask(processedCounter);
    auto* task2 = new TestTask(processedCounter);
    {
        TaskQueue queue;
        ASSERT(!queue.processTask());
        ASSERT_EQUAL(processedCounter, 0);
        ASSERT_EQUAL(allocationCounter, 2);
        queue.scheduleTask(task1);
        ASSERT_EQUAL(processedCounter, 0);
        ASSERT_EQUAL(allocationCounter, 2);
        ASSERT(queue.processTask());
        ASSERT_EQUAL(processedCounter, 1);
        ASSERT_EQUAL(allocationCounter, 1);
        ASSERT(!queue.processTask());
        ASSERT_EQUAL(processedCounter, 1);
        ASSERT_EQUAL(allocationCounter, 1);
        queue.scheduleTask(task2);
        ASSERT_EQUAL(processedCounter, 1);
        ASSERT_EQUAL(allocationCounter, 1);
    }
    ASSERT_EQUAL(processedCounter, 1);
    ASSERT_EQUAL(allocationCounter, 0);
}

void safeObjectPointerTest()
{
    SafeObjectPointer* ptr1 = nullptr;
    {
        Object object;
        ASSERT(object.m_sharedData);
        ASSERT_EQUAL(object.m_sharedData->refCounter, 1u);
        ASSERT_EQUAL(object.m_sharedData->object, &object);
        ptr1 = new SafeObjectPointer(&object);
        ASSERT_EQUAL(object.m_sharedData, ptr1->m_sharedData);
        ASSERT_EQUAL(ptr1->m_sharedData->refCounter, 2u);
        {
            SafeObjectPointer ptr2(&object);
            ASSERT_EQUAL(ptr1->m_sharedData->refCounter, 3u);
        }
        ASSERT_EQUAL(ptr1->m_sharedData->refCounter, 2u);
    }
    ASSERT_EQUAL(ptr1->m_sharedData->refCounter, 1u);
    ASSERT_EQUAL(ptr1->m_sharedData->object, static_cast<Object*>(nullptr));
    delete ptr1;
}

void connectionChannelTest()
{
    class MessageHandler : public AbstractMessageHandler
    {
    public:
        MessageHandler(std::vector<std::string>& messageList) :
            m_messageList(messageList) {}

        void operator()(const std::string& message) final
        {
            m_messageList.push_back(message);
        }

    private:
        std::vector<std::string>& m_messageList;
    };
    class DisconnectedHandler : public AbstractAction
    {
    public:
        DisconnectedHandler(bool& disconnected) :
            m_disconnected(disconnected) {}

        void operator()() final
        {
            m_disconnected = true;
        }

    private:
        bool& m_disconnected;
    };

    {
        TaskQueue taskQueue;
        const uint64_t peerId1 = 1;
        const uint64_t peerId2 = 2;

        ConnectionChannel channel1(taskQueue, peerId2);
        ConnectionChannel channel2(taskQueue, peerId1);

        std::vector<std::string> messages1;
        std::vector<std::string> messages2;
        MessageHandler messageHandler1(messages1);
        MessageHandler messageHandler2(messages2);
        channel1.setMessageHandler(&messageHandler1);
        channel2.setMessageHandler(&messageHandler2);

        bool disconnected1 = false;
        bool disconnected2 = false;
        DisconnectedHandler disconnectedHandler1(disconnected1);
        DisconnectedHandler disconnectedHandler2(disconnected2);
        channel1.setDisconnectedHandler(&disconnectedHandler1);
        channel2.setDisconnectedHandler(&disconnectedHandler2);

        ASSERT_EQUAL(channel1.peerId(), peerId2);
        ASSERT_EQUAL(channel2.peerId(), peerId1);
        ASSERT(!channel1.connected());
        ASSERT(!channel2.connected());
        channel1.connect(&channel2);
        channel2.connect(&channel1);
        ASSERT(channel1.connected());
        ASSERT(channel2.connected());
        channel1.sendMessage("1");
        channel2.sendMessage("11");
        channel1.sendMessage("2");
        channel2.sendMessage("22");
        channel1.sendMessage("3");
        channel2.sendMessage("33");
        channel1.disconnect();
        channel1.sendMessage("4");
        channel2.sendMessage("44");
        ASSERT(!channel1.connected());
        ASSERT(!channel2.connected());
        ASSERT(!disconnected1);
        ASSERT(!disconnected2);
        ASSERT_EQUAL(messages1, std::vector<std::string>());
        ASSERT_EQUAL(messages2, std::vector<std::string>());
        while (taskQueue.processTask())
            ;
        ASSERT(!channel1.connected());
        ASSERT(!channel2.connected());
        ASSERT(disconnected1);
        ASSERT(disconnected2);
        ASSERT_EQUAL(messages1, std::vector<std::string>({ "11", "22", "33" }));
        ASSERT_EQUAL(messages2, std::vector<std::string>({ "1", "2", "3" }));
    }

    {
        TaskQueue taskQueue;
        const uint64_t peerId1 = 1;
        const uint64_t peerId2 = 2;

        auto* channel1 = new ConnectionChannel(taskQueue, peerId2);
        auto* channel2 = new ConnectionChannel(taskQueue, peerId1);

        std::vector<std::string> messages1;
        std::vector<std::string> messages2;
        MessageHandler messageHandler1(messages1);
        MessageHandler messageHandler2(messages2);
        channel1->setMessageHandler(&messageHandler1);
        channel2->setMessageHandler(&messageHandler2);

        bool disconnected1 = false;
        bool disconnected2 = false;
        DisconnectedHandler disconnectedHandler1(disconnected1);
        DisconnectedHandler disconnectedHandler2(disconnected2);
        channel1->setDisconnectedHandler(&disconnectedHandler1);
        channel2->setDisconnectedHandler(&disconnectedHandler2);

        ASSERT_EQUAL(channel1->peerId(), peerId2);
        ASSERT_EQUAL(channel2->peerId(), peerId1);
        ASSERT(!channel1->connected());
        ASSERT(!channel2->connected());
        channel1->connect(channel2);
        channel2->connect(channel1);
        ASSERT(channel1->connected());
        ASSERT(channel2->connected());
        channel1->sendMessage("1");
        channel2->sendMessage("11");
        channel1->sendMessage("2");
        channel2->sendMessage("22");
        channel1->sendMessage("3");
        channel2->sendMessage("33");
        delete channel1;
        channel2->sendMessage("44");
        ASSERT(!channel2->connected());
        ASSERT(!disconnected1);
        ASSERT(!disconnected2);
        ASSERT_EQUAL(messages1, std::vector<std::string>());
        ASSERT_EQUAL(messages2, std::vector<std::string>());
        while (taskQueue.processTask())
            ;
        ASSERT(!channel2->connected());
        ASSERT(!disconnected1);
        ASSERT(disconnected2);
        ASSERT_EQUAL(messages1, std::vector<std::string>());
        ASSERT_EQUAL(messages2, std::vector<std::string>({ "1", "2", "3" }));
        delete channel2;
    }
}

void clientServerTest()
{
    TaskQueue taskQueue;

    AbstractClientConnection* client = new ClientConnectionMock(taskQueue);
    AbstractConnectionServer* server = new ConnectionServerMock(taskQueue);

    std::ostringstream ostr;
    class ClientMessageHandler : public AbstractMessageHandler
    {
    public:
        ClientMessageHandler(std::ostream& out) :
            m_out(out) { ++allocationCounter; }
        ~ClientMessageHandler() { --allocationCounter; }

    private:
        void operator()(const std::string& message) final
        {
            m_out << message;
        }

    private:
        std::ostream& m_out;
    };
    client->setMessageHandler(new ClientMessageHandler(ostr));

    class ConnectedHandler : public AbstractAction
    {
    public:
        ConnectedHandler(AbstractClientConnection* client) :
            m_client(client) { ++allocationCounter; }
        ~ConnectedHandler() { --allocationCounter; }

    private:
        void operator()() final
        {
            m_client->sendMessage("connection request");
        }

    private:
        AbstractClientConnection* m_client = nullptr;
    };
    client->setConnectedHandler(new ConnectedHandler(client));

    class DisconnectedHandler : public AbstractAction
    {
    public:
        DisconnectedHandler(bool& disconnected) :
            m_disconnected(disconnected) { ++allocationCounter; }
        ~DisconnectedHandler() { --allocationCounter; }

    private:
        void operator()() final
        {
            m_disconnected = true;
        }

    private:
        bool& m_disconnected;
    };

    bool clientDisconnected = false;
    client->setDisconnectedHandler(new DisconnectedHandler(clientDisconnected));

    struct ServerMessageHandler : public AbstractMessageHandler
    {
        ServerMessageHandler(AbstractConnection* conn) :
            m_conn(conn) { ++allocationCounter; }
        ~ServerMessageHandler() { --allocationCounter; }

    private:
        void operator()(const std::string& message) final
        {
            std::ostringstream out;
            out << "ECHO from " << std::to_string(m_conn->peerId()) << ": " << message;
            m_conn->sendMessage(out.str());
        }

    private:
        AbstractConnection* m_conn = nullptr;
    };

    class NewConnectionHandler : public AbstractNewConnectionHandler
    {
    public:
        NewConnectionHandler(std::map<uint64_t, bool>& disconnected) :
            m_disconnected(disconnected) { ++allocationCounter; }
        ~NewConnectionHandler() { --allocationCounter; }

    private:
        void operator()(AbstractConnection* conn) final
        {
            conn->setMessageHandler(new ServerMessageHandler(conn));
            conn->setDisconnectedHandler(new DisconnectedHandler(m_disconnected[conn->peerId()]));
        }

    private:
        std::map<uint64_t, bool>& m_disconnected;
    };
    std::map<uint64_t, bool> serverDisconnected;
    server->setNewConnectionHandler(new NewConnectionHandler(serverDisconnected));

    const uint64_t clientId = 11;
    const uint64_t serverId = 1;
    ASSERT(!client->bind(0));
    ASSERT(client->bind(clientId));
    ASSERT(!client->bind(clientId));
    ASSERT(!server->listen(0));
    ASSERT(server->listen(serverId));
    ASSERT(!server->listen(serverId));
    ASSERT(!client->connectToHost(0));
    ASSERT(client->connectToHost(serverId));

    ASSERT(!client->connected());
    ASSERT_EQUAL(client->peerId(), 0u);
    ASSERT(!server->connection(clientId));
    ASSERT_EQUAL(ostr.str(), "");
    while (taskQueue.processTask())
        ;
    ASSERT(client->connected());
    ASSERT_EQUAL(client->peerId(), serverId);
    auto* serverConn = server->connection(clientId);
    ASSERT(serverConn);
    ASSERT(serverConn->connected());
    ASSERT_EQUAL(serverConn->peerId(), clientId);
    ASSERT_EQUAL(ostr.str(), "ECHO from 11: connection request");

    ASSERT(!clientDisconnected);
    ASSERT(!serverDisconnected[clientId]);
    client->disconnect();
    ASSERT(!clientDisconnected);
    ASSERT(!serverDisconnected[clientId]);
    while (taskQueue.processTask())
        ;
    ASSERT(clientDisconnected);
    ASSERT(serverDisconnected[clientId]);

    delete client;
    delete server;
    ASSERT_EQUAL(allocationCounter, 0);
}
