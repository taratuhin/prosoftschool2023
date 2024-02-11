
/**
 *   \file     big_endian.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H


#include  <istream>
#include  <ostream>

/**
 *   \brief  Функция сериализации целого числа \a value типа \a T в поток \a ostream в виде последовательности байтов в порядке Big Endian
 */
template <typename  T>
void  to_big_endian( std::ostream&  ostream, T  value )
{
    constexpr  size_t  bytes_num = sizeof( value );
    unsigned  char  buf[bytes_num];


    for ( size_t  i = 0; i < bytes_num; ++i )
    {
        buf[i] = value;
        value >>= 8;
    }

    for ( size_t  i = bytes_num; i > 0; --i )
    {
        ostream.put( buf[i - 1] );
    }
}


/**
 *   \brief  Функция десериализации целого числа типа \a T из потока \a istream в виде последовательности байтов в порядке Big Endian
 */
template <typename  T>
T  from_big_endian( std::istream&  istream )
{
    T  value {};
    for ( size_t  i = 0; i < sizeof( T ); ++i )
    {
        value <<= 8;
        value |= istream.get();
    }

    return  value;
}

#endif // BIG_ENDIAN_H
