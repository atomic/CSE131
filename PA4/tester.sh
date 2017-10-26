#!/bin/bash

TXT_RESET=$(tput sgr 0 2> /dev/null)
TXT_RED=$(tput setaf 1 2> /dev/null)
TXT_GREEN=$(tput setaf 2 2> /dev/null)
OUTFILES=outfiles

if [ ! -f "glc" ]; then
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
        ./glc < $file > ${OUTFILES}/$filename.out 2>&1

        if cmp -s ${OUTFILES}/$filename.out samples/$filename.out; then
            echo "${TXT_GREEN}[PASSED]${TXT_RESET} $file"
        else
            echo "${TXT_RED}[FAILED]${TXT_RESET} $file"
        fi

    done

    rm -rf ${OUTFILES}
}

while true; do
    case "$1" in
        --all ) compare_all; break ;;
        *     ) usage ;;
    esac
done
