#ifndef CONNECTIONCHANNEL_H
#define CONNECTIONCHANNEL_H

#include "object.h"

#include <cstdint>
#include <string>

class TaskQueue;
class AbstractMessageHandler;
class AbstractAction;

/*!
 * \brief Класс канала передачи сообщений.
 */
class ConnectionChannel final : public Object
{
public:
    ConnectionChannel(TaskQueue& taskQueue, uint64_t peerId);
    ~ConnectionChannel();

    /*!
     * \brief Идентификатор связанного узла (клиента или сервера)
     */
    uint64_t peerId() const { return m_peerId; }
    /*!
     * \brief Канал активен.
     */
    bool connected() const { return m_connected; }
    /*!
     * \brief Отправить сообщение.
     * \param message - сообщение
     */
    void sendMessage(const std::string& message);
    /*!
     * \brief Выполнить соединение каналов.
     * \param peer - невладеющий указатель на связанный канал
     */
    void connect(ConnectionChannel* peer);
    /*!
     * \brief Выполнить разрыв соединения.
     */
    void disconnect();
    /*!
     * \brief Установить обработчик события получения сообщения.
     * \param handler - невладеющий указатель на обработчик
     */
    void setMessageHandler(AbstractMessageHandler* handler);
    /*!
     * \brief Установить обработчик события разрыва соединения.
     * \param handler - невладеющий указатель на обработчик
     */
    void setDisconnectedHandler(AbstractAction* handler);
    /*!
     * \brief Сообщить о получении сообщения.
     * \param message - сообщение
     */
    void onMessageReceived(const std::string& message) const;
    /*!
     * \brief Сообщить о разрыве соединения.
     */
    void onDisconnected();

private:
    void disconnectPeer();

private:
    TaskQueue& m_taskQueue;
    uint64_t m_peerId = 0;
    AbstractMessageHandler* m_messageHandler = nullptr;
    AbstractAction* m_disconnectedHandler = nullptr;
    SafeObjectPointer m_peerChannel;
    bool m_connected = false;
};

#endif // CONNECTIONCHANNEL_H
