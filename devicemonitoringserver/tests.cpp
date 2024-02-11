
/**
 *   \file     test.cpp
 *   \version  0.1
 *   \date     2024.02.11
 */

#include "tests.h"
#include "command_center.h"
#include "devicemock.h"
#include "devicemonitoringserver.h"
#include "deviceworkschedule.h"
#include "dummy_encoder_executor.h"
#include "message.h"
#include "message_command.h"
#include "message_encoder.h"
#include "message_error.h"
#include "message_meterage.h"
#include "message_serializer.h"
#include "test_runner.h"
#include <servermock/clientconnectionmock.h>
#include <servermock/connectionservermock.h>
#include <servermock/taskqueue.h>

#include <limits>

#define  compareVectorsOfSharedPtrs(a, b)  \
ASSERT_EQUAL(a.size(), b.size());     \
    for (size_t i = 0; i < a.size(); ++i) \
{                                     \
        ASSERT(a[i].get());               \
        ASSERT(b[i].get());               \
        ASSERT_EQUAL(*a[i], *b[i]);       \
}

struct  MonitoringServerTest
{
    MonitoringServerTest( uint64_t  serverId ) :
        serverId( serverId ),
        server( new  ConnectionServerMock( task_queue ) )
    {
        ASSERT( server.message_encoder().add_executor( new DummyEncoderExecutor() ) );
        ASSERT( server.message_encoder().select_executor( "Dummy" ) );
        ASSERT( server.listen( serverId ) );
    }

    ~MonitoringServerTest()
    {
        for ( auto  & value : devices )
        {
            delete  value.second;
        }
    }
    void  connectDevice( uint64_t  deviceId )
    {
        devices.insert( { deviceId, new DeviceMock( new ClientConnectionMock( task_queue ) ) } );
        ASSERT( devices[deviceId]->messageEncoder().add_executor( new  DummyEncoderExecutor() ) );
        ASSERT( devices[deviceId]->messageEncoder().select_executor( "Dummy" ) );
        ASSERT( devices[deviceId]->bind( deviceId ) );
        ASSERT( devices[deviceId]->connectToServer( serverId ) );
        while ( task_queue.processTask() )
            ;
    }

    uint64_t  serverId;
    TaskQueue  task_queue;
    DeviceMonitoringServer  server;
    std::map<uint64_t, DeviceMock *>  devices;
};


void  monitoringServerTestNoSchedule()
{
    MonitoringServerTest  test( 11u );
    uint64_t  deviceId = 111u;
    test.connectDevice( deviceId );
    std::vector<uint8_t>  meterages = { 0u };
    test.devices[deviceId]->setMeterages( meterages );
    test.devices[deviceId]->startMeterageSending();
    while ( test.task_queue.processTask() )
        ;

    std::vector<std::shared_ptr<Message>>  expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
    };
    auto  & messages = test.devices[deviceId]->messages();
    compareVectorsOfSharedPtrs( expected, messages );
}


void  monitoringServerTestNoTimeStamp()
{
    MonitoringServerTest  test(11u);
    uint64_t  deviceId = 111u;
    test.connectDevice(deviceId);
    DeviceWorkSchedule  schedule { deviceId, { { 1u, 0u } } };
    std::vector<uint8_t>  meterages = { 0u };
    test.server.setDeviceWorkSchedule( schedule );
    test.devices[deviceId]->setMeterages( meterages );
    test.devices[deviceId]->startMeterageSending();
    while ( test.task_queue.processTask() )
        ;

    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
    };
    auto& messages = test.devices[deviceId]->messages();
    compareVectorsOfSharedPtrs(expected, messages);
}

void monitoringServerTestObsolete()
{
    MonitoringServerTest test(11u);
    uint64_t deviceId = 111u;
    test.connectDevice(deviceId);
    std::vector<uint8_t> meterages = { 0u };
    test.devices[deviceId]->setMeterages(meterages);
    test.devices[deviceId]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    test.devices[deviceId]->setMeterages(meterages);
    test.devices[deviceId]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
    };
    auto& messages = test.devices[deviceId]->messages();
    compareVectorsOfSharedPtrs(expected, messages);
}

