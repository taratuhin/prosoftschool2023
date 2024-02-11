
/**
 *   \file     message_error.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef MESSAGE_ERROR_H
#define MESSAGE_ERROR_H


#include  "message.h"


/**
 *   \brief  Класс сообщения об ошибке
 */
class  MessageError final : public Message
{
    public:
        /**
         *   \brief  Типы ошибок
         */
        enum  class  ErrorType
        {
            NoSchedule,
            NoTimestamp,
            Obsolete
        };

    private:
        const  ErrorType  m_error_type;

    public:

    /**
     *   \brief  Конструктор
     *   \param  errorType - тип ошибки
     */
    MessageError( ErrorType  error_type ) : m_error_type( error_type ) {}

    /**
     *   \brief  Тип ошибки
     */
    ErrorType  error_type() const
    {
        return  m_error_type;
    }

    bool  operator==( const  Message  & other ) const override
    {
        const  MessageError  * o = dynamic_cast<const  MessageError  * >( &other );
        return  o && error_type() == o->error_type();
    }

    std::ostream  & print( std::ostream  & os ) const override;
};


inline  std::ostream  & operator<<( std::ostream  & os, MessageError::ErrorType  t )
{
    os << "MessageError::ErrorType::";
    switch ( t )
    {
        case MessageError::ErrorType::NoSchedule:
            os << "NoSchedule";

            break;

        case MessageError::ErrorType::NoTimestamp:
            os << "NoTimestamp";

            break;

        case MessageError::ErrorType::Obsolete:
            os << "Obsolete";

            break;

        default:
            os << "<unknown>";
    }

    return  os;
}


inline  std::ostream  & MessageError::print( std::ostream  & os ) const
{
    return  os << "MessageError (errorType=" << error_type() << ")";
}

#endif  // MESSAGE_ERROR_H
