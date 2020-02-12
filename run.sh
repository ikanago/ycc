#!/bin/bash
source_file=$1
asm_file=$(echo ${source_file} | sed -e "s/\.c$/\.s/")
obj_file=${source_file%.*}

./bin/ycc ${source_file} > ${asm_file}
gcc -g -o ${obj_file} ${asm_file}
./${obj_file}
echo $?
rm ${asm_file}
rm ${obj_file}