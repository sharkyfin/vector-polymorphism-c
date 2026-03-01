#ifndef FIELD_INFO_H
#define FIELD_INFO_H

#include <stddef.h>

typedef struct Complex {
    double re;
    double im;
} Complex;

typedef void (*BinaryOperation)(void* out, const void* a, const void* b);

typedef struct FieldInfo {
    size_t elemSize;
    void (*zero)(void* out, size_t elemSize);
    BinaryOperation add;
    BinaryOperation mul;
    void (*print)(const void* value);
    int (*read)(void* out);
} FieldInfo;

const FieldInfo* getRealVector(void);
const FieldInfo* getComplexVector(void);

#endif
