int g_a = 1;
int g_b = 2;

int f(int a, int b, int c, int d) {
    return (a + b) * (c + d);
}

int g(int x, int y) {
    if (x < g_a) {
        return f(x, y, 3, 2);
    }
    else if (y == 2) {
        return f(x, y, g_a, g_b);
    }
    else {
        return f(x, y, x, y);
    }
}

int main() {
    int result = g(3, 2);
    if (result == 18) {
        printf("test0.c passed.\n");
        return 0;
    }
    else {
        printf("test0.c failed.\n");
        return 1;
    }
}