void monitoringServerTestCommand()
{
    MonitoringServerTest test(11u);
    uint64_t deviceId = 111u;
    test.connectDevice(deviceId);
    DeviceWorkSchedule schedule { deviceId, {
                                           { 0u, 0u },
                                           { 1u, 0u },
                                           { 2u, 100u },
                                           } };
    std::vector<uint8_t> meterages = { 0u, 100u, 0u };
    test.server.setDeviceWorkSchedule(schedule);
    test.devices[deviceId]->setMeterages(meterages);
    test.devices[deviceId]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(-100)),
        std::shared_ptr<Message>(new MessageCommand(100)),
    };
    auto& messages = test.devices[deviceId]->messages();
    compareVectorsOfSharedPtrs(expected, messages);
}

void monitoringServerTestTwoDevices()
{
    MonitoringServerTest test(11u);
    uint64_t deviceId1 = 111u, deviceId2 = 654u;
    test.connectDevice(deviceId1);
    test.connectDevice(deviceId2);
    test.server.setDeviceWorkSchedule({ deviceId1, {
                                                   { 0u, 0u },
                                                   { 1u, 0u },
                                                   { 2u, 3u },
                                                   } });
    test.server.setDeviceWorkSchedule({ deviceId2, {
                                                   { 1u, 100u },
                                                   { 2u, 50u },
                                                   { 3u, 0u },
                                                   } });
    test.devices[deviceId1]->setMeterages({ 0u, 1u, 2u });
    test.devices[deviceId2]->setMeterages({ 0u, 0u, 50u, 100u });
    test.devices[deviceId1]->startMeterageSending();
    test.devices[deviceId2]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    std::vector<std::shared_ptr<Message>> expected1 = {
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(-1)),
        std::shared_ptr<Message>(new MessageCommand(1)),
    };
    auto& messages1 = test.devices[deviceId1]->messages();
    compareVectorsOfSharedPtrs(expected1, messages1);
    std::vector<std::shared_ptr<Message>> expected2 = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
        std::shared_ptr<Message>(new MessageCommand(100)),
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(-100)),
    };
    auto& messages2 = test.devices[deviceId2]->messages();
    compareVectorsOfSharedPtrs(expected2, messages2);
}

void monitoringServerCryptoPositiveTest()
{
    MonitoringServerTest test(11u);
    ASSERT(test.server.message_encoder().select_executor("ROT3"));
    uint64_t deviceId = 111u;
    test.connectDevice(deviceId);
    ASSERT(test.devices[deviceId]->messageEncoder().select_executor("ROT3"));
    DeviceWorkSchedule schedule { deviceId, {
                                           { 0u, 0u },
                                           { 1u, 0u },
                                           { 2u, 3u },
                                           } };
    std::vector<uint8_t> meterages = { 0u, 1u, 2u };
    test.server.setDeviceWorkSchedule(schedule);
    test.devices[deviceId]->setMeterages(meterages);
    test.devices[deviceId]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(-1)),
        std::shared_ptr<Message>(new MessageCommand(1)),
    };
    auto& messages = test.devices[deviceId]->messages();
    compareVectorsOfSharedPtrs(expected, messages);
}

void monitoringServerCryptoNegativeTest()
{
    MonitoringServerTest test(11u);
    ASSERT(test.server.message_encoder().select_executor("ROT3"));
    uint64_t deviceId = 111u;
    test.connectDevice(deviceId);
    ASSERT(test.devices[deviceId]->messageEncoder().select_executor("Mirror"));
    DeviceWorkSchedule schedule { deviceId, {
                                           { 0u, 0u },
                                           { 1u, 0u },
                                           { 2u, 3u },
                                           } };
    std::vector<uint8_t> meterages = { 0u, 1u, 2u };
    test.server.setDeviceWorkSchedule(schedule);
    test.devices[deviceId]->setMeterages(meterages);
    test.devices[deviceId]->startMeterageSending();
    while (test.task_queue.processTask())
        ;

    ASSERT(test.devices[deviceId]->messages().empty());
}

