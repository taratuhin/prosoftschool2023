#ifndef OBJECT_H
#define OBJECT_H

#include "../common.h"

#include <cstdint>
#include <utility>

class Object;

/*!
 * \brief Разделяемая структура данных.
 */
struct SharedData
{
    Object* object = nullptr; ///< Указатель на объект
    uint32_t refCounter = 0;  ///< Число ссылок на разделяемую структуру
};

/*!
 * \brief Класс с функционалом работы с разделяемыми данными.
 */
class SharedDataHolder
{
    NON_COPYABLE(SharedDataHolder)
public:
    SharedDataHolder() = default;
    /*!
     * \brief Присоединить объект к разделяемым данным.
     * \param sharedData - разделяемые данные
     */
    void join(SharedData* sharedData);
    /*!
     * \brief Отсоединить объект от разделяемых данных.
     */
    void detach();

protected:
    SharedData* m_sharedData = nullptr;
};

/*!
 * \brief Класс объекта, указатель на который можно проверить на валидность.
 */
class Object : private SharedDataHolder
{
public:
    Object();
    virtual ~Object();

    friend class SafeObjectPointer;
    friend void safeObjectPointerTest();
};

/*!
 * \brief Класс-обертка для указателя, позволяющий проверить его валидность.
 */
class SafeObjectPointer final : private SharedDataHolder
{
public:
    SafeObjectPointer(Object* object = nullptr);
    SafeObjectPointer(const SafeObjectPointer& other);
    ~SafeObjectPointer();
    /*!
     * \brief Указатель на объект.
     * \return nullptr, если объект уже был удален
     */
    Object* data() const;

    // Интерфейс, необходимый для использования объекта в контексте указателя:
    SafeObjectPointer& operator=(const SafeObjectPointer& other);
    operator Object*() const { return data(); }         // Оператор приведения типа к Object*
    operator bool() const { return data() != nullptr; } // Оператор приведения типа к bool
    Object* operator->() const { return data(); }
    Object& operator*() const { return *data(); }

    friend void safeObjectPointerTest();
};

#endif // OBJECT_H
