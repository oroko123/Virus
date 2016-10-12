#!/bin/bash

#Colors:
RED='\033[0;31m'
BLU='\033[0;34m'
GRN='\033[0;32m'
YEL='\033[1;33m'
NC='\033[0m'

function run_single {
    printf "  ${BLU}[INFO] At test $1\n${NC}"
    ../main <$1.in >$1.myout 2>$1.myerr
    if [ "$(diff $1.out $1.myout)" != "" ]
    then
            printf "    ${RED}[ERR] Your stdout differs!\n${NC}";
    else
            printf "    ${GRN}[OK] Stdout ok.\n${NC}"
    fi

    if [ "$(diff $1.err $1.myerr)" != "" ]
    then
            printf "    ${RED}[ERR] Your stderr differs!\n${NC}";
    else
            printf "    ${GRN}[OK] Stderr ok\n${NC}"
    fi

}

function run_subdir {
    cd $1;
    echo;
    for j in $(cat testlist); do
        run_single $j;
    done;
    cd $2;
}

g++ -std=c++14 -Wall ../../main.cpp -o main
for i in $(cat testlist); do
    run_subdir $i $(pwd);
done;
