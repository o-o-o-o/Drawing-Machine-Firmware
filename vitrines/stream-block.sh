#!/bin/bash

#use this script in /etc/rc.local when there are multiple files to choose from
#  i.e. vitrine1, vitrine2, vitrine3
#The other script, stream2.sh, should be used when the image must
#  be broken into subfiles EVxxx (using splitfile.sh)

for i in $(seq 1 $(wc -l $1 | cut -d ' ' -f1))
do
	/home/pi/vitrines/equitable-send-command-block $(sed -n ${i}p $1)
	if [ $? == 0 ]
		then echo "Line number ${i} was executed" > /home/pi/vitrines/machine.log 
		else break;
	fi
done
echo "exited on line ${i}" > /home/pi/vitrines/machine.log
	