void messageSerializationTest()
{
    MessageSerializer serializer;
    std::vector<std::shared_ptr<Message>> messages;

    auto callback = [&](const Message& msg) {
        do
        {
            const MessageCommand* messageCommand = dynamic_cast<const MessageCommand*>(&msg);
            if (messageCommand)
            {
                messages.push_back(std::shared_ptr<Message>(new MessageCommand(*messageCommand)));
                break;
            }
            const MessageError* messageError = dynamic_cast<const MessageError*>(&msg);
            if (messageError)
            {
                messages.push_back(std::shared_ptr<Message>(new MessageError(*messageError)));
                break;
            }
            const MessageMeterage* messageMeterage = dynamic_cast<const MessageMeterage*>(&msg);
            if (messageMeterage)
            {
                messages.push_back(std::shared_ptr<Message>(new MessageMeterage(*messageMeterage)));
                break;
            }
        } while (false);
    };

    ASSERT_EQUAL(serializer.deserialize("", callback), false);
    ASSERT_EQUAL(serializer.deserialize("12345", callback), false);

    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(0)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(1)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(-1)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(123)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(std::numeric_limits<int8_t>::max())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageCommand(std::numeric_limits<int8_t>::min())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageMeterage(12345u, 123u)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageMeterage(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint8_t>::min())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageMeterage(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint8_t>::min())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageMeterage(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint8_t>::max())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageMeterage(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint8_t>::max())), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageError(MessageError::ErrorType::NoSchedule)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageError(MessageError::ErrorType::NoTimestamp)), callback), true);
    ASSERT_EQUAL(serializer.deserialize(serializer.serialize(MessageError(MessageError::ErrorType::Obsolete)), callback), true);

    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(1)),
        std::shared_ptr<Message>(new MessageCommand(-1)),
        std::shared_ptr<Message>(new MessageCommand(123)),
        std::shared_ptr<Message>(new MessageCommand(std::numeric_limits<int8_t>::max())),
        std::shared_ptr<Message>(new MessageCommand(std::numeric_limits<int8_t>::min())),
        std::shared_ptr<Message>(new MessageMeterage(12345u, 123u)),
        std::shared_ptr<Message>(new MessageMeterage(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint8_t>::min())),
        std::shared_ptr<Message>(new MessageMeterage(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint8_t>::min())),
        std::shared_ptr<Message>(new MessageMeterage(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint8_t>::max())),
        std::shared_ptr<Message>(new MessageMeterage(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint8_t>::max())),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
    };
    compareVectorsOfSharedPtrs(expected, messages);
}

void messageEncoderEmptyTest()
{
    MessageEncoder encoder;
    ASSERT(encoder.encode("123").empty());
    ASSERT(encoder.decode("123").empty());
}

void messageEncoderDummyTest()
{
    MessageEncoder encoder;
    ASSERT(encoder.add_executor(new DummyEncoderExecutor()));
    ASSERT(encoder.select_executor("Dummy"));
    std::string message;
    const int start = std::numeric_limits<unsigned char>::min(),
        end = std::numeric_limits<unsigned char>::max() + 1;
    const size_t len = end - start;
    for (int i = start; i < end; ++i)
    {
        message += i;
    }
    ASSERT_EQUAL(len, message.length());

    auto encoded = encoder.encode(message);
    ASSERT_EQUAL(len, encoded.length());
    ASSERT_EQUAL(message, encoded);

    auto decoded = encoder.decode(encoded);
    ASSERT_EQUAL(len, decoded.length());

    ASSERT_EQUAL(message, decoded);
}

void messageEncoderNegativeTest()
{
    MessageEncoder encoder;
    ASSERT_EQUAL(false, encoder.add_executor(nullptr));
    ASSERT_EQUAL(false, encoder.select_executor("123"));
    std::string message = "123";
    ASSERT(encoder.encode(message).empty());
    ASSERT(encoder.decode(message).empty());
}

void messageEncoderAddTest()
{
    class TestEncoderExecutor : public BaseEncoderExecutor
    {
    public:
        TestEncoderExecutor(const std::string& message) :
            test_message(message) {}
        std::string encode(const std::string&) override { return test_message; }
        std::string decode(const std::string&) override { return test_message; }
        std::string name() const override { return "Test"; }

    private:
        std::string test_message;
    };
    MessageEncoder encoder;
    ASSERT(encoder.add_executor(new TestEncoderExecutor("123")));
    ASSERT(encoder.select_executor("Test"));
    ASSERT(encoder.add_executor(new DummyEncoderExecutor()));
    ASSERT_EQUAL(std::string("123"), encoder.encode("test"));
    ASSERT_EQUAL(std::string("123"), encoder.decode("test"));
}

