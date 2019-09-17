int fib(int n) {
    if (n <= 1)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

int fact(int n) {
    int product = 1;
    for (int i = 2; i <= n; ++i) {
        product *= i;
    }
    return product;
}

int main() {
    int result = fib(5) + fact(4);
    if (result == 32) {
        printf("test1.c passed.\n");
        return 0;
    }
    else {
        printf("test1.c failed.\n");
        return 1;
    }
}