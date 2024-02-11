
/**
 *   \file     rot3_encoder_executor.cpp
 *   \version  0.1
 *   \date     2024.02.09
 */

#include  "rot3_encoder_executor.h"

#include  <limits>


static  constexpr  int  start = std::numeric_limits<char>::min();
static  constexpr  int  end = std::numeric_limits<char>::max() + 1;


std::string  Rot3EncoderExecutor::encode( const  std::string&  message )
{
    std::string  encoded( message );


    for ( auto&  ch : encoded )
    {
        int  c = ch;


        if ( c >= start && c < end )
        {
            c += 3;

            if ( c >= end )
            {
                c -= end - start;
            }

            ch = c;
        }
    }

    return  encoded;
}


std::string  Rot3EncoderExecutor::decode( const  std::string&  message )
{
    std::string  decoded( message );


    for ( auto&  ch : decoded )
    {
        int c = ch;


        if ( c >= start && c < end )
        {
            c -= 3;

            if ( c < start )
            {
                c += end - start;
            }

            ch = c;
        }
    }

    return  decoded;
}
