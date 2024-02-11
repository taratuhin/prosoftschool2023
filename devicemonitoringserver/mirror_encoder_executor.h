
/**
 *   \file     mirror_encoder_executor.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MIRROR_ENCODER_EXECUTOR_H
#define MIRROR_ENCODER_EXECUTOR_H


#include "base_encoder_executor.h"

class  MirrorEncoderExecutor : public  BaseEncoderExecutor
{
    public:
        std::string  encode( const  std::string&  message )  override;
        std::string  decode( const  std::string&  message )  override;
        std::string  name() const override { return  "Mirror"; }
};

#endif // MIRROR_ENCODER_EXECUTOR_H
