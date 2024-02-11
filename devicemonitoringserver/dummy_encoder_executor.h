
/**
 *   \file     dummy_encoder_executor.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef DUMMY_ENCODER_EXECUTOR_H
#define DUMMY_ENCODER_EXECUTOR_H


#include  "base_encoder_executor.h"

class  DummyEncoderExecutor : public  BaseEncoderExecutor
{
    public:
        std::string  encode( const  std::string&  message ) override { return  message; }
        std::string  decode( const  std::string&  message ) override { return  message; }
        std::string  name() const override { return  "Dummy"; }
};

#endif // DUMMY_ENCODER_EXECUTOR_H
