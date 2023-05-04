#include <stdio.h>

int main() {
    // int i = 10;
    // int a[1] = { 1, 2 }; // excess elements in array initializer.
    // int a[i]; // variable-sized object may not be initialized.
    // int a[]; // array size missing in 'a'
    // int a[] = { 1, 2, "as" };  // as is incompatible with the type 'int' of array.
    int b[4];
    // int b = 10; // a multiple declaration of variable 'b'.
    b[2] = 6;
    b[1] = 2;
    int c;
    c     = b[2];
    int d = b[1];
    c     = c + d;

    // c = b[c];
    // int g[-10]; // size of array 'g' is negative.

    // double ab = 10.0;
    // c         = b[ab]; // array subscript is not an integer.
    // c = b[]; // array index missing in 'b'.

    printf("%d\n", c);
    return 0;
}