#!/bin/bash

#This script takes one argument, the name of a file that is to be 
#  split into smaller files
j=1
lines=0
for i in $(seq 5000 5000 $(wc -l $1 | cut -d ' ' -f1))
do
	head -${i} $1 | tail -5000 > EV$(printf %03d ${j})
	j=$((j+1))
	lines=$((lines+5000))
done

let total="$(wc -l $1 | cut -d ' ' -f1)"
let remainder=$((total-lines))
tail -${remainder} $1 > EV$(printf %03d ${j})
