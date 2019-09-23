#!/bin/bash
COLOR_VALID="\e[32;1m"
COLOR_INVALID="\e[31;1m"
COLOR_OFF="\e[m"

# shellcheck disable=SC2034
for test_file in ./tests/*.c
do
  ./bin/ycc $test_file > ./obj/tmp.s
  gcc -static -o ./obj/tmp ./obj/tmp.s
  ./obj/tmp
  if [ $? != 0 ]; then
    echo -e "${COLOR_INVALID}Integration test failed.${COLOR_OFF}"
    exit 1
  fi
done

echo -e "${COLOR_VALID}All integrationg test passed.${COLOR_OFF}"