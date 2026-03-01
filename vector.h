#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

#include "field_info.h"

typedef struct Vector Vector;

Vector* vectorCreate(const FieldInfo* field, size_t size);
void vectorDestroy(Vector* v);

size_t vectorSize(const Vector* v);
size_t vectorElemSize(const Vector* v);
int vectorGet(const Vector* v, size_t i, void* outValue);
int vectorSet(Vector* v, size_t i, const void* value);
int vectorReadElem(Vector* v, size_t i);
int vectorPrintValue(const Vector* v, const void* value);
Vector* vectorClone(const Vector* v);
void vectorPrint(const char* name, const Vector* v);

Vector* vectorAdd(const Vector* a, const Vector* b);
int vectorDot(const Vector* a, const Vector* b, void* outScalar);

#endif
