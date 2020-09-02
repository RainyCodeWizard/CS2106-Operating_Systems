#!/bin/bash

####################
# Lab 1 Exercise 4
# Name: H Mohamed Hussain
# Student No: A0199425R
# Lab Group: 10
####################

# fill the below up
hostname=$(hostname)
kernel_version=$(uname -r)
process_cnt=$(ps -e --no-headers | wc -l)
user_process_cnt=$(ps -x --no-headers | wc -l)
mem_usage=$(free | awk 'FNR==2 {print $3/$2 *100}')
swap_usage=$(free | awk 'FNR==3 {print $3/$2 *100}')

echo "Hostname: $hostname"
echo "Linux Kernel Version: $kernel_version"
echo "Total Processes: $process_cnt"
echo "User Processes: $user_process_cnt"
echo "Memory Used (%): $mem_usage"
echo "Swap Used (%): $swap_usage"
