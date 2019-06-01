#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./ycc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$expected expected, but got $actual"
		exit 1
	fi
}

try 42 "42;"
try 44 "12 + 34 - 5 + 3;"
try 47 "5 + 6 * 7;"
try 8 "( 3 + 5 ) / (4 - 2) + (-3) * 2 + 10;"
try 1 "1 == (2 - 1);"
try 0 "3 * 2 != ((4 - 2) * 3);"
try 1 "4 * 3 <= (27 / 9 + 9);"
try 0 "3 < 5 - 3;"
try 1 "55 >= 3 * 13;"
try 1 "55 > 3 * 13;"
try 5 "a = 5; return a;"
try 5 "hoge = 5; return hoge;"
try 11 "a = 5; b = 6; return a + b;"
try 11 "hoge = 5; fuga = 6; return hoge + fuga;"
try 31 "hoge = 3; fuga = 7; piyo = hoge * fuga; hoge = 40; bar = 4; return piyo + hoge / bar;"
echo OK
