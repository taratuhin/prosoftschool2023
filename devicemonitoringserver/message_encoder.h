
/**
 *   \file     message_encoder.h
 *   \version  0.1
 *   \date     2024.02.08
 */

#ifndef MESSAGE_ENCODER_H
#define MESSAGE_ENCODER_H


#include <string>
#include <vector>


class  BaseEncoderExecutor;

/**
 *   \brief  Класс шифрования сообщений
 */
class  MessageEncoder
{
    private:
        BaseEncoderExecutor  * m_current_executor = nullptr;
        std::vector<BaseEncoderExecutor  *>  m_executors;

    public:
        MessageEncoder();
        ~MessageEncoder();

        /**
         *   \brief  Зашифровать сообщение
         *   \param  message - сообщение
         */
        std::string  encode( const  std::string  & message );

        /**
         *   \brief  Расшифровать сообщение
         *   \param  message - сообщение
         */
        std::string  decode( const  std::string  & message );

        /**
         *   \brief   Выбор алгоритма шифрования
         *   \param   name - название алгоритма шифрования
         *   \return  false в случае ошибки
         */
        bool  select_executor( const  std::string  & name );

        /**
         *   \brief   Зарегистрировать алгоритм шифрования
         *   \param  *executor - реализацию алгоритма шифрования
         *   \return  false в случае ошибки
         */
        bool  add_executor( BaseEncoderExecutor  * executor );
};

#endif // MESSAGE_ENCODER_H
