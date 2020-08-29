#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: H Mohamed Hussain
# Student No: A0199425R
# Lab Group: 10
####################

echo "Printing system call report"

# compile file
gcc -std=c99 pid_checker.c -o ex5

# use strace to get report
strace -c ./ex5
