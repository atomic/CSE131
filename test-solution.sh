#!/bin/bash


make clean
make 

MYSAMPLES=mysamples
mkdir -p ${MYSAMPLES}

for file in $(ls samples/*.java); do
	filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')
	./lexer < $file > ${MYSAMPLES}/$filename.myout
	diff ${MYSAMPLES}/$filename.myout samples/$filename.out
done
