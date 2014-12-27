#!/bin/bash

#This script will send the stepper motor instructions from the files
#  EV001, EV002, ...
#We do this because if the stepper motor instriiction file is too long
#  then calling 'sed -n ${i}p ${j}' takes too long an introduces a delay
#  between successive stepper motor instruction executions

for j in $(ls /home/pi/vitrines/EV*)
do
	for i in $(seq 1 $(wc -l ${j} | cut -d ' ' -f1))
	do
		/home/pi/vitrines/equitable-send-command-block $(sed -n ${i}p ${j})
		if [ $? == 0 ]
			then echo "Line number ${i} was executed" > /home/pi/vitrines/machine${j}.log 
			else break;
		fi

	done
	echo "exited on line ${i}" > /home/pi/vitrines/machine.log
done	
