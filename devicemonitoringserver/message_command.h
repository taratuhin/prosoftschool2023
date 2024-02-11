
/**
 *   \file     message_commond.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MESSAGE_COMMAND_H
#define MESSAGE_COMMAND_H


#include  "message.h"

#include  <cstdint>


/**
 *   \brief Класс сообщения с командой корректировки физического параметра
 */
class  MessageCommand  final : public  Message
{
    private:
        const  int8_t  m_command;

    public :
        /**
         *   \brief  Конструктор
         *   \param  command - величина коррекции физического параметра
         */
        MessageCommand( int8_t  command ) : m_command( command ) {}

        /**
         *   \brief  Величина коррекции физического параметра
         */
        int8_t  command() const { return  m_command; }

        bool  operator==( const  Message  & other ) const
        {
            const  MessageCommand  * o = dynamic_cast<const  MessageCommand  *>( &other );
            return o && command() == o->command();
        }

        std::ostream  & print( std::ostream  & os ) const override
        {
            return  os << "MessageCommand (command=" << command() << ")";
        }
};

#endif // MESSAGE_COMMAND_H
