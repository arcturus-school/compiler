#include <stdio.h>

// void abc() {
//     return 10; // incompatible types when return type 'void' using type 'int'.
// }

int fib(int a, int b) {
    if (a > 1000) {
        return a * b % 5 - -1;
    }

    return fib(a + b, a) + 1;
}

int main() {
    printf("%d\n", fib(1, 1));
    return 0;
}
