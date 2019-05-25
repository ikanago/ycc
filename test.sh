#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
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

try 0 0
try 42 42
try 21 '5+20-4'
try 44 "12 + 34 - 5 + 3"
try 47 "5 + 6 * 7"
try 5 "-10 + 15"
try 15 "5 * ( 9 - 6 )"
try 4 "( 3 + 5 ) / 2"
try 1 "1 == (2 - 1)"
try 0 "3 * 2 != ((4 - 2) * 3)"
echo OK
