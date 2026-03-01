#include "field_info.h"
#include <stdio.h>
#include <string.h>

static void genericZero(void* out, size_t elemSize) {
    memset(out, 0, elemSize);
}

static void doubleAdd(void* out, const void* a, const void* b) {
    *(double*)out = *(const double*)a + *(const double*)b;
}

static void doubleMul(void* out, const void* a, const void* b) {
    *(double*)out = *(const double*)a * *(const double*)b;
}

static void doublePrint(const void* value) {
    printf("%.10g", *(const double*)value);
}

static int doubleRead(void* out) {
    return scanf("%lf", (double*)out) == 1 ? 1 : 0;
}

static void complexAdd(void* out, const void* a, const void* b) {
    const Complex* ca = (const Complex*)a;
    const Complex* cb = (const Complex*)b;
    Complex* result = (Complex*)out;
    result->re = ca->re + cb->re;
    result->im = ca->im + cb->im;
}

static void complexMul(void* out, const void* a, const void* b) {
    const Complex* ca = (const Complex*)a;
    const Complex* cb = (const Complex*)b;
    Complex* result = (Complex*)out;
    result->re = ca->re * cb->re - ca->im * cb->im;
    result->im = ca->re * cb->im + ca->im * cb->re;
}

static void complexPrint(const void* value) {
    const Complex* c = (const Complex*)value;
    printf("(%.10g%+.10gi)", c->re, c->im);
}

static int complexRead(void* out) {
    Complex* c = (Complex*)out;
    return scanf("%lf %lf", &c->re, &c->im) == 2 ? 1 : 0;
}

const FieldInfo* getRealVector(void) {
    static const FieldInfo info = {
        sizeof(double),
        genericZero,
        doubleAdd,
        doubleMul,
        doublePrint,
        doubleRead
    };
    return &info;
}

const FieldInfo* getComplexVector(void) {
    static const FieldInfo info = {
        sizeof(Complex),
        genericZero,
        complexAdd,
        complexMul,
        complexPrint,
        complexRead
    };
    return &info;
}
