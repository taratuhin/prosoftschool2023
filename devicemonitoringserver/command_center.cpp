
/**
 *   \file     command_center.cpp
 *   \version  0.1
 *   \date     2024.02.08
 */

#include "command_center.h"

#include "message_command.h"
#include "message_error.h"
#include "message_meterage.h"

#include <algorithm>
#include <cmath>
#include <numeric>

void CommandCenter::set_schedule(const DeviceWorkSchedule& schedule)
{
    m_schedule_info[schedule.deviceId] = {};
    auto& scheduleInfo = m_schedule_info[schedule.deviceId];
    scheduleInfo.phases = schedule.schedule;
    std::sort(scheduleInfo.phases.begin(),
              scheduleInfo.phases.end(),
              [](const Phase& phaseA, const Phase& phaseB) { return phaseA.timeStamp < phaseB.timeStamp; });
}

void CommandCenter::process_meterage(uint64_t deviceId, MessageMeterage meterage, std::function<void(uint64_t, const Message&)> callback)
{
    auto currentTimeStamp = meterage.time_stamp();

    auto lastTimeStampIt = m_last_time_stamp.find(deviceId);
    if (lastTimeStampIt != m_last_time_stamp.end() && lastTimeStampIt->second >= currentTimeStamp)
    {
        callback(deviceId, MessageError(MessageError::ErrorType::Obsolete));
        return;
    }
    m_last_time_stamp[deviceId] = currentTimeStamp;

    auto& scheduleInfo = m_schedule_info[deviceId];
    auto& statsInfo = m_stats_info[deviceId];
    if (scheduleInfo.phases.empty())
    {
        callback(deviceId, MessageError(MessageError::ErrorType::NoSchedule));
        return;
    }

    while (scheduleInfo.current_phase_index + 1 < scheduleInfo.phases.size()
           && scheduleInfo.phases[scheduleInfo.current_phase_index + 1].timeStamp <= currentTimeStamp)
    {
        ++scheduleInfo.current_phase_index;
    }

    auto currentPhase = scheduleInfo.phases[scheduleInfo.current_phase_index];

    if (currentPhase.timeStamp > currentTimeStamp)
    {
        callback(deviceId, MessageError(MessageError::ErrorType::NoTimestamp));
        return;
    }

    int command = currentPhase.value - meterage.meterage();

    auto lastDeviationStatIt = statsInfo.deviation_stats.rbegin();
    if (lastDeviationStatIt == statsInfo.deviation_stats.rend()
        || lastDeviationStatIt->phase.timeStamp != currentPhase.timeStamp
        || lastDeviationStatIt->phase.value != currentPhase.value)
    {
        statsInfo.square_diffs.clear();
        statsInfo.deviation_stats.push_back({ currentPhase, currentTimeStamp, 0.0 });
        lastDeviationStatIt = statsInfo.deviation_stats.rbegin();
    }
    statsInfo.square_diffs.push_back(command * command);
    double squareDiffsSum = std::accumulate(statsInfo.square_diffs.cbegin(), statsInfo.square_diffs.cend(), 0.0);
    double deviation = std::sqrt(squareDiffsSum / statsInfo.square_diffs.size());
    lastDeviationStatIt->deviation = deviation;

    callback(deviceId, MessageCommand(command));
}

std::vector<DeviationStats> CommandCenter::deviation_stats(uint64_t device_id)
{
    return m_stats_info[device_id].deviation_stats;
}
