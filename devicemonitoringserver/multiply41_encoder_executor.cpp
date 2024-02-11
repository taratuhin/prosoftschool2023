
/**
 *   \file     multiply41_encoder_executor.cpp
 *   \version  0.1
 *   \date     2024.02.09
 */

#include "multiply41_encoder_executor.h"
#include "big_endian.h"

#include <cstdint>
#include <sstream>


std::string  Multiply41EncoderExecutor::encode( const  std::string&  message )
{
    std::ostringstream  ostream(std::ios_base::binary);


    for ( auto ch : message )
    {
        auto  c = static_cast<unsigned char>( ch );
        to_big_endian( ostream, static_cast<uint16_t>( c * 41 ) );
    }

    return  ostream.str();
}


std::string  Multiply41EncoderExecutor::decode( const std::string&  message )
{
    std::istringstream  istream( message, std::ios_base::binary );
    std::string  decoded;


    while ( true )
    {
        auto  value = from_big_endian<uint16_t>( istream );


        if ( istream.fail() )
        {
            break;
        }

        decoded += static_cast<unsigned char>( value / 41 );
    }

    return  decoded;
}
