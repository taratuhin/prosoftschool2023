#include "object.h"

#include <cassert>

void SharedDataHolder::join(SharedData* sharedData)
{
    assert(!m_sharedData);
    m_sharedData = sharedData;
    if (m_sharedData)
        m_sharedData->refCounter++;
}

void SharedDataHolder::detach()
{
    if (m_sharedData)
    {
        m_sharedData->refCounter--;
        if (m_sharedData->refCounter == 0)
            delete m_sharedData;
        m_sharedData = nullptr;
    }
}

Object::Object()
{
    auto* sharedData = new SharedData;
    sharedData->object = this;
    join(sharedData);
}

Object::~Object()
{
    if (m_sharedData)
        m_sharedData->object = nullptr;
    detach();
}

SafeObjectPointer::SafeObjectPointer(Object* object)
{
    if (object)
        join(object->m_sharedData);
}

SafeObjectPointer::SafeObjectPointer(const SafeObjectPointer& other) :
    SharedDataHolder()
{
    join(other->m_sharedData);
}

SafeObjectPointer::~SafeObjectPointer()
{
    detach();
}

Object* SafeObjectPointer::data() const
{
    return m_sharedData ? m_sharedData->object : nullptr;
}

SafeObjectPointer& SafeObjectPointer::operator=(const SafeObjectPointer& other)
{
    if (this != &other)
    {
        using namespace std;
        SafeObjectPointer copy(other);
        swap(m_sharedData, copy.m_sharedData);
    }
    return *this;
}
