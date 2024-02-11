
/**
 *   \file     rot3_encoder_executor.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef  ROT3_ENCODER_EXECUTOR_H
#define  ROT3_ENCODER_EXECUTOR_H


#include  "base_encoder_executor.h"

class  Rot3EncoderExecutor : public BaseEncoderExecutor
{
    public:
        std::string  encode( const  std::string&  message ) override;
        std::string  decode( const  std::string&  message ) override;
        std::string  name() const override { return "ROT3"; }
};

#endif // ROT3_ENCODER_EXECUTOR_H
