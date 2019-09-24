/*
 * Test: add-assign operator, nested if-else, logical and/or/not
 */

int f(int n) {
    int a = 0;
    for (int i = 0; i < n; ++i) {
        a += i;
    }
    for (int j = 0; j < n; ++j) {
        a += j;
    }
    return a;
}

int g() {
    int a = f(3);
    int b = f(4);
    int c = f(5);
    int d = f(6);
    if (a < b && c != d) {
        if (a * 2 == b || c > 10)
            return 0;
        else
            return 1;
    }
    return 2;
}

int main() {
    int result = g();
    if (!result) {
        printf("test3.c passed.\n");
        return 0;
    }
    else {
        printf("test3.c failed. result = %d.\n", result);
        return 1;
    }
}