void messageEncoderDoubleAddTest()
{
    class TestEncoderExecutor : public BaseEncoderExecutor
    {
    public:
        TestEncoderExecutor(const std::string& message) :
            test_message(message) {}
        std::string encode(const std::string&) override { return test_message; }
        std::string decode(const std::string&) override { return test_message; }
        std::string name() const override { return "Test"; }

    private:
        std::string test_message;
    };
    MessageEncoder encoder;
    ASSERT(encoder.add_executor(new TestEncoderExecutor("123")));
    ASSERT(encoder.select_executor("Test"));
    ASSERT_EQUAL(std::string("123"), encoder.encode("test"));
    ASSERT_EQUAL(std::string("123"), encoder.decode("test"));
    ASSERT(encoder.add_executor(new TestEncoderExecutor("456")));
    ASSERT_EQUAL(std::string("456"), encoder.encode("test"));
    ASSERT_EQUAL(std::string("456"), encoder.decode("test"));
}

void messageEncoderRot3Test()
{
    MessageEncoder encoder;
    ASSERT(encoder.select_executor("ROT3"));
    std::string message;
    const int start = std::numeric_limits<unsigned char>::min(),
        end = std::numeric_limits<unsigned char>::max() + 1;
    const size_t len = end - start;
    for (int i = start; i < end; ++i)
    {
        message += i;
    }
    ASSERT_EQUAL(len, message.length());

    auto encoded = encoder.encode(message);
    ASSERT_EQUAL(len, encoded.length());
    for (size_t i = 0; i < len; ++i)
    {
        int enc = message[i] + 3;
        if (enc >= end)
        {
            enc -= end - start;
        }
        ASSERT_EQUAL(static_cast<char>(enc), encoded[i]);
    }

    auto decoded = encoder.decode(encoded);
    ASSERT_EQUAL(len, decoded.length());

    ASSERT_EQUAL(message, decoded);
}

void messageEncoderMirrorTest()
{
    MessageEncoder encoder;
    ASSERT(encoder.select_executor("Mirror"));
    std::string message;
    const int start = std::numeric_limits<unsigned char>::min(),
        end = std::numeric_limits<unsigned char>::max() + 1;
    const size_t len = end - start;
    for (int i = start; i < end; ++i)
    {
        message += i;
    }
    ASSERT_EQUAL(len, message.length());

    auto encoded = encoder.encode(message);
    ASSERT_EQUAL(len * 3, encoded.length());
    std::vector<uint8_t> results;
    for (size_t i = 0; i < len; ++i)
    {
        unsigned char ch = message[i];
        if (ch == 0 || ch == 9 || ch == 90 || ch == 91 || ch == 100 || ch == 132 || ch == 159 || ch == 255)
        {
            results.push_back(encoded[3 * i]);
            results.push_back(encoded[3 * i + 1]);
            results.push_back(encoded[3 * i + 2]);
        }
    }
    std::vector<uint8_t> expected = {
        0,
        0,
        0,
        1,
        0,
        9,
        2,
        0,
        9,
        2,
        0,
        19,
        3,
        0,
        1,
        3,
        0,
        231,
        3,
        3,
        183,
        3,
        2,
        40,
    };
    ASSERT_EQUAL(expected, results);

    auto decoded = encoder.decode(encoded);
    ASSERT_EQUAL(len, decoded.length());

    ASSERT_EQUAL(message, decoded);
}

void messageEncoderMultiply41Test()
{
    MessageEncoder encoder;
    ASSERT(encoder.select_executor("Multiply41"));
    std::string message;
    const int start = std::numeric_limits<unsigned char>::min(),
        end = std::numeric_limits<unsigned char>::max() + 1;
    const size_t len = end - start;
    for (int i = start; i < end; ++i)
    {
        message += i;
    }
    ASSERT_EQUAL(len, message.length());

    auto encoded = encoder.encode(message);
    ASSERT_EQUAL(len * 2, encoded.length());
    std::vector<uint8_t> results;
    for (size_t i = 0; i < len; ++i)
    {
        unsigned char ch = message[i];
        if (ch == 0 || ch == 1 || ch == 6 || ch == 7 || ch == 255)
        {
            results.push_back(encoded[2 * i]);
            results.push_back(encoded[2 * i + 1]);
        }
    }
    std::vector<uint8_t> expected = {
        0,
        0,
        0,
        41,
        0,
        246,
        1,
        31,
        40,
        215,
    };
    ASSERT_EQUAL(expected, results);

    auto decoded = encoder.decode(encoded);
    ASSERT_EQUAL(len, decoded.length());

    ASSERT_EQUAL(message, decoded);
}

