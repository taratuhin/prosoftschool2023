
/**
*   \file     test.h
*   \version  0.1
*   \date     2024.02.11
*/

#ifndef TESTS_H
#define TESTS_H


void  message_serialization_test();
void  message_encoder_empty_test();
void  message_encoder_dummy_test();
void  message_encoder_negative_test();
void  message_encoder_add_test();
void  message_encoder_double_add_test();
void  message_encoder_rot3_test();
void  message_encoder_mirror_test();
void  message_encoder_multiply41_test();

void  command_center_no_schedule_test();
void  command_center_no_timestamp_test();
void  command_center_command_zero_test();
void  command_center_command_up_test();
void  command_center_command_down_test();
void  command_center_obsolete_test();
void  command_center_two_devices_test();
void  command_center_unsorted_schedule_test();
void  command_center_dublicate_schedule_test();
void  command_center_new_schedule_test();
void  command_center_deviation_test();
void  command_center_deviation_new_schedule_test();

void  monitoring_server_test_no_schedule();
void  monitoring_server_test_obsolete();
void  monitoring_server_test_command();
void  monitoring_server_test_no_time_stamp();
void  monitoring_server_test_two_devices();
void  monitoring_server_crypto_positive_test();
void  monitoring_server_crypto_negative_test();

#endif // TESTS_H
