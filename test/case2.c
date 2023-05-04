#include <stdio.h>

int main() {
    int a = 1, b = 1;
    int c;

    // int ac = 10;
    // ac = "233"; // incompatible types when assign type 'int' using type 'char*'.

    // int ab = "asas"; // incompatible types when initializing type 'int' using type 'char*'.

    // ab = 10; // use undeclared variables 'ab'.

    // &c = 10; // lvalue required as left operand of assignment

    // Fibonacci
    for (int i = 2; i < 20; i++) {
        c = a;
        a = a + b;
        b = c;
    }

    printf("%d %d\n", a, b);
    return 0;
}