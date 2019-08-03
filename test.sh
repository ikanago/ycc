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

try 42 "main(){ return 42; }"
try 44 "main(){ return 12 + 34 - 5 + 3; }"
try 47 "main(){ return 5 + 6 * 7; }"
try 8 "main(){ return ( 3 + 5 ) / (4 - 2) + (-3) * 2 + 10; }"
try 1 "main(){ return 1 == (2 - 1); }"
try 0 "main(){ return 3 * 2 != ((4 - 2) * 3); }"
try 1 "main(){ return 4 * 3 <= (27 / 9 + 9); }"
try 0 "main(){ return 3 < 5 - 3; }"
try 1 "main(){ return 55 >= 3 * 13; }"
try 1 "main(){ return 55 > 3 * 13; }"
try 5 "main(){ a = 5; return a; }"
try 5 "main(){ hoge = 5; return hoge; }"
try 11 "main(){ a = 5; b = 6; return a + b; }"
try 11 "main(){ hoge = 5; fuga = 6; return hoge + fuga; }"
try 31 "main(){ hoge = 3; fuga = 7; piyo = hoge * fuga; hoge = 40; bar = 4; return piyo + hoge / bar; }"
try 3 "main(){ hoge = 1; fuga = 1; if (hoge == fuga) fuga = 3; return fuga; }"
try 2 "main(){ hoge = 1; fuga = 2; if (hoge == fuga) fuga = 3; return fuga; }"
try 7 "main(){ hoge = 1; fuga = 2; if (hoge == fuga){ fuga = 3; } else { hoge = 3; fuga = 4; } return hoge + fuga; }"
try 8 "main(){ hoge = 3; fuga = foo(); return hoge + fuga; }"
try 2 "main(){ va = 1; if(va < 0){va = 3;} else if(va > 0){va = 2;} else{va = 1;} return va; }"
try 2 "main(){ a = 1; if(a < 5){ if(a > 0){ a = a + 1; } else{ a = a + 2; } } return a; }"
try 4 "main(){ a = 1; b = 2; if(a > 5){ if(a > 0){ a = a + 1; } } else if(a > 0){ if(b < 4){ b = b + 1; } else{ b = b + 2; } } return a + b; }"
try 6 "main(){ a = 1; b = 2; c = 3; d = sum3(a, b, c); return d; }"
try 3 "main(){ a = 1; b = 2; c = sum2(a, b); return c; }"
try 10 "main(){ a = 1; b = 2; c = 3; d = 4; e = sum4(a, b, c, d); return e; }"
try 4 "hoge(){ return 3; } main(){ a = 1; return a + hoge(); }"
try 4 "hoge(){ if(1) { return 3; } else { return 2; } } main(){ a = 1; return a + hoge(); }"
try 3 "add2(a, b){ c = a + b; return c; } main(){ a = 1; b = 2; return add2(1, 2); }"
try 21 "add6(a, b, c, d, e, f){ x = a + b + c + d + e + f; return x; } main(){ return add6(1, 2, 3, 4, 5, 6); }"
try 8 "fib(n){ if(n == 0){ return 1; } if(n == 1){ return 1; } return fib(n - 1) + fib(n - 2); } main(){ n = 5; return fib(n); }"
try 120 "fact(n){ if(n == 1){ return 1; } return n * fact(n - 1); } main(){ n = 5; return fact(n); }"
try 15 "main(){ n = 0; i = 0; while(i < 6){ n = n + i; i = i + 1; } return n; }"
try 120 "fact(n){ a = 1; i = 1; while(i <= n){ a = a * i; i = i + 1; } return a; } main(){ a = fact(5); return a; }"
try 120 "fact(n){ if(n < 0){ return -1; } r = 1; for(i = 1; i <= n; i = i + 1){ r = r * i; } return r; } main(){ n = 5; return fact(n);}"
try 0 "fact(n){ if(n < 0){ return 0; } r = 1; for(i = 1; i <= n; i = i + 1){ r = r * i; } return r; } main(){ n = -3; return fact(n);}"
try 3 "main(){ a = 1; b = 2; if(a < 2 && b > 1){ return 3; } return 2; }"
try 2 "main(){ a = 1; b = 2; if(a > 2 && b > 1){ return 3; } return 2; }"
try 3 "main(){ a = 1; b = 2; if(a < 2 || b > 1){ return 3; } return 2; }"
try 2 "main(){ a = 1; b = 2; if(a > 2 || b < 1){ return 3; } return 2; }"
try 1 "main(){ return 0 || 1 || 0; }"
try 0 "main(){ return 0 || 1 && 0; }"
try 1 "main(){ return 1 || 1 && 0; }"
try 0 "main(){ return 1 && 1 && 0; }"
try 1 "main(){ return 1 && 1 && 1; }"
try 2 "main(){ a = 1; if(!(a < 2)){ return 3; } return 2; }"
try 2 "main(){ a = 1; b = 2; if(!(a < 2 && b > 1)){ return 3; } return 2; }"
try 3 "main(){ a = 1; b = 2; if(!(a > 2 || b < 1)){ return 3; } return 2; }"
try 1 "main(){ a = 1; b = &a; return *b; }"
try 1 "main(){ a = 0; b = 1; c = &a - 8; return *c; }"
echo -e "${COLOR_VALID}🎉  Passed all test 🎉${COLOR_OFF}."
