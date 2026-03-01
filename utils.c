#include "utils.h"
#include "field_info.h"

#include <stdio.h>
#include <stdlib.h>

static void flushStdinLine(void) {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int readIntPrompt(const char* prompt, int* out) {
    printf("%s", prompt);
    int rc = scanf("%d", out);
    if (rc != 1) {
        if (rc != EOF) {
            flushStdinLine();
        }
        return 0;
    }
    return 1;
}

static const FieldInfo* chooseFieldFromMenu(void) {
    int choice = 0;
    printf("Select type:\n");
    printf("1) double\n");
    printf("2) complex\n");
    if (!readIntPrompt("Choice: ", &choice)) {
        return NULL;
    }

    switch (choice) {
        case 1:
            return getRealVector();
        case 2:
            return getComplexVector();
        default:
            return NULL;
    }
}

static int readSizePrompt(const char* prompt, size_t* out) {
    printf("%s", prompt);
    int rc = scanf("%zu", out);
    if (rc != 1) {
        if (rc != EOF) {
            flushStdinLine();
        }
        return 0;
    }
    return 1;
}

void fillVectorFromInput(Vector* v, const char* name) {
    if (!v) {
        printf("%s не создан.\n", name);
        return;
    }

    size_t size = vectorSize(v);
    for (size_t i = 0; i < size; ++i) {
        printf("%s[%zu] = ", name, i);
        if (!vectorReadElem(v, i)) {
            if (feof(stdin)) {
                return;
            }
            flushStdinLine();
            printf("Ошибка ввода (%zu).\n", i);
            return;
        }
    }
    printf("%s заполнен.\n", name);
}

unsigned char* createScalarBuffer(const Vector* a) {
    if (!a) {
        fprintf(stderr, "Ошибка скалярного произведения.\n");
        return NULL;
    }

    size_t scalarSize = vectorElemSize(a);
    if (scalarSize == 0) {
        fprintf(stderr, "Ошибка скалярного произведения.\n");
        return NULL;
    }

    unsigned char* scalar = (unsigned char*)malloc(scalarSize);
    if (!scalar) {
        fprintf(stderr, "Ошибка памяти.\n");
        return NULL;
    }

    return scalar;
}

void createVectorSlot(Vector** slot, const char* name) {
    if (!slot || !name) {
        return;
    }

    const FieldInfo* field = chooseFieldFromMenu();
    if (!field) {
        if (feof(stdin)) {
            return;
        }
        printf("Неизвестный тип.\n");
        return;
    }

    size_t size = 0;
    if (!readSizePrompt("Length: ", &size)) {
        if (feof(stdin)) {
            return;
        }
        printf("Неверная длина.\n");
        return;
    }
    if (size == 0) {
        printf("Длина должна быть > 0.\n");
        return;
    }

    Vector* newVector = vectorCreate(field, size);
    if (!newVector) {
        fprintf(stderr, "Ошибка создания.\n");
        return;
    }

    vectorDestroy(*slot);
    *slot = newVector;
    printf("%s создан.\n", name);
}
