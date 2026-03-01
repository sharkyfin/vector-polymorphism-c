#include "vector.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Vector {
    size_t size;
    const FieldInfo* field;
    unsigned char* data;
};

static int fieldIsValid(const FieldInfo* field) {
    return (field != NULL &&
            field->elemSize > 0 &&
            field->zero != NULL &&
            field->add != NULL &&
            field->mul != NULL &&
            field->print != NULL) ? 1 : 0;
}

static unsigned char* elemPtr(const Vector* v, size_t i) {
    return v->data + i * v->field->elemSize;
}

static int vectorsCompatible(const Vector* a, const Vector* b) {
    return (a && b && a->field == b->field && a->size == b->size) ? 1 : 0;
}

Vector* vectorCreate(const FieldInfo* field, size_t size) {
    if (!fieldIsValid(field) || (size > 0 && size > SIZE_MAX / field->elemSize)) {
        return NULL;
    }

    Vector* v = (Vector*)malloc(sizeof(Vector));
    if (!v) {
        return NULL;
    }

    v->size = size;
    v->field = field;
    v->data = NULL;

    if (size > 0) {
        v->data = (unsigned char*)malloc(size * v->field->elemSize);
        if (!v->data) {
            free(v);
            return NULL;
        }

        for (size_t i = 0; i < size; ++i) {
            v->field->zero(v->data + i * v->field->elemSize, v->field->elemSize);
        }
    }

    return v;
}

void vectorDestroy(Vector* v) {
    if (!v) {
        return;
    }
    free(v->data);
    free(v);
}

size_t vectorSize(const Vector* v) {
    if (!v) {
        return 0;
    }
    return v->size;
}

size_t vectorElemSize(const Vector* v) {
    if (!v || !v->field) {
        return 0;
    }
    return v->field->elemSize;
}

int vectorGet(const Vector* v, size_t i, void* outValue) {
    if (!v || !outValue || i >= v->size) {
        return 0;
    }

    memcpy(outValue, elemPtr(v, i), v->field->elemSize);
    return 1;
}

int vectorSet(Vector* v, size_t i, const void* value) {
    if (!v || !value || i >= v->size) {
        return 0;
    }

    memcpy(elemPtr(v, i), value, v->field->elemSize);
    return 1;
}

int vectorReadElem(Vector* v, size_t i) {
    if (!v || i >= v->size || !v->field || !v->field->read) {
        return 0;
    }
    return v->field->read(elemPtr(v, i)) ? 1 : 0;
}

int vectorPrintValue(const Vector* v, const void* value) {
    if (!v || !value || !v->field || !v->field->print) {
        return 0;
    }
    v->field->print(value);
    return 1;
}

Vector* vectorClone(const Vector* v) {
    if (!v) {
        return NULL;
    }

    Vector* copy = vectorCreate(v->field, v->size);
    if (!copy) {
        return NULL;
    }

    if (v->size > 0) {
        memcpy(copy->data, v->data, v->size * v->field->elemSize);
    }
    return copy;
}

void vectorPrint(const char* name, const Vector* v) {
    const char* label = name ? name : "Vector";

    if (!v) {
        printf("%s не создан.\n", label);
        return;
    }

    printf("%s (n=%zu): ", label, v->size);
    printf("[");
    for (size_t i = 0; i < v->size; ++i) {
        if (i > 0) {
            printf(", ");
        }
        v->field->print(elemPtr(v, i));
    }
    printf("]\n");
}

Vector* vectorAdd(const Vector* a, const Vector* b) {
    if (!vectorsCompatible(a, b)) {
        return NULL;
    }

    Vector* c = vectorCreate(a->field, a->size);
    if (!c) {
        return NULL;
    }

    for (size_t i = 0; i < a->size; ++i) {
        const void* av = elemPtr(a, i);
        const void* bv = elemPtr(b, i);
        void* cv = elemPtr(c, i);
        a->field->add(cv, av, bv);
    }
    return c;
}

int vectorDot(const Vector* a, const Vector* b, void* outScalar) {
    if (!outScalar || !vectorsCompatible(a, b)) {
        return 0;
    }

    size_t elemSize = a->field->elemSize;
    void* sum = malloc(elemSize);
    void* product = malloc(elemSize);
    if (!sum || !product) {
        free(sum);
        free(product);
        return 0;
    }

    a->field->zero(sum, elemSize);
    for (size_t i = 0; i < a->size; ++i) {
        const void* av = elemPtr(a, i);
        const void* bv = elemPtr(b, i);
        a->field->mul(product, av, bv);
        a->field->add(sum, sum, product);
    }

    memcpy(outScalar, sum, elemSize);
    free(sum);
    free(product);
    return 1;
}
