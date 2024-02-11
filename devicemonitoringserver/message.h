
/**
 *   \file     message.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MESSAGE_H
#define MESSAGE_H


#include <ostream>

/**
 *   \brief Абстрактный базовый класс для сообщений
 */
class  Message
{
    public :
        virtual  ~Message() = default;

    /**
     *   \brief Вывод сообщения в виде строки для отладки
     */
    virtual  std::ostream  & print( std::ostream  & os ) const = 0;
    virtual  bool  operator==( const  Message  & other ) const = 0;
    bool  operator!=( const  Message  & other ) const
    {
        return  !( *this == other );
    }
};


inline  std::ostream  & operator<<( std::ostream  & os, const  Message  & message )
{
    return  message.print( os );
}

#endif // MESSAGE_H
