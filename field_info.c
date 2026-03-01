#include "field_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Complex {
    double re;
    double im;
} Complex;

static FieldInfo* realInfo = NULL;
static FieldInfo* complexInfo = NULL;

size_t complexValueSize(void) {
    return sizeof(Complex);
}

void complexMake(void* out, double re, double im) {
    if (!out) {
        return;
    }
    Complex* c = (Complex*)out;
    c->re = re;
    c->im = im;
}

double complexGetRe(const void* value) {
    if (!value) {
        return 0.0;
    }
    return ((const Complex*)value)->re;
}

double complexGetIm(const void* value) {
    if (!value) {
        return 0.0;
    }
    return ((const Complex*)value)->im;
}

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
    if (!realInfo) {
        realInfo = (FieldInfo*)malloc(sizeof(FieldInfo));
        if (!realInfo) {
            return NULL;
        }

        realInfo->elemSize = sizeof(double);
        realInfo->zero = genericZero;
        realInfo->add = doubleAdd;
        realInfo->mul = doubleMul;
        realInfo->print = doublePrint;
        realInfo->read = doubleRead;
    }

    return realInfo;
}

const FieldInfo* getComplexVector(void) {
    if (!complexInfo) {
        complexInfo = (FieldInfo*)malloc(sizeof(FieldInfo));
        if (!complexInfo) {
            return NULL;
        }

        complexInfo->elemSize = sizeof(Complex);
        complexInfo->zero = genericZero;
        complexInfo->add = complexAdd;
        complexInfo->mul = complexMul;
        complexInfo->print = complexPrint;
        complexInfo->read = complexRead;
    }

    return complexInfo;
}

void fieldInfoShutdown(void) {
    free(realInfo);
    free(complexInfo);
    realInfo = NULL;
    complexInfo = NULL;
}
