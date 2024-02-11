
/**
 *   \file     multiply41_encoder_executor.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MULTIPLY41_ENCODER_EXECUTOR_H
#define MULTIPLY41_ENCODER_EXECUTOR_H


#include "base_encoder_executor.h"


class  Multiply41EncoderExecutor : public  BaseEncoderExecutor
{
    public:
        std::string  encode( const  std::string&  message ) override;
        std::string  decode( const  std::string&  message ) override;
        std::string  name() const override { return "Multiply41"; }
};

#endif // MULTIPLY41_ENCODER_EXECUTOR_H
