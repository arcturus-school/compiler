#include <stdio.h>

int main() {
    int a = 2;
    int b = 3;
    printf("%d\n", a);
    printf("%d\n", b);
    int* pa = &a;
    int* pb = &b;
    int  t  = *pb;
    *pb     = *pa;
    *pa     = t * *pb * *pa / *pb;
    printf("%d\n", *pa);
    printf("%d\n", *pb);
    return 0;
}
