#ifndef TESTS_H
#define TESTS_H

void monitoringServerTestNoSchedule();
void monitoringServerTestObsolete();
void monitoringServerTestCommand();
void monitoringServerTestNoTimeStamp();
void monitoringServerTestTwoDevices();
void monitoringServerCryptoPositiveTest();
void monitoringServerCryptoNegativeTest();

void messageSerializationTest();

void messageEncoderEmptyTest();
void messageEncoderDummyTest();
void messageEncoderNegativeTest();
void messageEncoderAddTest();
void messageEncoderDoubleAddTest();
void messageEncoderRot3Test();
void messageEncoderMirrorTest();
void messageEncoderMultiply41Test();

void commandCenterNoScheduleTest();
void commandCenterObsoleteTest();
void commandCenterNoTimestampTest();
void commandCenterCommandZeroTest();
void commandCenterCommandUpTest();
void commandCenterCommandDownTest();
void commandCenterTwoDevicesTest();
void commandCenterDeviationTest();
void commandCenterDeviationNewScheduleTest();
void commandCenterNewScheduleTest();
void commandCenterUnsortedScheduleTest();
void commandCenterDublicateScheduleTest();

#endif // TESTS_H
