#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

static Vector* makeVector(const FieldInfo* field, const void* values, size_t size) {
    Vector* v = vectorCreate(field, size);
    assert(v != NULL);

    const unsigned char* bytes = (const unsigned char*)values;
    for (size_t i = 0; i < size; ++i) {
        assert(vectorSet(v, i, bytes + i * field->elemSize) == 1);
    }
    return v;
}

static void testVectorAddReal(void) {
    double aValues[] = {1.5, -2.0, 3.25};
    double bValues[] = {2.5, 4.0, -0.25};

    Vector* a = makeVector(getRealVector(), aValues, 3);
    Vector* b = makeVector(getRealVector(), bValues, 3);
    Vector* c = vectorAdd(a, b);
    assert(c != NULL);
    assert(vectorSize(c) == 3);

    double got = 0.0;
    assert(vectorGet(c, 0, &got) == 1);
    assert(got == 4.0);
    assert(vectorGet(c, 1, &got) == 1);
    assert(got == 2.0);
    assert(vectorGet(c, 2, &got) == 1);
    assert(got == 3.0);

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
}

static void testVectorDotReal(void) {
    double aValues[] = {1.0, 2.5, 3.0};
    double bValues[] = {4.0, 0.5, -2.0};
    double dot = 0.0;

    Vector* a = makeVector(getRealVector(), aValues, 3);
    Vector* b = makeVector(getRealVector(), bValues, 3);
    assert(vectorDot(a, b, &dot) == 1);
    assert(dot == -0.75);

    vectorDestroy(a);
    vectorDestroy(b);
}

static void testVectorAddComplex(void) {
    Complex aValues[] = {{1.0, 2.0}, {-3.0, 0.5}};
    Complex bValues[] = {{4.0, -1.0}, {2.0, 3.0}};

    Vector* a = makeVector(getComplexVector(), aValues, 2);
    Vector* b = makeVector(getComplexVector(), bValues, 2);
    Vector* c = vectorAdd(a, b);
    assert(c != NULL);
    assert(vectorSize(c) == 2);

    Complex got = {0.0, 0.0};
    assert(vectorGet(c, 0, &got) == 1);
    assert(got.re == 5.0);
    assert(got.im == 1.0);

    assert(vectorGet(c, 1, &got) == 1);
    assert(got.re == -1.0);
    assert(got.im == 3.5);

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
}

static void testVectorDotComplex(void) {
    Complex aValues[] = {{1.0, 2.0}, {-3.0, 0.5}};
    Complex bValues[] = {{4.0, -1.0}, {2.0, 3.0}};
    Complex dot = {0.0, 0.0};

    Vector* a = makeVector(getComplexVector(), aValues, 2);
    Vector* b = makeVector(getComplexVector(), bValues, 2);
    assert(vectorDot(a, b, &dot) == 1);
    assert(dot.re == -1.5);
    assert(dot.im == -1.0);

    vectorDestroy(a);
    vectorDestroy(b);
}

static void testSizeMismatch(void) {
    double aValues[] = {1.0, 2.0};
    double bValues[] = {3.0, 4.0, 5.0};
    double dot = 0.0;

    Vector* a = makeVector(getRealVector(), aValues, 2);
    Vector* b = makeVector(getRealVector(), bValues, 3);

    assert(vectorAdd(a, b) == NULL);
    assert(vectorDot(a, b, &dot) == 0);

    vectorDestroy(a);
    vectorDestroy(b);
}

static void testTypeMismatch(void) {
    double aValues[] = {1.0, 2.0, 3.0};
    Complex bValues[] = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}};
    Complex dot = {0.0, 0.0};

    Vector* a = makeVector(getRealVector(), aValues, 3);
    Vector* b = makeVector(getComplexVector(), bValues, 3);

    assert(vectorAdd(a, b) == NULL);
    assert(vectorDot(a, b, &dot) == 0);

    vectorDestroy(a);
    vectorDestroy(b);
}

static void testOob(void) {
    double vals[] = {7.0, 8.0};
    Vector* v = makeVector(getRealVector(), vals, 2);

    double out = 0.0;
    assert(vectorGet(v, 2, &out) == 0);

    double newValue = 42.0;
    assert(vectorSet(v, 2, &newValue) == 0);

    vectorDestroy(v);
}

static void testLargeReal(void) {
    const size_t size = 1000000;
    Vector* a = vectorCreate(getRealVector(), size);
    Vector* b = vectorCreate(getRealVector(), size);
    assert(a != NULL);
    assert(b != NULL);

    for (size_t i = 0; i < size; ++i) {
        double x = (double)(i % 1000);
        double y = (double)(i % 200);
        assert(vectorSet(a, i, &x) == 1);
        assert(vectorSet(b, i, &y) == 1);
    }

    clock_t t0 = clock();
    Vector* c = vectorAdd(a, b);
    clock_t t1 = clock();
    assert(c != NULL);
    assert(vectorSize(c) == size);

    double last = 0.0;
    double expectedLast = (double)(((size - 1) % 1000) + ((size - 1) % 200));
    assert(vectorGet(c, size - 1, &last) == 1);
    assert(last == expectedLast);

    double dot = 0.0;
    clock_t t2 = clock();
    assert(vectorDot(a, b, &dot) == 1);
    clock_t t3 = clock();
    assert(dot > 0.0);

    printf("Large real size=%zu: add=%.6f s, dot=%.6f s\n",
           size,
           (double)(t1 - t0) / CLOCKS_PER_SEC,
           (double)(t3 - t2) / CLOCKS_PER_SEC);

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
}

int main(void) {
    testVectorAddReal();
    testVectorDotReal();
    testLargeReal();
    testVectorAddComplex();
    testVectorDotComplex();
    testSizeMismatch();
    testTypeMismatch();
    testOob();

    printf("All vector tests passed.\n");
    return 0;
}
