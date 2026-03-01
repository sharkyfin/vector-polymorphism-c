#ifndef UTILS_H
#define UTILS_H

#include "vector.h"

int readIntPrompt(const char* prompt, int* out);
void fillVectorFromInput(Vector* v, const char* name);
void createVectorSlot(Vector** slot, const char* name);
unsigned char* createScalarBuffer(const Vector* a);

#endif
