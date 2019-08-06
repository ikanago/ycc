#!/bin/bash
COLOR_VALID="\e[32;1m"
COLOR_INVALID="\e[31;1m"
COLOR_OFF="\e[m"

try() {
	expected="$1"
	input="$2"

	./bin/ycc --raw "$input" > ./obj/tmp.s
	gcc -o ./obj/tmp ./obj/tmp.s ./test/test.c
	./obj/tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo -e "${input} => ${COLOR_VALID}${actual}${COLOR_OFF}"
	else
		echo -e "${COLOR_INVALID}⛔  ${expected} expected, but got ${actual} ⛔${COLOR_OFF}"
		exit 1
	fi
}

try 42 "int main(){ return 42; }"
try 44 "int main(){ return 12 + 34 - 5 + 3; }"
try 47 "int main(){ return 5 + 6 * 7; }"
try 8 "int main(){ return ( 3 + 5 ) / (4 - 2) + (-3) * 2 + 10; }"
try 1 "int main(){ return 1 == (2 - 1); }"
try 0 "int main(){ return 3 * 2 != ((4 - 2) * 3); }"
try 1 "int main(){ return 4 * 3 <= (27 / 9 + 9); }"
try 0 "int main(){ return 3 < 5 - 3; }"
try 1 "int main(){ return 55 >= 3 * 13; }"
try 1 "int main(){ return 55 > 3 * 13; }"
try 5 "int main(){ int a = 5; return a; }"
try 5 "int main(){ int hoge = 5; return hoge; }"
try 11 "int main(){ int a = 5; int  b = 6; return a + b; }"
try 11 "int main(){ int hoge = 5; int fuga = 6; return hoge + fuga; }"
try 31 "int main(){ int hoge = 3; int fuga = 7; int piyo = hoge * fuga; hoge = 40; int bar = 4; return piyo + hoge / bar; }"
try 3 "int main(){ int  hoge = 1; int fuga = 1; if (hoge == fuga) fuga = 3; return fuga; }"
try 2 "int main(){ int  hoge = 1; int fuga = 2; if (hoge == fuga) fuga = 3; return fuga; }"
try 7 "int main(){ int  hoge = 1; int fuga = 2; if (hoge == fuga){ fuga = 3; } else { hoge = 3; fuga = 4; } return hoge + fuga; }"
try 8 "int main(){ int hoge = 3; int fuga = foo(); return hoge + fuga; }"
try 2 "int main(){ int va = 1; if(va < 0){va = 3;} else if(va > 0){va = 2;} else{va = 1;} return va; }"
try 2 "int main(){ int a = 1; if(a < 5){ if(a > 0){ a = a + 1; } else{ a = a + 2; } } return a; }"
try 4 "int main(){ int a = 1; int b = 2; if(a > 5){ if(a > 0){ a = a + 1; } } else if(a > 0){ if(b < 4){ b = b + 1; } else{ b = b + 2; } } return a + b; }"
try 6 "int main(){ int a = 1; int b = 2; int c = 3; int d = sum3(a, b, c); return d; }"
try 3 "int main(){ int a = 1; int b = 2; int c = sum2(a, b); return c; }"
try 10 "int main(){ int a = 1; int b = 2; int c = 3; int d = 4; int e = sum4(a, b, c, d); return e; }"
try 4 "int hoge(){ return 3; } int main(){ int a = 1; return a + hoge(); }"
try 4 "int hoge(){ if(1) { return 3; } else { return 2; } } int main(){ int a = 1; return a + hoge(); }"
try 3 "int add2(int a, int b){ int c = a + b; return c; } int main(){ int a = 1; int b = 2; return add2(1, 2); }"
try 21 "int add6(int a, int b, int c, int d, int e, int f){ int x = a + b + c + d + e + f; return x; } int main(){ return add6(1, 2, 3, 4, 5, 6); }"
try 8 "int fib(int n){ if(n == 0){ return 1; } if(n == 1){ return 1; } return fib(n - 1) + fib(n - 2); } int main(){ int n = 5; return fib(n); }"
try 120 "int fact(int n){ if(n == 1){ return 1; } return n * fact(n - 1); } int main(){ int n = 5; return fact(n); }"
try 15 "int main(){ int n = 0; int i = 0; while(i < 6){ n = n + i; i = i + 1; } return n; }"
try 120 "int fact(int n){ int a = 1; int i = 1; while(i <= n){ a = a * i; i = i + 1; } return a; } int main(){int a = fact(5); return a; }"
try 120 "int fact(int n){ if(n < 0){ return -1; } int r = 1; for(int i = 1; i <= n; i = i + 1){ r = r * i; } return r; } int main(){ int n = 5; return fact(n);}"
try 0 "int fact(int n){ if(n < 0){ return 0; } int r = 1; for(int i = 1; i <= n; i = i + 1){ r = r * i; } return r; } int main(){ int n = -3; return fact(n);}"
try 3 "int main(){ int a = 1; int b = 2; if(a < 2 && b > 1){ return 3; } return 2; }"
try 2 "int main(){ int a = 1; int b = 2; if(a > 2 && b > 1){ return 3; } return 2; }"
try 3 "int main(){ int a = 1; int b = 2; if(a < 2 || b > 1){ return 3; } return 2; }"
try 2 "int main(){ int a = 1; int b = 2; if(a > 2 || b < 1){ return 3; } return 2; }"
try 1 "int main(){ return 0 || 1 || 0; }"
try 0 "int main(){ return 0 || 1 && 0; }"
try 1 "int main(){ return 1 || 1 && 0; }"
try 0 "int main(){ return 1 && 1 && 0; }"
try 1 "int main(){ return 1 && 1 && 1; }"
try 2 "int main(){ int a = 1; if(!(a < 2)){ return 3; } return 2; }"
try 2 "int main(){ int a = 1; int b = 2; if(!(a < 2 && b > 1)){ return 3; } return 2; }"
try 3 "int main(){ int a = 1; int b = 2; if(!(a > 2 || b < 1)){ return 3; } return 2; }"
try 1 "int main(){ int a = 1; int *b = &a; return *b; }"
try 1 "int main(){ int a = 0; int b = 1; int *c = &a - 8; return *c; }"
try 0 "int main(){ int a = 0; int *b = &a; int **c = &b; return **c; }"
try 3 "int main(){ int a = 1; int b = 2; int *pa = &a; int *pb = &b; return *pa + *pb; }"
echo -e "${COLOR_VALID}🎉  Passed all tests. 🎉${COLOR_OFF}"