struct CommandCenterCallback
{
    CommandCenterCallback(uint64_t deviceId) :
        deviceId(deviceId) {}

    void operator()(uint64_t id, const Message& msg)
    {
        ASSERT_EQUAL(id, deviceId);
        do
        {
            const MessageCommand* messageCommand = dynamic_cast<const MessageCommand*>(&msg);
            if (messageCommand)
            {
                messages.push_back(std::shared_ptr<Message>(new MessageCommand(*messageCommand)));
                break;
            }
            const MessageError* messageError = dynamic_cast<const MessageError*>(&msg);
            if (messageError)
            {
                messages.push_back(std::shared_ptr<Message>(new MessageError(*messageError)));
                break;
            }
            ASSERT(false);
        } while (false);
    }
    uint64_t deviceId;
    std::vector<std::shared_ptr<Message>> messages;
};

void commandCenterNoScheduleTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    MessageMeterage meterage = MessageMeterage(0u, 0u);
    center.process_meterage(deviceId, meterage, std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterNoTimestampTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    MessageMeterage meterage = MessageMeterage(0u, 0u);
    center.set_schedule({ deviceId, { { 1u, 0u } } });
    center.process_meterage(deviceId, meterage, std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterCommandZeroTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    MessageMeterage meterage = MessageMeterage(0u, 0u);
    center.set_schedule({ deviceId, { { 0u, 0u } } });
    center.process_meterage(deviceId, meterage, std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(0)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterCommandUpTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    MessageMeterage meterage = MessageMeterage(0u, 49u);
    center.set_schedule({ deviceId, { { 0u, 100u } } });
    center.process_meterage(deviceId, meterage, std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(51)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterCommandDownTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    MessageMeterage meterage = MessageMeterage(0u, 100u);
    center.set_schedule({ deviceId, { { 0u, 49u } } });
    center.process_meterage(deviceId, meterage, std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(-51)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterObsoleteTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.set_schedule({ deviceId, { { 2u, 50u } } });
    center.process_meterage(deviceId, MessageMeterage(1u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
        std::shared_ptr<Message>(new MessageCommand(-50)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterTwoDevicesTest()
{
    CommandCenter center;
    MessageMeterage meterage = MessageMeterage(0u, 100u);
    uint64_t deviceId1 = 111u, deviceId2 = 222u;
    CommandCenterCallback callback1(deviceId1), callback2(deviceId2);
    center.set_schedule({ deviceId1, { { 0u, 10u } } });
    center.set_schedule({ deviceId2, { { 0u, 50u } } });
    center.process_meterage(deviceId1, meterage, std::ref(callback1));
    center.process_meterage(deviceId1, meterage, std::ref(callback1));
    center.process_meterage(deviceId2, meterage, std::ref(callback2));

    std::vector<std::shared_ptr<Message>> expected1 = {
        std::shared_ptr<Message>(new MessageCommand(-90)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
    };
    compareVectorsOfSharedPtrs(expected1, callback1.messages);

    std::vector<std::shared_ptr<Message>> expected2 = {
        std::shared_ptr<Message>(new MessageCommand(-50)),
    };
    compareVectorsOfSharedPtrs(expected2, callback2.messages);
}

void commandCenterUnsortedScheduleTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.set_schedule({ deviceId, { { 3u, 30u }, { 0u, 0u }, { 2u, 20u }, { 1u, 10u } } });
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(3u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(4u, 100u), std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(-100)),
        std::shared_ptr<Message>(new MessageCommand(-90)),
        std::shared_ptr<Message>(new MessageCommand(-80)),
        std::shared_ptr<Message>(new MessageCommand(-70)),
        std::shared_ptr<Message>(new MessageCommand(-70)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterDublicateScheduleTest()
{
    CommandCenter  center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.set_schedule({ deviceId, { { 0u, 0u }, { 0u, 0u }, { 0u, 0u }, { 1u, 10u } } });
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(-100)),
        std::shared_ptr<Message>(new MessageCommand(-90)),
        std::shared_ptr<Message>(new MessageCommand(-90)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterNewScheduleTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.set_schedule({ deviceId, { { 0u, 100u }, { 1u, 50u } } });
    center.process_meterage(deviceId, MessageMeterage(0u, 99u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 50u), std::ref(callback));
    center.set_schedule({ deviceId, { { 3u, 0u } } });
    center.process_meterage(deviceId, MessageMeterage(3u, 50u), std::ref(callback));
    center.set_schedule({ deviceId, { { 0u, 100u }, { 1u, 50u }, { 3u, 0u }, { 4u, 10u } } });
    center.process_meterage(deviceId, MessageMeterage(4u, 40u), std::ref(callback));
    center.set_schedule({ deviceId, { { 10u, 0u } } });
    center.process_meterage(deviceId, MessageMeterage(5u, 50u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(5u, 50u), std::ref(callback));
    center.set_schedule({ deviceId, {} });
    center.process_meterage(deviceId, MessageMeterage(6u, 50u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(6u, 50u), std::ref(callback));
    std::vector<std::shared_ptr<Message>> expected = {
        std::shared_ptr<Message>(new MessageCommand(1)),
        std::shared_ptr<Message>(new MessageCommand(-50)),
        std::shared_ptr<Message>(new MessageCommand(0)),
        std::shared_ptr<Message>(new MessageCommand(-50)),
        std::shared_ptr<Message>(new MessageCommand(-30)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoTimestamp)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::NoSchedule)),
        std::shared_ptr<Message>(new MessageError(MessageError::ErrorType::Obsolete)),
    };
    compareVectorsOfSharedPtrs(expected, callback.messages);
}

void commandCenterDeviationTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.set_schedule({ deviceId, { { 0u, 50u }, { 3u, 90u } } });
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 0u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(3u, 0u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(4u, 50u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(5u, 100u), std::ref(callback));

    const auto& deviations = center.deviation_stats(deviceId);
    std::vector<DeviationStats> expected = { { { 0u, 50u }, 0u, 50.0 }, { { 3u, 90u }, 3u, 57.15 } };
    ASSERT_EQUAL(expected.size(), deviations.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        ASSERT_WITH_THRESHOLD(expected[i].deviation, deviations[i].deviation, 1.0);
        ASSERT_EQUAL(expected[i].phase.timeStamp, deviations[i].phase.timeStamp);
        ASSERT_EQUAL(expected[i].phase.value, deviations[i].phase.value);
    }
}

void commandCenterDeviationNewScheduleTest()
{
    CommandCenter center;
    uint64_t deviceId = 123u;
    CommandCenterCallback callback(deviceId);
    center.set_schedule({ deviceId, { { 0u, 50u }, { 3u, 90u } } });
    center.process_meterage(deviceId, MessageMeterage(0u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(1u, 0u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(2u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(3u, 80u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(4u, 100u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(5u, 80u), std::ref(callback));
    center.set_schedule({ deviceId, { { 0u, 0u }, { 6u, 30u } } });
    center.process_meterage(deviceId, MessageMeterage(6u, 0u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(7u, 60u), std::ref(callback));
    center.process_meterage(deviceId, MessageMeterage(8u, 0u), std::ref(callback));
    center.set_schedule({ deviceId, { { 0u, 0u } } });
    center.process_meterage(deviceId, MessageMeterage(9u, 50u), std::ref(callback));

    const auto& deviations = center.deviation_stats( deviceId );
    std::vector<DeviationStats> expected = {
                                            { { 0u, 50u }, 0u, 50.0 },
                                            { { 3u, 90u }, 3u, 10.0 },
                                            { { 6u, 30u }, 6u, 30.0 },
                                            { { 0u, 0u }, 9u, 50.0 },
                                            };
    ASSERT_EQUAL(expected.size(), deviations.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        ASSERT_WITH_THRESHOLD(expected[i].deviation, deviations[i].deviation, 1.0);
        ASSERT_EQUAL(expected[i].phase.timeStamp, deviations[i].phase.timeStamp);
        ASSERT_EQUAL(expected[i].phase.value, deviations[i].phase.value);
    }
}
