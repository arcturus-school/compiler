#include <stdio.h>

int main() {
    int a   = 50;
    int sum = -1;

    // outer loop
    for (int i = 0; i < a * 2; i++) {
        // inner loop
        for (int j = 0; j < a; j = j + 1) {
            if (j < a / 2 && j % 2 != 0) {
                sum = sum + i * j / 2 + i * j;
            } else {
                while (sum > j || !sum % 2 == 1) {
                    sum = sum - 3 / 2;
                }
            }
        }
    }

    printf("%d\n", sum);

    return 0;
}