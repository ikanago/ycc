int foo() {
    return 5;
}

int sum2(int a, int b) {
    return a + b;
}

int sum3(int a, int b, int c) {
    return a + b + c;
}

int sum4(int a, int b, int c, int d) {
    return a + b + c + d;
}

void alloc4(int **p, int a, int b, int c, int d) {
    int array[] = {a, b, c, d};
    *p = &array[0];
}