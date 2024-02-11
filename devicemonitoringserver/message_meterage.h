
/**
 *   \file     message_meterage.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MESSAGE_METERAGE_H
#define MESSAGE_METERAGE_H


#include "message.h"

#include <cstdint>


/**
 *   \brief Класс сообщения с измерением физического параметра
 */
class MessageMeterage final : public Message
{
    private:
        const uint64_t m_time_stamp;
        const uint8_t m_meterage;

    public:
        /**
         *   \brief  Конструктор
         *   \param  time_stamp - временная метка измерения
         *   \param  meterage - величина измерения
         */
        MessageMeterage( uint64_t  time_stamp, uint8_t  meterage ) : m_time_stamp( time_stamp ), m_meterage( meterage ) {}
        /**
         *   \brief  Временная метка измерения
         */
        uint64_t  time_stamp() const { return  m_time_stamp; }
        /**
         *   \brief  Величина измерения
         */
        uint8_t  meterage() const { return  m_meterage; }
        bool  operator==( const  Message  & other ) const
        {
            const  MessageMeterage  * o = dynamic_cast<const  MessageMeterage  *>( &other );
            return  o && time_stamp() == o->time_stamp() && meterage() == o->meterage();
        }

        std::ostream  & print( std::ostream  & os ) const override
        {
            return  os << "MessageMeterage (timeStamp=" << time_stamp() << ", meterage=" << static_cast<int>( meterage() ) << ")";
        }
};

#endif // MESSAGE_METERAGE_H
