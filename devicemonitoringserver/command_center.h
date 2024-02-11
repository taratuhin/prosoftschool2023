
/**
 *   \file     command_center.h
 *   \version  0.1
 *   \date     2024.02.08
 */

#ifndef COMMAND_CENTER_H
#define COMMAND_CENTER_H


#include  "deviceworkschedule.h"

#include  <cstdint>
#include  <functional>
#include  <map>
#include  <vector>


class  DeviceMonitoringServer;
class  MessageMeterage;
class  Message;


/**
 *   \brief  Статистика СКО физических параметров от плана
 */
struct  DeviationStats
{
    Phase  phase;                ///* этап плана
    uint64_t  first_time_stamp;  ///* временная метка первого измерения для данного этапа плана
    double  deviation;           ///* СКО физического параметра от плана
};


/**
 *   \brief  Класс командного центра для управления и ведения статистики по физическим параметрам
 */
class  CommandCenter
{
    private:
        struct  ScheduleInfo
        {
            std::vector<Phase>  phases;
            size_t  current_phase_index;
        };

        struct  StatsInfo
        {
            std::vector<double>  square_diffs;
            std::vector<DeviationStats>  deviation_stats;
        };

        std::map<uint64_t, ScheduleInfo>  m_schedule_info;
        std::map<uint64_t, StatsInfo>  m_stats_info;
        std::map<uint64_t, uint64_t>  m_last_time_stamp;

    public:
        /**
         *   \brief  Установить план работы устройства
         */
        void set_schedule( const  DeviceWorkSchedule  & schedule );

        /**
         *   \brief   Обработать сообщение с измерением
         *   \param   deviceId - идентификатор устройства
         *   \param   meterage - сообшение с измерением
         *   \param   callback - коллбэк для отправки сообщения с результатом обработки
         *   \return  Нет
         */
        void process_meterage( uint64_t  device_id, MessageMeterage  meterage, std::function<void( uint64_t, const  Message & )> callback );

        /**
         *   \brief  Статистика СКО физических параметров от плана для устройства
         *   \param  device_id - идентификатор устройства
         */
        std::vector<DeviationStats>  deviation_stats( uint64_t  device_id );
};

#endif // COMMAND_CENTER_H
