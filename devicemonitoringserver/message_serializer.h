
/**
 *   \file     message_serializer.h
 *   \version  0.1
 *   \date     2024.02.08
 */

#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H


#include  <functional>
#include  <string>


class  Message;


/**
 *   \brief  Класс сериализации сообщений
 */
class  MessageSerializer
{
    public:
        /**
         * \brief Сериализовать сообщение \a message
         */
        std::string  serialize( const  Message  & message );

        /**
         *   \brief   Десериализовать сообщение
         *   \param   string - сообщение
         *   \param   callback - коллбэк для обработки десериализованного сообщения
         *   \return  false в случае ошибки
         */
        bool  deserialize( const  std::string  & string, std::function<void ( const  Message & )> callback );
};

#endif // MESSAGE_SERIALIZER_H
