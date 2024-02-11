#ifndef DEVICEMONITORINGSERVER_H
#define DEVICEMONITORINGSERVER_H

#include  "command_center.h"
#include  "common.h"
#include  "message_encoder.h"
#include  "message_serializer.h"

#include <cstdint>
#include <string>

struct DeviceWorkSchedule;
class AbstractConnectionServer;
class AbstractConnection;

/*!
 * \brief Класс сервера для мониторинга состояния устройств.
 */
class DeviceMonitoringServer
{
    NON_COPYABLE(DeviceMonitoringServer)
public:
    /*!
     * \brief Конструктор.
     * \param connectionServer - владеющий указатель на сервер для приема подключений
     */
    DeviceMonitoringServer(AbstractConnectionServer* connectionServer);
    ~DeviceMonitoringServer();

    /*!
     * \brief Установить план работы устройств.
     */
    void setDeviceWorkSchedule(const DeviceWorkSchedule&);
    /*!
     * \brief Начать прием подключений по идентификатору \a serverId
     */
    bool listen(uint64_t serverId);

    /**
     *   \brief  Статистика СКО физических параметров от плана для устройства
     *   \param  device_id - идентификатор устройства
     */
    std::vector<DeviationStats> deviation_stats( uint64_t  device_id );

    /**
     *   \brief Ссылка на объект MessageEncoder для управления параметрами шифрования.
     */
    MessageEncoder& message_encoder();

private:
    /*!
     * \brief Отправить сообщение устройству.
     * \param deviceId - идентификатор устройства
     * \param message - сообщение
     */
    void sendMessage(uint64_t deviceId, const std::string& message);
    /*!
     * \brief Обработчик приема нового сообщения от устройства.
     * \param deviceId - идентификатор устройства
     * \param message - сообщение
     */
    void onMessageReceived(uint64_t deviceId, const std::string& message);
    /*!
     * \brief Обработчик поступления нового входящего подключения.
     * \param conn - невладеющий указатель на объект подключения
     */
    void onNewIncomingConnection(AbstractConnection* conn);
    /*!
     * \brief Обработчик разрыва соединения.
     * \param deviceId - идентификатор устройства
     */
    void onDisconnected(uint64_t clientId);

private:
    void addMessageHandler(AbstractConnection* conn);
    void addDisconnectedHandler(AbstractConnection* conn);
    void sendMessage(uint64_t deviceId, const Message& message);

private:
    AbstractConnectionServer* m_connectionServer = nullptr;
    CommandCenter  m_commandcenter;
    MessageSerializer  m_serializer;
    MessageEncoder  m_encoder;
};

#endif // DEVICEMONITORINGSERVER_H
