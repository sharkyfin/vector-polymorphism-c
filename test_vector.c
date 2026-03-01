#include "vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

static const double EPS = 1e-9;

typedef void (*MakeValueFunction)(void* out, double x, double y);
typedef int (*ValueEqualFunction)(const void* a, const void* b);

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
    assert(fabs(got - 4.0) < EPS);
    assert(vectorGet(c, 1, &got) == 1);
    assert(fabs(got - 2.0) < EPS);
    assert(vectorGet(c, 2, &got) == 1);
    assert(fabs(got - 3.0) < EPS);

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
    assert(fabs(dot + 0.75) < EPS);

    vectorDestroy(a);
    vectorDestroy(b);
}

static void testVectorAddComplex(void) {
    size_t complexSize = complexValueSize();
    unsigned char aValues[2 * complexSize];
    unsigned char bValues[2 * complexSize];
    unsigned char got[complexSize];

    complexMake(aValues + 0 * complexSize, 1.0, 2.0);
    complexMake(aValues + 1 * complexSize, -3.0, 0.5);
    complexMake(bValues + 0 * complexSize, 4.0, -1.0);
    complexMake(bValues + 1 * complexSize, 2.0, 3.0);

    Vector* a = makeVector(getComplexVector(), aValues, 2);
    Vector* b = makeVector(getComplexVector(), bValues, 2);
    Vector* c = vectorAdd(a, b);
    assert(c != NULL);
    assert(vectorSize(c) == 2);

    assert(vectorGet(c, 0, got) == 1);
    assert(fabs(complexGetRe(got) - 5.0) < EPS);
    assert(fabs(complexGetIm(got) - 1.0) < EPS);

    assert(vectorGet(c, 1, got) == 1);
    assert(fabs(complexGetRe(got) + 1.0) < EPS);
    assert(fabs(complexGetIm(got) - 3.5) < EPS);

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
}

