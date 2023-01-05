#ifndef COMMON_H
#define COMMON_H

#define NON_COPYABLE(ClassName)           \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define UNUSED(x) (void)(x)

#endif // COMMON_H
