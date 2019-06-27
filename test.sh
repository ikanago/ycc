#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./ycc "$input" > tmp.s
	gcc -o tmp tmp.s ./test/test.c
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$expected expected, but got $actual"
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
# try 2 "va = 1; if(va < 0){va = 3;} else if(va > 0){va = 2;} else{va = 1;} return va;"
try 6 "main(){ a = 1; b = 2; c = 3; d = sum3(a, b, c); return d; }"
try 3 "main(){ a = 1; b = 2; c = sum2(a, b); return c; }"
try 10 "main(){ a = 1; b = 2; c = 3; d = 4; e = sum4(a, b, c, d); return e; }"
try 4 "hoge(){ return 3; } main(){ a = 1; return a + hoge(); }"
echo OK
