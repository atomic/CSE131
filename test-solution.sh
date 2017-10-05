#!/bin/bash

TXT_RESET=$(tput sgr 0 2> /dev/null)
TXT_RED=$(tput setaf 1 2> /dev/null)
TXT_GREEN=$(tput setaf 2 2> /dev/null)
MYSAMPLES=mysamples

make clean
make 

function error_msg() {
	echo "Miles Eli Jones rules! Also you broke the script."
	echo "Exiting..."
	exit 1
}

function compare() {
	local samples_dir=samples/
	./lexer < $samples_dir/$1.java > lexer.output
	vimdiff lexer.output $samples_dir/$1.out
	rm lexer.output
}

function compare_all() {
	mkdir -p ${MYSAMPLES}

	for file in $(ls samples/*.java); do

		filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')

		if [ $filename = "unrecognized_char" ] || [ $filename = "unterminated_comment" ]; then
			./lexer < $file 2> ${MYSAMPLES}/$filename.myout
		else
			./lexer < $file > ${MYSAMPLES}/$filename.myout
		fi

		if diff -s ${MYSAMPLES}/$filename.myout samples/$filename.out > /dev/null; then
			echo "${TXT_GREEN}[PASSED]${TXT_RESET} $file"
		else
			echo "${TXT_RED}[FAILED]${TXT_RESET} $file"
		fi

	done

	rm -rf ${MYSAMPLES}
}

while true; do
	case "$1" in
		-s | --single ) compare $2; break ;;
		--all ) compare_all; break ;;
		*     ) error_msg ;;
	esac
done
