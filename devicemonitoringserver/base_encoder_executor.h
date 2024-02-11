
/**
 *   \file     base_encoder_executor.h
 *   \version  0.1
 *   \date     2024.02.09
 */

#ifndef BASE_ENCODER_EXECUTOR_H
#define BASE_ENCODER_EXECUTOR_H


#include <string>


/**
 *   \brief  Абстрактный базовый класс для алгоритмов шифрования
 */
class BaseEncoderExecutor
{
    public:
        virtual  ~BaseEncoderExecutor() = default;

        /**
         *   \brief  Зашифровать сообщение
         *   \param  message - сообщение
         */
        virtual  std::string  encode( const  std::string&  message ) = 0;

        /**
         *   \brief  Расшифровать сообщение
         *   \param  message - сообщение
         */
        virtual  std::string  decode( const  std::string&  message ) = 0;

        /**
         *   \brief  Название алгоритма
         */
        virtual  std::string  name() const = 0;
};

#endif // BASE_ENCODER_EXECUTOR_H
