#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    Vector* a = NULL;
    Vector* b = NULL;
    Vector* c = NULL;

    int running = 1;
    while (running) {
        int choice = -1;
        printf("\nMenu:\n");
        printf("1) Create A\n");
        printf("2) Create B\n");
        printf("3) Fill A\n");
        printf("4) Fill B\n");
        printf("5) Print A\n");
        printf("6) Print B\n");
        printf("7) C = A + B\n");
        printf("8) S = dot(A, B)\n");
        printf("9) Print C\n");
        printf("10) Delete C\n");
        printf("0) Exit\n");

        if (!readIntPrompt("Choice: ", &choice)) {
            if (feof(stdin)) {
                printf("\nВвод завершен.\n");
                break;
            }
            printf("Ошибка ввода.\n");
            continue;
        }

        switch (choice) {
            case 0:
                running = 0;
                break;
            case 1:
                createVectorSlot(&a, "A");
                break;
            case 2:
                createVectorSlot(&b, "B");
                break;
            case 3:
                fillVectorFromInput(a, "A");
                break;
            case 4:
                fillVectorFromInput(b, "B");
                break;
            case 5:
                vectorPrint("A", a);
                break;
            case 6:
                vectorPrint("B", b);
                break;
            case 7: {
                Vector* result = vectorAdd(a, b);
                if (!result) {
                    printf("Ошибка сложения.\n");
                    break;
                }
                vectorDestroy(c);
                c = result;
                printf("C обновлен.\n");
                break;
            }
            case 8: {
                unsigned char* scalar = createScalarBuffer(a);
                if (!scalar) {
                    break;
                }

                if (!vectorDot(a, b, scalar)) {
                    free(scalar);
                    printf("Ошибка скалярного произведения.\n");
                    break;
                }

                printf("S = ");
                if (!vectorPrintValue(a, scalar)) {
                    free(scalar);
                    printf("Ошибка печати.\n");
                    break;
                }
                printf("\n");

                free(scalar);
                break;
            }
            case 9:
                vectorPrint("C", c);
                break;
            case 10:
                vectorDestroy(c);
                c = NULL;
                printf("C удален.\n");
                break;
            default:
                printf("Нет такого пункта.\n");
                break;
        }
    }

    vectorDestroy(a);
    vectorDestroy(b);
    vectorDestroy(c);
    return 0;
}
// Complex вынести в field_info.c
// Ленивый getComplex / getReal
// сделать тесты так будто это кольцо