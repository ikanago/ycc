/*
 * Test: string literal
 */

int scan(char *input, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%c\n", *input);
        ++input;
    }
    return 0;
}

int main() {
    scan("hello", 5);
    printf("test4.c passed.\n");
    return 0;
}