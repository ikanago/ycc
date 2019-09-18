int f() {
    int array[5];
    array[0] = 1;
    array[1] = 2;
    array[2] = 3;
    array[3] = 4;
    array[4] = 5;
    int i = 0;
    int r = 0;
    while (i < 5) {
        r += array[i];
        ++i;
    }
    return r + sizeof(array);
}

int g(int n) {
    int a[10];
    for (int i = 0; i < 10; ++i) {
        a[i] = i + 1;
    }
    int *p = a;
    int r = 0;
    for (int i = 0; i < 10; ++i) {
        r += *p;
        ++p;
    }
    return r + sizeof(p) + n;
}

int main() {
    int result = f() + g(2);
    if (result == 100) {
        printf("test2.c passed.\n");
        return 0;
    }
    else {
        printf("test2.c failed.\n");
        return 1;
    }
}