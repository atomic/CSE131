#!/bin/bash

TXT_RESET=$(tput sgr 0 2> /dev/null)
TXT_RED=$(tput setaf 1 2> /dev/null)
TXT_GREEN=$(tput setaf 2 2> /dev/null)
OUTFILES=outfiles

if [ ! -f "parser" ]; then
    echo "'parser' executable does not exist. Use 'make'"
    exit 1
fi

function usage() {
    echo -e "$0 [OPTIONS]...\n"
    echo -e "OPTIONS"
    echo -e "  --all Compares all solution files"
    exit 1
}

function compare_all() {
    mkdir -p ${OUTFILES}

    for file in $(ls samples/*.java); do

        filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')
        ./parser < $file > ${OUTFILES}/$filename.out

        if diff -s ${OUTFILES}/$filename.out samples/$filename.out > /dev/null; then
            echo "${TXT_GREEN}[PASSED]${TXT_RESET} $file"
        else
            echo "${TXT_RED}[FAILED]${TXT_RESET} $file"
            if [ -n "$1" ]; then
                compare_diff ${filename}
            fi
        fi

    done

    rm -rf ${OUTFILES}
}

function compare_pattern() {
    for file in $(ls samples/$1*.java); do
        filename=$(echo $file | cut -f1 -d '.' | cut -f2 -d '/')
        printf "\n\n COMPARING  ${filename}:\n"
        compare_diff ${filename}
    done
}

function compare_diff() {
    if [[ $(which colordiff) =~ "not found" ]]; then
        diff -yW"`tput cols`" <(./parser < samples/$1.java) <(cat samples/$1.out)
        echo "Note: install colordiff for nicer output"
    else
        if [ -z "$2" ]; then col="`tput cols`"; else col="$2"; fi
        colordiff -yW "$col" <(./parser < samples/$1.java) <(cat samples/$1.out)
    fi
}

function rebuild() {
    make clean
    make > /dev/null
}

while true; do
    case "$1" in
        --all  ) compare_all; break ;;
        --alld ) compare_all $1; break ;;
        -p    ) compare_pattern $2; break ;;
        -d    ) compare_diff $2 $3; break ;;
        -r    ) rebuild; break ;;
        *     ) usage ;;
    esac
done
