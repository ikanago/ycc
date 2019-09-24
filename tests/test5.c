/*
 * Tests in this file verify standard function call.
 * Each test return 0 when succeeded, 1 when failed.
 */

int test_strlen() {
    char *s = "hello";
    if (strlen(s) == 5)
        return 0;
    return 1;
}

int test_strtol() {
    char *s = "34s123s0";
    int r0 = strtol(s, &s, 10);
    ++s;
    int r1 = strtol(s, &s, 10);
    ++s;
    int r2 = strtol(s, &s, 10);
    if (r0 == 34 && r1 == 123 && r2 == 0) {
        return 0;
    }
    return 1;
}

int test_strncmp() {
    char *s = "return";
    return strncmp(s, "return", 6);
}

int test_malloc() {
    int a = 0;
    int len = 10;
    int *p = malloc(sizeof a * len);
    for (int i = 0; i < len; ++i) {
        p[i] = i + 1;
    }
    int sum = 0;
    for (int j = 0; j < len; ++j) {
        sum += p[j];
    }
    if (sum == 55)
        return 0;
    return 1;
}

int main() {
    int r0 = test_strlen();
    int r1 = test_strtol();
    int r2 = test_strncmp();
    int r3 = test_malloc();
    // Succeed when all return value are 0
    if (r0 + r1 + r2 + r3 == 0)
        return 0;
    return 1;
}