static void testVectorDotComplex(void) {
    size_t complexSize = complexValueSize();
    unsigned char aValues[2 * complexSize];
    unsigned char bValues[2 * complexSize];
    unsigned char dot[complexSize];

    complexMake(aValues + 0 * complexSize, 1.0, 2.0);
    complexMake(aValues + 1 * complexSize, -3.0, 0.5);
    complexMake(bValues + 0 * complexSize, 4.0, -1.0);
    complexMake(bValues + 1 * complexSize, 2.0, 3.0);

    Vector* a = makeVector(getComplexVector(), aValues, 2);
    Vector* b = makeVector(getComplexVector(), bValues, 2);
    assert(vectorDot(a, b, dot) == 1);
    assert(fabs(complexGetRe(dot) + 1.5) < EPS);
    assert(fabs(complexGetIm(dot) + 1.0) < EPS);

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
    size_t complexSize = complexValueSize();
    double aValues[] = {1.0, 2.0, 3.0};
    unsigned char bValues[3 * complexSize];
    unsigned char dot[complexSize];

    complexMake(bValues + 0 * complexSize, 1.0, 0.0);
    complexMake(bValues + 1 * complexSize, 2.0, 0.0);
    complexMake(bValues + 2 * complexSize, 3.0, 0.0);

    Vector* a = makeVector(getRealVector(), aValues, 3);
    Vector* b = makeVector(getComplexVector(), bValues, 3);

    assert(vectorAdd(a, b) == NULL);
    assert(vectorDot(a, b, dot) == 0);

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
    assert(fabs(last - expectedLast) < EPS);

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

static void makeRealValue(void* out, double x, double y) {
    (void)y;
    *(double*)out = x;
}

static int equalRealValue(const void* a, const void* b) {
    return (fabs(*(const double*)a - *(const double*)b) < EPS) ? 1 : 0;
}

static void makeComplexValue(void* out, double x, double y) {
    complexMake(out, x, y);
}

static int equalComplexValue(const void* a, const void* b) {
    return (fabs(complexGetRe(a) - complexGetRe(b)) < EPS &&
            fabs(complexGetIm(a) - complexGetIm(b)) < EPS) ? 1 : 0;
}

static void testScalarRingAxioms(const FieldInfo* field,
                                 MakeValueFunction makeValue,
                                 ValueEqualFunction equalValue,
                                 double a1,
                                 double a2,
                                 double b1,
                                 double b2,
                                 double c1,
                                 double c2) {
    assert(field != NULL);
    assert(makeValue != NULL);
    assert(equalValue != NULL);

    size_t elemSize = field->elemSize;
    unsigned char a[elemSize];
    unsigned char b[elemSize];
    unsigned char c[elemSize];
    unsigned char zero[elemSize];
    unsigned char t1[elemSize];
    unsigned char t2[elemSize];
    unsigned char t3[elemSize];
    unsigned char t4[elemSize];

    makeValue(a, a1, a2);
    makeValue(b, b1, b2);
    makeValue(c, c1, c2);
    field->zero(zero, elemSize);

    field->add(t1, a, zero);
    assert(equalValue(t1, a) == 1);

    field->add(t1, a, b);
    field->add(t2, t1, c);
    field->add(t3, b, c);
    field->add(t4, a, t3);
    assert(equalValue(t2, t4) == 1);

    field->add(t1, a, b);
    field->add(t2, b, a);
    assert(equalValue(t1, t2) == 1);

    field->mul(t1, a, b);
    field->mul(t2, t1, c);
    field->mul(t3, b, c);
    field->mul(t4, a, t3);
    assert(equalValue(t2, t4) == 1);

    field->mul(t1, a, b);
    field->mul(t2, b, a);
    assert(equalValue(t1, t2) == 1);

    field->add(t1, b, c);
    field->mul(t2, a, t1);
    field->mul(t3, a, b);
    field->mul(t4, a, c);
    field->add(t1, t3, t4);
    assert(equalValue(t2, t1) == 1);

    field->add(t1, a, b);
    field->mul(t2, t1, c);
    field->mul(t3, a, c);
    field->mul(t4, b, c);
    field->add(t1, t3, t4);
    assert(equalValue(t2, t1) == 1);

    makeValue(t4, -a1, -a2);
    field->add(t1, a, t4);
    assert(equalValue(t1, zero) == 1);
}

static void testDotDistributive(const FieldInfo* field,
                                const void* aValues,
                                const void* bValues,
                                const void* cValues,
                                size_t size,
                                ValueEqualFunction equalValue) {
    Vector* a = makeVector(field, aValues, size);
    Vector* b = makeVector(field, bValues, size);
    Vector* c = makeVector(field, cValues, size);

    Vector* aPlusB = vectorAdd(a, b);
    assert(aPlusB != NULL);

    size_t elemSize = field->elemSize;
    unsigned char left[elemSize];
    unsigned char right1[elemSize];
    unsigned char right2[elemSize];
    unsigned char right[elemSize];

    assert(vectorDot(aPlusB, c, left) == 1);
    assert(vectorDot(a, c, right1) == 1);
    assert(vectorDot(b, c, right2) == 1);
    field->add(right, right1, right2);
    assert(equalValue(left, right) == 1);

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
    vectorDestroy(aPlusB);
}

static void testRingAxiomsReal(void) {
    const FieldInfo* field = getRealVector();
    assert(field != NULL);

    testScalarRingAxioms(field, makeRealValue, equalRealValue, 2.0, 0.0, -3.0, 0.0, 5.0, 0.0);

    double aValues[] = {1.0, 2.0, 3.0};
    double bValues[] = {4.0, -5.0, 6.0};
    double cValues[] = {-7.0, 8.0, 9.0};
    testDotDistributive(field, aValues, bValues, cValues, 3, equalRealValue);
}

static void testRingAxiomsComplex(void) {
    const FieldInfo* field = getComplexVector();
    assert(field != NULL);

    testScalarRingAxioms(field, makeComplexValue, equalComplexValue, 1.0, 2.0, -3.0, 4.0, 5.0, -6.0);

    size_t complexSize = complexValueSize();
    unsigned char aValues[3 * complexSize];
    unsigned char bValues[3 * complexSize];
    unsigned char cValues[3 * complexSize];

    complexMake(aValues + 0 * complexSize, 1.0, 2.0);
    complexMake(aValues + 1 * complexSize, -3.0, 4.0);
    complexMake(aValues + 2 * complexSize, 5.0, -6.0);

    complexMake(bValues + 0 * complexSize, 2.0, -1.0);
    complexMake(bValues + 1 * complexSize, 3.0, 0.0);
    complexMake(bValues + 2 * complexSize, -4.0, 2.0);

    complexMake(cValues + 0 * complexSize, -1.0, 3.0);
    complexMake(cValues + 1 * complexSize, 6.0, -2.0);
    complexMake(cValues + 2 * complexSize, 0.0, 1.0);

    testDotDistributive(field, aValues, bValues, cValues, 3, equalComplexValue);
}

int main(void) {
    testRingAxiomsReal();
    testRingAxiomsComplex();
    testVectorAddReal();
    testVectorDotReal();
    testLargeReal();
    testVectorAddComplex();
    testVectorDotComplex();
    testSizeMismatch();
    testTypeMismatch();
    testOob();

    printf("All vector tests passed.\n");
    fieldInfoShutdown();
    return 0;
}
