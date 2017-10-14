#!/bin/bash

TXT_RESET=$(tput sgr 0 2> /dev/null)
TXT_RED=$(tput setaf 1 2> /dev/null)
TXT_GREEN=$(tput setaf 2 2> /dev/null)
MYSAMPLES=mysamples

if [ ! -f "parser" ]; then
    echo "'parser' executable does not exist. Use 'make'"
    exit 1
fi

function error_msg() {
    echo "PANIC PANIC PANIC"
    echo "Exiting..."
    exit 1
}

function compare() {
    local samples_dir=samples/
    ./parser < $samples_dir/$1.java > myoutput 2>&1
    diff myoutput $samples_dir/$1.out
    rm myoutput
}

function compare_all() {
    mkdir -p ${MYSAMPLES}

    for file in $(ls samples/*.java); do

        filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')

        ./parser < $file > ${MYSAMPLES}/$filename.out 2>&1

        if diff -s ${MYSAMPLES}/$filename.out samples/$filename.out > /dev/null; then
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
