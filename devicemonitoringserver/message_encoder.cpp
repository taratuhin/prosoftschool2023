
/**
 *   \file     meassage_encoder.cpp
 *   \version  0.1
 *   \date     2024.02.09
 */

#include  "message_encoder.h"
#include  "base_encoder_executor.h"
#include  "mirror_encoder_executor.h"
#include  "multiply41_encoder_executor.h"
#include  "rot3_encoder_executor.h"


MessageEncoder::MessageEncoder()
{
    add_executor( new Rot3EncoderExecutor() );
    add_executor( new MirrorEncoderExecutor() );
    add_executor( new Multiply41EncoderExecutor() );
}


MessageEncoder::~MessageEncoder()
{
    for ( auto  executor : m_executors )
    {
        delete  executor;
    }
}


std::string  MessageEncoder::encode( const  std::string&  message )
{
    if ( !m_current_executor )
    {
        return  std::string();
    }

    return  m_current_executor->encode( message );
}


std::string  MessageEncoder::decode( const  std::string  & message )
{
    if ( !m_current_executor )
    {
        return  std::string();
    }

    return  m_current_executor->decode( message );
}


bool  MessageEncoder::select_executor( const  std::string  & name )
{
    for ( auto  executor : m_executors )
    {
        if ( executor->name() == name )
        {
            m_current_executor = executor;

            return  true;
        }
    }

    return  false;
}


bool  MessageEncoder::add_executor( BaseEncoderExecutor  * executor )
{
    if ( !executor )
    {
        return  false;
    }

    for ( auto  &e : m_executors )
    {
        if ( e->name() == executor->name() )
        {
            if ( m_current_executor == e )
            {
                m_current_executor = executor;
            }

            delete  e;
            e = executor;

            return  true;
        }
    }

    m_executors.push_back( executor );

    return  true;
}
