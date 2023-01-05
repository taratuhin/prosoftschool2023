#ifndef DEVICEWORKSCHEDULE_H
#define DEVICEWORKSCHEDULE_H

#include <cstdint>
#include <vector>

/*!
 * \brief Параметры этапа.
 */
struct Phase
{
    uint64_t timeStamp = 0; ///< Метка времени начала этапа
    uint8_t value = 0;      ///< Целевое значение этапа
};

/*!
 * \brief План работы устройства.
 */
struct DeviceWorkSchedule
{
    uint64_t deviceId = 0;       ///< Идентификатор устройства
    std::vector<Phase> schedule; ///< План работы устройства
};

#endif // DEVICEWORKSCHEDULE_H
