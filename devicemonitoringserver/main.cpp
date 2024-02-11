#include "test_runner.h"
#include "tests.h"
#include <servermock/servertests.h>

int main()
{
    TestRunner tr;
    RUN_TEST(tr, taskQueueTest);
    RUN_TEST(tr, safeObjectPointerTest);
    RUN_TEST(tr, connectionChannelTest);
    RUN_TEST(tr, clientServerTest);

    RUN_TEST(tr, messageSerializationTest);

    RUN_TEST(tr, messageEncoderEmptyTest);
    RUN_TEST(tr, messageEncoderDummyTest);
    RUN_TEST(tr, messageEncoderNegativeTest);
    RUN_TEST(tr, messageEncoderAddTest);
    RUN_TEST(tr, messageEncoderDoubleAddTest);
    RUN_TEST(tr, messageEncoderRot3Test);
    RUN_TEST(tr, messageEncoderMirrorTest);
    RUN_TEST(tr, messageEncoderMultiply41Test);

    RUN_TEST(tr, commandCenterNoScheduleTest);
    RUN_TEST(tr, commandCenterNoTimestampTest);
    RUN_TEST(tr, commandCenterCommandZeroTest);
    RUN_TEST(tr, commandCenterCommandUpTest);
    RUN_TEST(tr, commandCenterCommandDownTest);
    RUN_TEST(tr, commandCenterObsoleteTest);
    RUN_TEST(tr, commandCenterTwoDevicesTest);
    RUN_TEST(tr, commandCenterUnsortedScheduleTest);
    RUN_TEST(tr, commandCenterDublicateScheduleTest);
    RUN_TEST(tr, commandCenterNewScheduleTest);
    RUN_TEST(tr, commandCenterDeviationTest);
    RUN_TEST(tr, commandCenterDeviationNewScheduleTest);

    RUN_TEST(tr, monitoringServerTestNoSchedule);
    RUN_TEST(tr, monitoringServerTestObsolete);
    RUN_TEST(tr, monitoringServerTestCommand);
    RUN_TEST(tr, monitoringServerTestNoTimeStamp);
    RUN_TEST(tr, monitoringServerTestTwoDevices);
    RUN_TEST(tr, monitoringServerCryptoPositiveTest);
    RUN_TEST(tr, monitoringServerCryptoNegativeTest);

    return 0;
}
