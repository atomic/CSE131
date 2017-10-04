#!/bin/bash

make clean
make 

if [[ "$#" -ne "2" ]] && [[ "$#" -ne "1" ]]; then
	echo "Miles Eli Jones rules."
	exit 1
fi

if [[ "$#" -eq "2" ]]; then 
	./lexer < $1 > lexer.output
	diff lexer.output $2
elif [[ "$#" -eq "1" ]] && [[ "$1" -eq "ALL" ]]; then

	MYSAMPLES=mysamples
	mkdir -p ${MYSAMPLES}

	for file in $(ls samples/*.java); do
		echo $file
		filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')
		./lexer < $file > ${MYSAMPLES}/$filename.myout
		diff ${MYSAMPLES}/$filename.myout samples/$filename.out
	done

fi
