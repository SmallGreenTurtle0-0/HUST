#include <stdio.h>
#include <stdlib.h>

int main() {
    int *a, *b, *c;
    *a = 1; *b = 2; *c = 3;
    b = &a;
    c = a;
    puts("ok");
    printf("a:%f --> %f => %d\n", &a, a, *a);
    printf("b:%f --> %f => %d\n", &b, b, *b);
    printf("c:%f --> %f => %d\n", &c, c, *c);
    system("pause");
    return 0;
}
