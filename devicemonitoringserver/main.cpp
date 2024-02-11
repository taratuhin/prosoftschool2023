#include "test_runner.h"
#include "tests.h"
#include <servermock/servertests.h>

int main()
{
    TestRunner  tr;
    RUN_TEST(tr, taskQueueTest);
    RUN_TEST(tr, safeObjectPointerTest);
    RUN_TEST(tr, connectionChannelTest);
    RUN_TEST(tr, clientServerTest);

    RUN_TEST( tr, message_serialization_test );

    RUN_TEST( tr, message_encoder_empty_test );
    RUN_TEST( tr, message_encoder_dummy_test );
    RUN_TEST( tr, message_encoder_negative_test );
    RUN_TEST( tr, message_encoder_add_test );
    RUN_TEST( tr, message_encoder_double_add_test );
    RUN_TEST( tr, message_encoder_rot3_test );
    RUN_TEST( tr, message_encoder_mirror_test );
    RUN_TEST( tr, message_encoder_multiply41_test );

    RUN_TEST( tr, command_center_no_schedule_test );
    RUN_TEST( tr, command_center_no_timestamp_test );
    RUN_TEST( tr, command_center_command_zero_test );
    RUN_TEST( tr, command_center_command_up_test );
    RUN_TEST( tr, command_center_command_down_test );
    RUN_TEST( tr, command_center_obsolete_test );
    RUN_TEST( tr, command_center_two_devices_test );
    RUN_TEST( tr, command_center_unsorted_schedule_test );
    RUN_TEST( tr, command_center_dublicate_schedule_test );
    RUN_TEST( tr, command_center_new_schedule_test );
    RUN_TEST( tr, command_center_deviation_test );
    RUN_TEST( tr, command_center_deviation_new_schedule_test );

    RUN_TEST( tr, monitoring_server_test_no_schedule );
    RUN_TEST( tr, monitoring_server_test_obsolete );
    RUN_TEST( tr, monitoring_server_test_command );
    RUN_TEST( tr, monitoring_server_test_no_time_stamp );
    RUN_TEST( tr, monitoring_server_test_two_devices );
    RUN_TEST( tr, monitoring_server_crypto_positive_test );
    RUN_TEST( tr, monitoring_server_crypto_negative_test );

    return  0;
}
