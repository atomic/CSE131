#! /bin/bash
#
# test.sh
# Copyright (C) 2017 Tony Lim <twsalim@ucsd.edu>
#
# Distributed under terms of the MIT license.
#

make;
./lexer < samples/$1.java > temp.out; diff temp.out samples/$1.out
